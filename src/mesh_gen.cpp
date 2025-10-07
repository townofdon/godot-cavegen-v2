#include "mesh_gen.h"
#include "easing.hpp"
// #include "godot_cpp/core/math.hpp"

#include "constants.h"
#include <chrono>
#include <iostream>
#include <string>

void MeshGen::_bind_methods() {
	ClassDB::bind_method(D_METHOD("generate", "global_cfg", "room_cfg", "noise", "border_noise"), &MeshGen::generate);
}

float *MeshGen::time_processNoise = new float[100]{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void MeshGen::generate(GlobalConfig *p_global_cfg, RoomConfig *p_room_cfg, Noise *p_noise, Noise *p_border_noise) {
	if (p_global_cfg == nullptr) {
		UtilityFunctions::printerr("global_cfg cannot be null");
		return;
	}
	if (p_room_cfg == nullptr) {
		UtilityFunctions::printerr("room_cfg cannot be null");
		return;
	}
	if (p_noise == nullptr) {
		UtilityFunctions::printerr("noise cannot be null");
		return;
	}
	if (p_border_noise == nullptr) {
		UtilityFunctions::printerr("border_noise cannot be null");
		return;
	}
	if (p_global_cfg->CellSize <= 0) {
		UtilityFunctions::printerr("cell_size must be greater than zero");
		return;
	}

	GlobalConfig cfg = *p_global_cfg;
	RoomConfig room = *p_room_cfg;
	struct Vector3i numCells;
	numCells.x = (int)floor(cfg.RoomWidth / cfg.CellSize);
	numCells.y = (int)floor(cfg.RoomHeight / cfg.CellSize);
	numCells.z = (int)floor(cfg.RoomDepth / cfg.CellSize);
	if (numCells.x <= 0 || numCells.y <= 0 || numCells.z <= 0) {
		auto x = String(std::to_string(numCells.x).c_str());
		auto y = String(std::to_string(numCells.y).c_str());
		auto z = String(std::to_string(numCells.z).c_str());
		UtilityFunctions::printerr("invalid numCells: (" + x + "," + y + "," + z + ")");
		return;
	}
	if (numCells.x * numCells.y * numCells.z > MAX_NUM_CELLS) {
		auto x = String(std::to_string(numCells.x).c_str());
		auto y = String(std::to_string(numCells.y).c_str());
		auto z = String(std::to_string(numCells.z).c_str());
		auto sum = String(std::to_string(numCells.x * numCells.y * numCells.z).c_str());
		UtilityFunctions::printerr("numCells too large: (" + x + "," + y + "," + z + "), total=" + sum);
		return;
	}

	// setup context
	struct MeshGen::Context::Config ctxCfg = {
		// global
		cfg.RoomWidth,
		cfg.RoomHeight,
		cfg.RoomDepth,
		cfg.CellSize,
		cfg.Ceiling,
		cfg.ActivePlaneOffset,
		// debug
		room.ShowNoise,
		room.ShowBorder,
		room.ShowOuterWalls,
		// noise
		room.IsoValue,
		room.Curve,
		room.Tilt,
		room.FalloffAboveCeiling,
		room.Interpolate,
		room.RemoveOrphans,
		// border
		room.UseBorderNoise,
		room.BorderSize,
		room.SmoothBorderNoise,
		room.FalloffNearBorder,
	};
	struct MeshGen::Context ctx = {
		ctxCfg,
		*p_noise,
		*p_border_noise,
		numCells,
	};

	// initialize noiseSamples
	// float noiseSamples[numCells.x * numCells.y * numCells.z];
	float *noiseSamples = new float[numCells.x * numCells.y * numCells.z];

	auto t0 = std::chrono::high_resolution_clock::now();
	process_noise(ctx, noiseSamples);
	auto t1 = std::chrono::high_resolution_clock::now();

	// benchmark process_noise()
	{
		float millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
		UtilityFunctions::print("process_noise() took " + String(std::to_string(millis).c_str()) + "ms");
		float minv = INFINITY;
		float maxv = -INFINITY;
		float total = 0.0f;
		int count = 0;
		for (int i = 99; i >= 0; i--) {
			if (i <= 0) {
				time_processNoise[0] = millis;
			} else {
				time_processNoise[i] = time_processNoise[i - 1];
			}
			if (time_processNoise[i] > 0) {
				total += time_processNoise[i];
				count++;
				if (time_processNoise[i] > maxv) {
					maxv = time_processNoise[i];
				}
				if (time_processNoise[i] < minv) {
					minv = time_processNoise[i];
				}
			}
		}
		if (count > 0) {
			float avg = total / (float)count;
			auto dmin = String(std::to_string(minv).c_str());
			auto dmax = String(std::to_string(maxv).c_str());
			auto davg = String(std::to_string(avg).c_str());
			UtilityFunctions::print("> avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}

	delete[] noiseSamples;
}

// note - this mutates noiseSamples
void MeshGen::process_noise(MeshGen::Context ctx, float noiseSamples[]) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	auto noise = ctx.noise;
	float *noiseBuffer = new float[numCells.x * numCells.y * numCells.z];
	float minV = INFINITY;
	float maxV = -INFINITY;
	float cellSize = ctx.cfg.CellSize;
	// first pass - initialize && sample all noise values in grid
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = NoiseIndex(ctx, x, y, z);
				noiseBuffer[i] = 0.0f;
				noiseSamples[i] = 0.0f;
				if (cfg.ShowNoise) {
					float val = noise.get_noise_3d(x * cellSize, y * cellSize, z * cellSize);
					noiseSamples[i] = val;
					if (val < minV) {
						minV = val;
					}
					if (val > maxV) {
						maxV = val;
					}
				}
			}
		}
	}
	// second pass - normalize noise values, apply mods
	if (cfg.ShowNoise) {
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					// normalize
					int i = NoiseIndex(ctx, x, y, z);
					float val;
					val = inverse_lerp(minV, maxV, noiseSamples[i]);
					val = clamp(val, 0.0f, 1.0f);
					// apply noise curve
					float valEaseIn = Easing::InCubic(val);
					float valEaseOut = Easing::OutCubic(val);
					val = lerp(valEaseIn, val, clamp(cfg.Curve, 0.0f, 1.0f));
					val = lerp(val, valEaseOut, clamp(cfg.Curve - 1, 0.0f, 1.0f));
					// apply falloff above ceiling
					float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					zeroValue = lerp(0.0f, zeroValue, cfg.FalloffAboveCeiling);
					val = lerp(val, zeroValue, GetAboveCeilAmount2(ctx, y));
					// apply tilt
					float yPct = GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * lerp(0.0f, 1.0f, yPct);
					float valTiltBottom = val * lerp(1.0f, 0.0f, yPct);
					val = lerp(valTiltTop, val, clamp(cfg.Tilt, 0.0f, 1.0f));
					val = lerp(val, valTiltBottom, clamp(cfg.Tilt - 1, 0.0f, 1.0f));
					noiseSamples[i] = val;
				}
			}
		}
	}

	delete[] noiseBuffer;
}

