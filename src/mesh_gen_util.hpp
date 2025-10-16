#ifndef GEN_UTIL_HPP
#define GEN_UTIL_HPP

#include "godot_cpp/classes/noise.hpp"
#include "godot_cpp/core/math.hpp"
#include "room_config.h"

namespace godot {

//
// math functions
//
inline float absf(float x) {
	return std::abs(x);
}
inline int absi(int x) {
	return std::abs(x);
}
inline float minf(float a, float b) {
	return std::min(a, b);
}
inline int mini(int a, int b) {
	return std::min(a, b);
}
inline float maxf(float a, float b) {
	return std::max(a, b);
}
inline int maxi(int a, int b) {
	return std::max(a, b);
}
inline float clampf(float x, float minv, float maxv) {
	return std::max(minv, std::min(x, maxv));
}
inline int clampi(int x, int minv, int maxv) {
	return std::max(minv, std::min(x, maxv));
}
inline float clamp01(float x) {
	return std::max(0.0f, std::min(x, 1.0f));
}
inline float lerpf(float a, float b, float t) {
	return a + t * (b - a);
}
inline float inverse_lerpf(float a, float b, float p_value) {
	if (a == b) [[unlikely]] {
		return 0.0f;
	} else [[likely]] {
		return (p_value - a) / (b - a);
	}
}

//
// MeshGen Utilities
//
namespace MG {

struct Context {
	struct Config {
		// global
		float RoomWidth;
		float RoomHeight;
		float RoomDepth;
		float CellSize;
		float Ceiling;
		float ActivePlaneOffset;
		// debug
		bool ShowNoise;
		bool ShowBorder;
		bool ShowOuterWalls;
		// room noise
		float IsoValue;
		float Curve;
		float Tilt;
		float FalloffAboveCeiling;
		float Interpolate;
		bool RemoveOrphans;
		// room border
		bool UseBorderNoise;
		int BorderSize;
		float BorderNoiseIsoValue;
		float SmoothBorderNoise;
		float FalloffNearBorder;
		int BorderGapSpread;
		// tiles
		float TileStrength;
		float TileSmoothing;
		float TileFloor;
		float TileFloorFalloff;
		float TileCeiling;
		float TileCeilingFalloff;
	};
	Config cfg;
	Noise noise;
	Noise borderNoise;
	Vector3i numCells;
	int *tiles;
};

inline int NoiseIndex(Context ctx, int x, int y, int z) {
	auto numCells = ctx.numCells;
	int i = x + y * numCells.x + z * numCells.y * numCells.x;
	// debug
	// if (i >= (numCells.x * numCells.y * numCells.z)) {
	// 	auto ax = String(std::to_string(x).c_str());
	// 	auto ay = String(std::to_string(y).c_str());
	// 	auto az = String(std::to_string(z).c_str());
	// 	auto bx = String(std::to_string(numCells.x).c_str());
	// 	auto by = String(std::to_string(numCells.y).c_str());
	// 	auto bz = String(std::to_string(numCells.z).c_str());
	// 	UtilityFunctions::printerr("noise index out of bounds: (" + ax + "," + ay + "," + az + "), numCells((" + bx + "," + by + "," + bz + ")");
	// 	return -1;
	// }
	ERR_FAIL_INDEX_V_EDMSG(i, (numCells.x * numCells.y * numCells.z), 0, "noise index out of bounds");
	return i;
}

inline float GetCeiling(Context ctx) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	return clampf(cfg.Ceiling * (float)(numCells.y - 1), 1.0f, (float)(numCells.y - 2));
}

inline float GetFloorToCeilAmount(Context ctx, int y) {
	float ceiling = GetCeiling(ctx);
	return (float)y / ceiling;
}

inline float GetAboveCeilAmount(Context ctx, int y, float scale) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	if (cfg.Ceiling >= 1) {
		return 0.0f;
	}
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	if (Math::is_zero_approx(absf(ceiling - maxY))) {
		return 0.0f;
	}
	maxY = lerpf(ceiling, maxY, clamp01(scale));
	if (y < ceiling) {
		return 0.0f;
	}
	if (y >= maxY) {
		return 1.0f;
	}
	if (ceiling >= maxY || Math::is_zero_approx(absf(ceiling - maxY))) {
		return 1.0f;
	}
	return clamp01(inverse_lerpf(ceiling, maxY, y));
}

