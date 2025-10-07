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
inline float clamp(float x, float minv, float maxv) {
	return std::max(minv, std::min(x, maxv));
}
inline float lerp(float a, float b, float t) {
	return a + t * (b - a);
}
inline float inverse_lerp(float a, float b, float p_value) {
	if (a == b) [[unlikely]] {
		return 0.0f;
	} else [[likely]] {
		return (p_value - a) / (b - a);
	}
}

namespace GenUtil {

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
		int FalloffNearBorder;
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
	return clamp(cfg.Ceiling * (numCells.y - 1), 1.0f, (float)numCells.y - 2);
}

inline float GetFloorToCeilAmount(Context ctx, int y) {
	float ceiling = GetCeiling(ctx);
	return (float)y / ceiling;
}

inline float GetAboveCeilAmount(Context ctx, int y) {
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
	maxY = Math::lerp(ceiling, maxY, cfg.FalloffAboveCeiling);
	if (y < ceiling) {
		return 0.0f;
	}
	if (y >= maxY) {
		return 1.0f;
	}
	if (ceiling >= maxY || Math::is_zero_approx(absf(ceiling - maxY))) {
		return 1.0f;
	}
	return Math::clamp(Math::inverse_lerp(ceiling, maxY, y), 0.0f, 1.0f);
}

inline float GetAboveCeilAmount2(Context ctx, int y) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	// if (cfg.Ceiling >= 1) return 0.0f;
	float high_ceiling_mask = 1.0f - float(cfg.Ceiling >= 1);
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	// if (Math::is_zero_approx(absf(ceiling - maxY))) return 0.0f;
	float zero_approx_mask = 1.0f - float(Math::is_zero_approx(absf(ceiling - maxY)));
	maxY = lerp(ceiling, maxY, cfg.FalloffAboveCeiling);
	// if (y < ceiling) return 0.0f;
	float below_ceiling_mask = 1.0f - float(y < ceiling);
	// if (y >= maxY) return 1.0f;
	float y_above_max_t = float(y >= maxY);
	// if (Math::is_zero_approx(absf(ceiling - maxY))) return 1.0f;
	float ceiling_at_max_y_t = float(Math::is_zero_approx(absf(ceiling - maxY)));
	// if (ceiling >= maxY) return 1.0f;
	float ceiling_above_max_t = float(ceiling >= maxY);
	// clang-format off
	return clamp(inverse_lerp(ceiling, maxY, y), 0.0f, 1.0f)
		* high_ceiling_mask * zero_approx_mask * below_ceiling_mask
		* (1.0f - y_above_max_t) * (1.0f - ceiling_above_max_t) * (1.0f - ceiling_at_max_y_t)
		+ y_above_max_t * ceiling_above_max_t * ceiling_at_max_y_t;
	// clang-format on
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
	if (ctx.cfg.Ceiling >= 1) {
		return true;
	}
	return y <= GetCeiling(ctx);
}

} // namespace GenUtil

} //namespace godot

#endif // GEN_UTIL_HPP
