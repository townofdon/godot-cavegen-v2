#ifndef GEN_UTIL_HPP
#define GEN_UTIL_HPP

#include "godot_cpp/classes/noise.hpp"
#include "godot_cpp/core/math.hpp"

namespace godot {

//
// math functions
//
inline float absf(float x) {
	return std::abs(x);
}
inline float minf(float a, float b) {
	return std::min(a, b);
}
inline float maxf(float a, float b) {
	return std::max(a, b);
}
inline float clampf(float x, float minv, float maxv) {
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
		bool Interpolate;
		bool RemoveOrphans;
		// room border
		bool UseBorderNoise;
		int BorderSize;
		float SmoothBorderNoise;
		float FalloffNearBorder;
	};
	Config cfg;
	Noise noise;
	Noise borderNoise;
	Vector3i numCells;
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
	ERR_FAIL_INDEX_V_MSG(i, (numCells.x * numCells.y * numCells.z), 0, "noise index out of bounds");
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
		y <= ctx.cfg.BorderSize ||
		z <= ctx.cfg.BorderSize ||
		x >= ctx.numCells.x - 1 - ctx.cfg.BorderSize ||
		z >= ctx.numCells.z - 1 - ctx.cfg.BorderSize);
}

inline bool IsAtBorderEdge(Context ctx, int x, int y, int z) {
	return (
		x == 1 ||
		y == 1 ||
		z == 1 ||
		x == ctx.numCells.x - 2 ||
		z == ctx.numCells.z - 2);
}

inline int DistFromBorder(Context ctx, int x, int y, int z) {
	auto BorderSize = ctx.cfg.BorderSize;
	auto numCells = ctx.numCells;
	int distX = minf(absf(x - BorderSize), absf(numCells.x - 1 - BorderSize - x));
	int distY = absf(y - BorderSize);
	int distZ = minf(absf(z - BorderSize), absf(numCells.z - 1 - BorderSize - z));
	return minf(minf(distX, distY), distZ);
}

inline bool IsBelowCeiling(Context ctx, int y) {
	return ctx.cfg.Ceiling >= 1 || y <= GetCeiling(ctx);
}

// TODO: replace with flood-fill from known border
inline bool IsPointOrphan(Context ctx, float noiseSamples[], int x, int y, int z) {
	if (!ctx.cfg.RemoveOrphans)
		return false;
	// // walk down from y to slightly below the ceiling, checking if any gaps
	// for (int y2 = y - 1; y2 >= floor(ctx.numCells.y * ctx.cfg.Ceiling) - 2; y2--) {
	// 	auto val = noiseSamples[NoiseIndex(ctx, x, y, z)];
	// 	auto active = val > ctx.cfg.IsoValue;
	// 	if (!active)
	// 		return true;
	// }
	return false;
}

inline bool IsPointActive(Context ctx, float noiseSamples[], int x, int y, int z) {
	auto val = noiseSamples[NoiseIndex(ctx, x, y, z)];
	auto active = val >= ctx.cfg.IsoValue;
	if (active && !IsBelowCeiling(ctx, y) && IsPointOrphan(ctx, noiseSamples, x, y, z)) {
		active = false;
	}
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
	bool aBound = IsAtBoundaryXZ(ctx, a.x, a.z);
	bool bBound = IsAtBoundaryXZ(ctx, b.x, b.z);
	bool onSamePlane = a.x == b.x || a.z == b.z;
	if (aBound && !(aBound && bBound && onSamePlane)) {
		return Vector3(a);
	}
	if (bBound && !(aBound && bBound && onSamePlane)) {
		return Vector3(b);
	}
	if (!ctx.cfg.Interpolate) {
		return (Vector3(a) + Vector3(b)) * 0.5f;
	}
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
	return p;
}

} //namespace MG

} //namespace godot

#endif // GEN_UTIL_HPP