inline float GetAboveCeilAmount2(Context ctx, int y, float scale) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	// if (cfg.Ceiling >= 1) return 0.0f;
	float high_ceiling_mask = 1.0f - float(cfg.Ceiling >= 1);
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	// if (Math::is_zero_approx(absf(ceiling - maxY))) return 0.0f;
	float zero_approx_mask = 1.0f - float(Math::is_zero_approx(absf(ceiling - maxY)));
	maxY = lerpf(ceiling, maxY, scale);
	// if (y < ceiling) return 0.0f;
	float below_ceiling_mask = 1.0f - float(y < ceiling);
	// if (y >= maxY) return 1.0f;
	float y_above_max_t = float(y >= maxY);
	// if (Math::is_zero_approx(absf(ceiling - maxY))) return 1.0f;
	float ceiling_at_max_y_t = float(Math::is_zero_approx(absf(ceiling - maxY)));
	// if (ceiling >= maxY) return 1.0f;
	float ceiling_above_max_t = float(ceiling >= maxY);
	// clang-format off
	return clamp01(inverse_lerpf(ceiling, maxY, y))
		* high_ceiling_mask * zero_approx_mask * below_ceiling_mask
		* (1.0f - y_above_max_t) * (1.0f - ceiling_above_max_t) * (1.0f - ceiling_at_max_y_t)
		+ y_above_max_t * ceiling_above_max_t * ceiling_at_max_y_t;
	// clang-format on
}

inline int GetAboveCeilCount(Context ctx, int y) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	float ceiling = GetCeiling(ctx);
	return (int)maxf(y - ceiling, 0);
}

inline bool IsAtBoundaryXZ(Context ctx, int x, int z) {
	return (
		x == 0 ||
		z == 0 ||
		x == ctx.numCells.x - 1 ||
		z == ctx.numCells.z - 1);
}

inline bool IsAtBoundaryY(Context ctx, int y) {
	return (
		y == 0 ||
		y == ctx.numCells.y - 1);
}

inline bool IsAtBorder(Context ctx, int x, int y, int z) {
	return (
		x <= ctx.cfg.BorderSize ||
		y <= 1 ||
		z <= ctx.cfg.BorderSize ||
		x >= ctx.numCells.x - 1 - ctx.cfg.BorderSize ||
		z >= ctx.numCells.z - 1 - ctx.cfg.BorderSize);
}

inline bool IsAtBorderEdge(Context ctx, int x, int y, int z) {
	return (
		x <= 1 ||
		y <= 1 ||
		z <= 1 ||
		x >= ctx.numCells.x - 2 ||
		z >= ctx.numCells.z - 2);
}

inline int DistFromBorder(Context ctx, int x, int y, int z, int BorderSize) {
	auto numCells = ctx.numCells;
	int distX = minf(absf(x - BorderSize), absf(numCells.x - 1 - BorderSize - x));
	int distZ = minf(absf(z - BorderSize), absf(numCells.z - 1 - BorderSize - z));
	return minf(distX, distZ);
}

inline bool IsBelowCeiling(Context ctx, int y) {
	return ctx.cfg.Ceiling >= 1 || y <= GetCeiling(ctx);
}

inline bool IsPointActive(Context ctx, float noiseSamples[], int x, int y, int z) {
	auto val = noiseSamples[NoiseIndex(ctx, x, y, z)];
	auto active = val >= ctx.cfg.IsoValue;
	return active;
}