int MeshGen::NoiseIndex(MeshGen::Context ctx, int x, int y, int z) {
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

float MeshGen::GetAboveCeilAmount(MeshGen::Context ctx, int y) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	if (cfg.Ceiling >= 1) {
		return 0.0f;
	}
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	if (Math::is_zero_approx(Math::abs(ceiling - maxY))) {
		return 0.0f;
	}
	maxY = Math::lerp(ceiling, maxY, cfg.FalloffAboveCeiling);
	if (y < ceiling) {
		return 0.0f;
	}
	if (y >= maxY) {
		return 1.0f;
	}
	if (ceiling >= maxY || Math::is_zero_approx(Math::abs(ceiling - maxY))) {
		return 1.0f;
	}
	return Math::clamp(Math::inverse_lerp(ceiling, maxY, y), 0.0f, 1.0f);
}

float MeshGen::GetAboveCeilAmount2(MeshGen::Context ctx, int y) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	// if (cfg.Ceiling >= 1) return 0.0f;
	float high_ceiling_mask = 1.0f - float(cfg.Ceiling >= 1);
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	// if (Math::is_zero_approx(Math::abs(ceiling - maxY))) return 0.0f;
	float zero_approx_mask = 1.0f - float(Math::is_zero_approx(Math::abs(ceiling - maxY)));
	maxY = lerp(ceiling, maxY, cfg.FalloffAboveCeiling);
	// if (y < ceiling) return 0.0f;
	float below_ceiling_mask = 1.0f - float(y < ceiling);
	// if (y >= maxY) return 1.0f;
	float y_above_max_t = float(y >= maxY);
	// if (Math::is_zero_approx(Math::abs(ceiling - maxY))) return 1.0f;
	float ceiling_at_max_y_t = float(Math::is_zero_approx(Math::abs(ceiling - maxY)));
	// if (ceiling >= maxY) return 1.0f;
	float ceiling_above_max_t = float(ceiling >= maxY);
	// clang-format off
	return clamp(inverse_lerp(ceiling, maxY, y), 0.0f, 1.0f)
		* high_ceiling_mask * zero_approx_mask * below_ceiling_mask
		* (1.0f - y_above_max_t) * (1.0f - ceiling_above_max_t) * (1.0f - ceiling_at_max_y_t)
		+ y_above_max_t * ceiling_above_max_t * ceiling_at_max_y_t;
	// clang-format on
}

float MeshGen::GetCeiling(MeshGen::Context ctx) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	return clamp(cfg.Ceiling * (numCells.y - 1), 1.0f, (float)numCells.y - 2);
	// return maxf(1.0f, minf(cfg.Ceiling * (numCells.y - 1), (float)numCells.y - 2));
}

float MeshGen::GetFloorToCeilAmount(MeshGen::Context ctx, int y) {
	float ceiling = GetCeiling(ctx);
	return (float)y / ceiling;
}
