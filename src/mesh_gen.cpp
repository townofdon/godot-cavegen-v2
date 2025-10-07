#include "mesh_gen.h"
#include "easing.hpp"

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
	if (p_global_cfg->RoomWidth <= 0 || p_global_cfg->RoomHeight <= 0 || p_global_cfg->RoomDepth <= 0) {
		auto x = String(std::to_string(p_global_cfg->RoomWidth).c_str());
		auto y = String(std::to_string(p_global_cfg->RoomHeight).c_str());
		auto z = String(std::to_string(p_global_cfg->RoomDepth).c_str());
		UtilityFunctions::printerr("invalid room dimensions: (" + x + "," + y + "," + z + ")");
		return;
	}

	GlobalConfig cfg = *p_global_cfg;
	RoomConfig room = *p_room_cfg;
	struct Vector3i numCells;
	float cellSize = cfg.CellSize - 0.5f;
	do {
		cellSize = cellSize + 0.5f;
		numCells.x = (int)floor(cfg.RoomWidth / cellSize);
		numCells.y = (int)floor(cfg.RoomHeight / cellSize);
		numCells.z = (int)floor(cfg.RoomDepth / cellSize);
	} while (numCells.x * numCells.y * numCells.z > MAX_NOISE_NODES);

	// setup context
	struct GenUtil::Context::Config ctxCfg = {
		// global
		cfg.RoomWidth,
		cfg.RoomHeight,
		cfg.RoomDepth,
		cellSize, // actual cell-size might not reflect value from config
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
	struct GenUtil::Context ctx = {
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
		auto dmillis = String(std::to_string(millis).c_str());
		auto dnodes = String(std::to_string(numCells.x * numCells.y * numCells.z).c_str());
		UtilityFunctions::print("process_noise() took " + dmillis + "ms for " + dnodes + " nodes");
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
void MeshGen::process_noise(GenUtil::Context ctx, float noiseSamples[]) {
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
				int i = GenUtil::NoiseIndex(ctx, x, y, z);
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
					int i = GenUtil::NoiseIndex(ctx, x, y, z);
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
					val = lerp(val, zeroValue, GenUtil::GetAboveCeilAmount2(ctx, y));
					// apply tilt
					float yPct = GenUtil::GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * lerp(0.0f, 1.0f, yPct);
					float valTiltBottom = val * lerp(1.0f, 0.0f, yPct);
					val = lerp(valTiltTop, val, clamp(cfg.Tilt, 0.0f, 1.0f));
					val = lerp(val, valTiltBottom, clamp(cfg.Tilt - 1, 0.0f, 1.0f));
					noiseSamples[i] = val;
				}
			}
		}
	}
	// third pass - apply bounds, borders
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = GenUtil::NoiseIndex(ctx, x, y, z);
				// apply bounds
				if (GenUtil::IsAtBoundaryXZ(ctx, x, z) && cfg.ShowOuterWalls || GenUtil::IsAtBoundaryY(ctx, y)) {
					noiseSamples[i] = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					continue;
				}
				// apply border
				if (GenUtil::IsAtBorder(ctx, x, y, z) && (!cfg.UseBorderNoise || GenUtil::IsAtBorderEdge(ctx, x, y, z))) {
					noiseSamples[i] = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					if (GenUtil::IsBelowCeiling(ctx, y) && cfg.ShowBorder) {
						noiseSamples[i] = maxf(noiseSamples[i], cfg.IsoValue + 0.1f);
					}
					continue;
				}
				// apply falloff to noise above ceil && close to border
				if (!GenUtil::IsBelowCeiling(ctx, y) && cfg.FalloffNearBorder > 0) {
					float dist = GenUtil::DistFromBorder(ctx, x, y, z);
					float t = inverse_lerp(0.0f, (float)cfg.FalloffNearBorder, dist - 1) * (1 - GenUtil::GetAboveCeilAmount2(ctx, y));
					float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					noiseSamples[i] = lerp(zeroValue, noiseSamples[i], clamp(t, 0, 1));
				}
			}
		}
	}

	delete[] noiseBuffer;
}