inline int GetTriangulation(Context ctx, float noiseSamples[], int x, int y, int z) {
	int idx = 0;
	idx |= (IsPointActive(ctx, noiseSamples, x + 0, y + 0, z + 0) ? 1 : 0) << 0;
	idx |= (IsPointActive(ctx, noiseSamples, x + 0, y + 0, z + 1) ? 1 : 0) << 1;
	idx |= (IsPointActive(ctx, noiseSamples, x + 1, y + 0, z + 1) ? 1 : 0) << 2;
	idx |= (IsPointActive(ctx, noiseSamples, x + 1, y + 0, z + 0) ? 1 : 0) << 3;
	idx |= (IsPointActive(ctx, noiseSamples, x + 0, y + 1, z + 0) ? 1 : 0) << 4;
	idx |= (IsPointActive(ctx, noiseSamples, x + 0, y + 1, z + 1) ? 1 : 0) << 5;
	idx |= (IsPointActive(ctx, noiseSamples, x + 1, y + 1, z + 1) ? 1 : 0) << 6;
	idx |= (IsPointActive(ctx, noiseSamples, x + 1, y + 1, z + 0) ? 1 : 0) << 7;
	return idx;
}

// source: https://cs.stackexchange.com/a/71116
inline Vector3 InterpolateMeshPoints(Context ctx, float noiseSamples[], Vector3i a, Vector3i b) {
	// if one of the points is on a boundary plane, return that point so that our room meshes line up perfectly.
	// note that a and b here are corners of the cube.
	// bool aBound = IsAtBoundaryXZ(ctx, a.x, a.z);
	// bool bBound = IsAtBoundaryXZ(ctx, b.x, b.z);
	// bool onSamePlane = a.x == b.x || a.z == b.z;
	// if (aBound && !(aBound && bBound && onSamePlane)) {
	// 	return (Vector3(a) + Vector3(b)) * 0.5f;
	// 	return Vector3(a);
	// }
	// if (bBound && !(aBound && bBound && onSamePlane)) {
	// 	return (Vector3(a) + Vector3(b)) * 0.5f;
	// 	return Vector3(b);
	// }
	float isovalue = ctx.cfg.IsoValue;
	float noise_a = noiseSamples[NoiseIndex(ctx, a.x, a.y, a.z)];
	float noise_b = noiseSamples[NoiseIndex(ctx, b.x, b.y, b.z)];
	if (Math::is_zero_approx(absf(isovalue - noise_a))) {
		return Vector3(a);
	}
	if (Math::is_zero_approx(absf(isovalue - noise_b))) {
		return Vector3(b);
	}
	if (Math::is_zero_approx(absf(noise_a - noise_b))) {
		return (Vector3(a) + Vector3(b)) * 0.5f;
	}
	float mu = (isovalue - noise_a) / (noise_b - noise_a);
	mu = clamp01(mu);
	auto p = Vector3(0, 0, 0);
	p.x = a.x + mu * (b.x - a.x);
	p.y = a.y + mu * (b.y - a.y);
	p.z = a.z + mu * (b.z - a.z);
	auto avg = (Vector3(a) + Vector3(b)) * 0.5f;
	return avg.lerp(p, clamp01(ctx.cfg.Interpolate));
}

inline Vector3 ClampMeshBorderPoint(Context ctx, Vector3 point) {
	Vector3 min = Vector3(
		1,
		1,
		1);
	Vector3 max = Vector3(
		ctx.numCells.x - 2,
		ctx.numCells.y - 2,
		ctx.numCells.z - 2);
	// clang-format off
	if (point.x < min.x) point.x = min.x;
	if (point.y < min.y) point.y = min.y;
	if (point.z < min.z) point.z = min.z;
	if (point.x > max.x) point.x = max.x;
	if (point.y > max.y) point.y = max.y;
	if (point.z > max.z) point.z = max.z;
	// clang-format on
	return point;
}

// given (x, z) in 3d noise space, return the corresponding tile from 2d tile space
inline RoomConfig::TileState GetTile(Context ctx, int x, int z, RoomConfig::TileState defaultVal = RoomConfig::TileState::TILE_STATE_UNSET) {
	// tiles exclude room bounds, which is empty space
	int sx = ctx.numCells.x - 2;
	int sz = ctx.numCells.z - 2;
	int i = x - 1 + (z - 1) * sx;
	if (x - 1 < 0 || x - 1 >= sx ||
		z - 1 < 0 || z - 1 >= sz ||
		i < 0 || i >= sx * sz ||
		i > MAX_NOISE_NODES_2D) {
		return defaultVal;
	}
	int tile = ctx.tiles[i];
	if (tile < 0 || tile >= RoomConfig::TileState::_TILE_STATE_MAX_) {
		return defaultVal;
	}
	return static_cast<RoomConfig::TileState>(tile);
}

// given (x, z) in 3d noise space, return the corresponding border tile (if present) from 2d tile space
inline RoomConfig::TileState GetBorderTile(Context ctx, int x, int z) {
	if (x <= ctx.cfg.BorderSize) {
		x = 1;
	} else if (x >= ctx.numCells.x - 1 - ctx.cfg.BorderSize) {
		x = ctx.numCells.x - 2;
	}
	if (z <= ctx.cfg.BorderSize) {
		z = 1;
	} else if (z >= ctx.numCells.z - 1 - ctx.cfg.BorderSize) {
		z = ctx.numCells.z - 2;
	}
	return GetTile(ctx, x, z);
}

inline bool IsBorderTile(Context ctx, int x, int z) {
	if (x <= ctx.cfg.BorderSize) {
		x = 1;
	} else if (x >= ctx.numCells.x - 1 - ctx.cfg.BorderSize) {
		x = ctx.numCells.x - 2;
	}
	if (z <= ctx.cfg.BorderSize) {
		z = 1;
	} else if (z >= ctx.numCells.z - 1 - ctx.cfg.BorderSize) {
		z = ctx.numCells.z - 2;
	}
	return (
		x == 1 || x == ctx.numCells.x - 2 ||
		z == 1 || z == ctx.numCells.z - 2);
}

inline int GetDistanceToEmptyBorderTile(Context ctx, int x, int z, int spread) {
	if (x <= ctx.cfg.BorderSize) {
		x = 1;
	} else if (x >= ctx.numCells.x - 1 - ctx.cfg.BorderSize) {
		x = ctx.numCells.x - 2;
	}
	if (z <= ctx.cfg.BorderSize) {
		z = 1;
	} else if (z >= ctx.numCells.z - 1 - ctx.cfg.BorderSize) {
		z = ctx.numCells.z - 2;
	}
	if (GetTile(ctx, x, z) == RoomConfig::TILE_STATE_EMPTY) {
		return 0;
	}
	bool isXBorder = x == 1 || x == ctx.numCells.x - 2;
	bool isZBorder = z == 1 || z == ctx.numCells.z - 2;
	for (size_t i = 1; i <= spread; i++) {
		if ((isZBorder && GetTile(ctx, x + i, z) == RoomConfig::TILE_STATE_EMPTY) ||
			(isZBorder && GetTile(ctx, x - i, z) == RoomConfig::TILE_STATE_EMPTY) ||
			(isXBorder && GetTile(ctx, x, z + i) == RoomConfig::TILE_STATE_EMPTY) ||
			(isXBorder && GetTile(ctx, x, z - i) == RoomConfig::TILE_STATE_EMPTY)) {
			return i;
		}
	}
	return INT_MAX;
}

inline int GetActivePlaneY(Context ctx) {
	int ceiling = round(GetCeiling(ctx)) + 1;
	int activeY = ceiling - ctx.cfg.ActivePlaneOffset;
	return maxi(activeY, 2);
}

} //namespace MG

} //namespace godot

#endif // GEN_UTIL_HPP
