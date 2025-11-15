#include "mesh_gen.h"
#include "easing.hpp"
#include "flood_fill_3d.hpp"

#include "constants.h"
#include "sizing_data.hpp"
#include <cfloat>
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

float *MeshGen::time_marchCubes = new float[100]{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static inline float *noiseSamples = new float[MAX_NOISE_NODES];
static inline float *rawSamples = new float[MAX_NOISE_NODES];
static inline float *noiseBuffer = new float[MAX_NOISE_NODES];
static inline bool *floodFillScreen = new bool[MAX_NOISE_NODES];

void MeshGen::generate(GlobalConfig *p_global_cfg, RoomConfig *p_room, Noise *p_noise, Noise *p_border_noise) {
	ERR_FAIL_NULL_EDMSG(p_global_cfg, "null p_global_cfg");
	ERR_FAIL_NULL_EDMSG(p_room, "null p_room_cfg");
	ERR_FAIL_NULL_EDMSG(p_noise, "null p_noise");
	ERR_FAIL_NULL_EDMSG(p_border_noise, "null p_border_noise");
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

	auto ctx = MG::SetupContext(p_global_cfg, p_room, p_noise, p_border_noise);

	//
	// process noise
	//
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		process_noise(ctx, p_room);
		auto t1 = std::chrono::high_resolution_clock::now();
		// benchmark
		auto numCells = ctx.numCells;
		float millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
		auto dmillis = String(std::to_string(millis).c_str());
		auto dnodes = String(std::to_string(numCells.x * numCells.y * numCells.z).c_str());
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
			UtilityFunctions::print("process_noise() took " + dmillis + "ms for " + dnodes + " nodes, avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}

	//
	// march cubes
	//
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		march_cubes(ctx, p_room, noiseSamples);
		auto t1 = std::chrono::high_resolution_clock::now();
		// benchmark
		float millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
		auto dmillis = String(std::to_string(millis).c_str());
		float minv = INFINITY;
		float maxv = -INFINITY;
		float total = 0.0f;
		int count = 0;
		for (int i = 99; i >= 0; i--) {
			if (i <= 0) {
				time_marchCubes[0] = millis;
			} else {
				time_marchCubes[i] = time_marchCubes[i - 1];
			}
			if (time_marchCubes[i] > 0) {
				total += time_marchCubes[i];
				count++;
				if (time_marchCubes[i] > maxv) {
					maxv = time_marchCubes[i];
				}
				if (time_marchCubes[i] < minv) {
					minv = time_marchCubes[i];
				}
			}
		}
		if (count > 0) {
			float avg = total / (float)count;
			auto dmin = String(std::to_string(minv).c_str());
			auto dmax = String(std::to_string(maxv).c_str());
			auto davg = String(std::to_string(avg).c_str());
			// UtilityFunctions::print("march_cubes() took " + dmillis + "ms, avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}
}

//
// process noise
//
#pragma region process_noise
void MeshGen::process_noise(MG::Context ctx, RoomConfig *room) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	auto noise = ctx.noise;
	float minV = INFINITY;
	float maxV = -INFINITY;
	float cellSize = ctx.cfg.CellSize;

	RoomConfig::RoomNodes nodes = {
		room->nodes.up,
		room->nodes.down,
		room->nodes.left,
		room->nodes.right,
	};
	MG::NeighborPropertyBool hasNeighbor = {
		room->nodes.up.is_valid(),
		room->nodes.down.is_valid(),
		room->nodes.left.is_valid(),
		room->nodes.right.is_valid(),
	};

	//
	// base noise
	//
	// - 1 - initialize && sample all noise values in grid
	{
		bool noiseCached = room->noiseCached;
		int numx = numCells.x;
		int numy = numCells.y;
		int numz = numCells.z;
		for (size_t z = 0; z < numz; z++) {
			for (size_t y = 0; y < numy; y++) {
				for (size_t x = 0; x < numx; x++) {
					int i = MG::NoiseIndex(ctx, x, y, z);
					noiseSamples[i] = 0.0f;
					rawSamples[i] = 0.0f;
					noiseBuffer[i] = 0.0f;
				}
			}
		}
		// this might seem like unnecessary repetition, but this cuts down on branching and allows compiler inlining perf gains
		if (cfg.ShowNoise && noiseCached) {
			for (size_t z = 0; z < numz; z++) {
				for (size_t y = 0; y < numy; y++) {
					for (size_t x = 0; x < numx; x++) {
						int i = MG::NoiseIndex(ctx, x, y, z);
						float val = rawSamples[i];
						// record min/max for normalization
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
		RoomConfig::NoiseNodes noiseNodes = {
			room->noiseNodes.up,
			room->noiseNodes.down,
			room->noiseNodes.left,
			room->noiseNodes.right,
		};
		if (cfg.ShowNoise && !noiseCached) {
			for (size_t z = 0; z < numz; z++) {
				for (size_t y = 0; y < numy; y++) {
					for (size_t x = 0; x < numx; x++) {
						int i = MG::NoiseIndex(ctx, x, y, z);
						noiseBuffer[i] = 0.0f;
						noiseSamples[i] = 0.0f;
						rawSamples[i] = 0.0f;
						// typically, noise output is in range [-1,1]
						float val = noise.get_noise_3d(x * cellSize, y * cellSize, z * cellSize);
						// mix in neighbor noise
						float nUp = noiseNodes.up.is_valid() && nodes.up.is_valid() && nodes.up->roomIdx < room->roomIdx
							? noiseNodes.up->get_noise_3d(x * cellSize, y * cellSize, z * cellSize + (cfg.RoomDepth - cellSize))
							: MAXVAL;
						float nDn = noiseNodes.down.is_valid() && nodes.down.is_valid() && nodes.down->roomIdx < room->roomIdx
							? noiseNodes.down->get_noise_3d(x * cellSize, y * cellSize, z * cellSize - (cfg.RoomDepth - cellSize))
							: MAXVAL;
						float nLf = noiseNodes.left.is_valid() && nodes.left.is_valid() && nodes.left->roomIdx < room->roomIdx
							? noiseNodes.left->get_noise_3d(x * cellSize + (cfg.RoomWidth - cellSize), y * cellSize, z * cellSize)
							: MAXVAL;
						float nRt = noiseNodes.right.is_valid() && nodes.right.is_valid() && nodes.right->roomIdx < room->roomIdx
							? noiseNodes.right->get_noise_3d(x * cellSize - (cfg.RoomWidth - cellSize), y * cellSize, z * cellSize)
							: MAXVAL;
						MG::NeighborPropertyFloat otherNoise = {
							nUp,
							nDn,
							nLf,
							nRt,
						};
						val = MG::GetNeighborWeightedField(ctx, x, z, val, otherNoise);
						rawSamples[i] = val;
						// record min/max for normalization
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
	}
	// - 2 - normalize noise values, apply mods
	if (cfg.ShowNoise) {
		MG::NeighborPropertyFloat neighborNoiseFloor = {
			hasNeighbor.up ? nodes.up->NoiseFloor : MAXVAL,
			hasNeighbor.down ? nodes.down->NoiseFloor : MAXVAL,
			hasNeighbor.left ? nodes.left->NoiseFloor : MAXVAL,
			hasNeighbor.right ? nodes.right->NoiseFloor : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborNoiseCeil = {
			hasNeighbor.up ? nodes.up->NoiseCeil : MAXVAL,
			hasNeighbor.down ? nodes.down->NoiseCeil : MAXVAL,
			hasNeighbor.left ? nodes.left->NoiseCeil : MAXVAL,
			hasNeighbor.right ? nodes.right->NoiseCeil : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborCurve = {
			hasNeighbor.up ? nodes.up->Curve : MAXVAL,
			hasNeighbor.down ? nodes.down->Curve : MAXVAL,
			hasNeighbor.left ? nodes.left->Curve : MAXVAL,
			hasNeighbor.right ? nodes.right->Curve : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborTiltY = {
			hasNeighbor.up ? nodes.up->TiltY : MAXVAL,
			hasNeighbor.down ? nodes.down->TiltY : MAXVAL,
			hasNeighbor.left ? nodes.left->TiltY : MAXVAL,
			hasNeighbor.right ? nodes.right->TiltY : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? nodes.right->IsoValue : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborBassBoost = {
			hasNeighbor.up ? nodes.up->BassBoost : MAXVAL,
			hasNeighbor.down ? nodes.down->BassBoost : MAXVAL,
			hasNeighbor.left ? nodes.left->BassBoost : MAXVAL,
			hasNeighbor.right ? nodes.right->BassBoost : MAXVAL,
		};
		float activeY = (float)MG::GetActivePlaneY(ctx);
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					// normalize
					int i = MG::NoiseIndex(ctx, x, y, z);
					float val;
					if (cfg.Normalize) {
						val = inverse_lerpf(minV, maxV, noiseBuffer[i]);
					} else {
						val = inverse_lerpf(-1, 1, noiseBuffer[i]);
					}
					float noiseFloor = MG::GetNeighborWeightedField(ctx, x, z, cfg.NoiseFloor, neighborNoiseFloor);
					float noiseCeil = MG::GetNeighborWeightedField(ctx, x, z, cfg.NoiseCeil, neighborNoiseCeil);
					float curve = MG::GetNeighborWeightedField(ctx, x, z, cfg.Curve, neighborCurve);
					float tiltY = MG::GetNeighborWeightedField(ctx, x, z, cfg.TiltY, neighborTiltY);
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.TiltY, neighborIsoValue);
					float bassBoost = MG::GetNeighborWeightedField(ctx, x, z, cfg.BassBoost, neighborBassBoost);
					val = clamp01(val);
					val = lerpf(noiseFloor, noiseCeil, val);
					// apply noise curve
					float valEaseIn = Easing::InCubic(val);
					float valEaseOut = Easing::OutCubic(val);
					val = lerpf(valEaseIn, val, clamp01(curve));
					val = lerpf(val, valEaseOut, clamp01(curve - 1));
					// apply falloff above ceiling (gradual, flattening happens below)
					float zeroValue = minf(noiseBuffer[i], isoValue - 0.001f);
					val = lerpf(val, zeroValue, Easing::InQuint(MG::GetAboveCeilAmount(ctx, y, 1)));
					// apply bass boost
					float bassPrg = clamp01(inverse_lerpf(0, activeY, y));
					float bassVal = lerpf(1.0, val, bassPrg);
					val = lerpf(val, bassVal, bassBoost);
					// apply tilt y
					float yPct = MG::GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * lerpf(0.0f, 1.0f, yPct);
					float valTiltBottom = val * lerpf(1.0f, 0.0f, yPct);
					val = lerpf(valTiltTop, val, clamp01(tiltY));
					val = lerpf(val, valTiltBottom, clamp01(tiltY - 1));
					noiseBuffer[i] = val;
				}
			}
		}
	}
	// - 3 - apply bounds, borders, offsets/tilt, smoothing
	{
		MG::NeighborPropertyFloat neighborTiltX = {
			hasNeighbor.up ? nodes.up->TiltX : MAXVAL,
			hasNeighbor.down ? nodes.down->TiltX : MAXVAL,
			MAXVAL,
			MAXVAL,
		};
		MG::NeighborPropertyFloat neighborTiltZ = {
			MAXVAL,
			MAXVAL,
			hasNeighbor.left ? nodes.left->TiltZ : MAXVAL,
			hasNeighbor.right ? nodes.right->TiltZ : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborOffsetY = {
			hasNeighbor.up && nodes.up->TiltZ <= 1 ? nodes.up->OffsetY : MAXVAL,
			hasNeighbor.down && nodes.down->TiltZ >= 1 ? nodes.down->OffsetY : MAXVAL,
			hasNeighbor.left && nodes.left->TiltX <= 1 ? nodes.left->OffsetY : MAXVAL,
			hasNeighbor.right && nodes.right->TiltX >= 1 ? nodes.right->OffsetY : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? nodes.right->IsoValue : MAXVAL,
		};
		float ceiling = MG::GetCeiling(ctx);
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.IsoValue, neighborIsoValue);
					float val = noiseBuffer[MG::NoiseIndex(ctx, x, y, z)];
					if (!MG::IsAtBorderEdge(ctx, x, z) || MG::GetBorderTile(ctx, x, z) == RoomConfig::TILE_STATE_UNSET) {
						// apply tilt x/z - sample different xyz
						float tiltX = MG::GetNeighborWeightedField(ctx, x, z, cfg.TiltX, neighborTiltX) - 1;
						float tiltZ = MG::GetNeighborWeightedField(ctx, x, z, cfg.TiltZ, neighborTiltZ) - 1;
						float px = x / float(numCells.x - 1);
						float pz = z / float(numCells.z - 1);
						float y0 = lerpf(y + numCells.y * clamp01(-tiltX), y + numCells.y * clamp01(tiltX), px);
						float y1 = lerpf(y0 + numCells.y * clamp01(-tiltZ), y0 + numCells.y * clamp01(tiltZ), pz);
						// apply offsetY
						float offsetY = MG::GetNeighborWeightedField(ctx, x, z, cfg.OffsetY, neighborOffsetY);
						float y2 = lerpf(y1 + numCells.y * 0.85f, y1 - numCells.y * 0.85f, offsetY);
						// interpolate
						int i0 = MG::ClampedNoiseIndex(ctx, x, floor(y2), z);
						int i1 = MG::ClampedNoiseIndex(ctx, x, ceil(y2), z);
						float t_offsetInterp = y2 - int(y2);
						val = lerpf(noiseBuffer[i0], noiseBuffer[i1], clamp01(t_offsetInterp));
						// apply smoothing - in a kernel fashion
						float tSmooth = ctx.cfg.Smoothing;
						if (tSmooth != 0) {
							val = 0;
							for (int k = -1; k <= 1; k++) {
								for (int j = -1; j <= 1; j++) {
									int i2 = MG::ClampedNoiseIndex(ctx, x + j, floor(y2), z + k);
									int i3 = MG::ClampedNoiseIndex(ctx, x + j, ceil(y2), z + k);
									float temp = lerpf(noiseBuffer[i2], noiseBuffer[i3], clamp01(t_offsetInterp));
									// 1/9 = 0.1111111111
									val += temp * lerpf(int(j == 0 && k == 0), 0.1111111111f, tSmooth);
								}
							}
						}
						// apply falloff (flattening) above ceiling
						float zeroValue = minf(val, isoValue - 0.1f);
						zeroValue = lerpf(0.0f, zeroValue, 1 - cfg.FalloffAboveCeiling); // flattens noise at ceiling as falloff approaches zero
						val = lerpf(val, zeroValue, Easing::InCubic(MG::GetAboveCeilAmount(ctx, y, 1 - cfg.FalloffAboveCeiling)));
					}
					float zeroValue = minf(val, isoValue - 0.1f);
					if (MG::IsAtBoundaryY(ctx, y)) {
						// apply y bounds
						val = zeroValue;
					} else if (MG::IsAtBoundaryXZ(ctx, x, z) && (cfg.ShowOuterWalls || !MG::IsBelowCeiling(ctx, y))) {
						// apply xz bounds
						val = zeroValue;
					} else if (y <= lerpf(1, ceiling, cfg.FloorLevel) && cfg.ShowFloor) {
						// apply floor
						val = maxf(val, isoValue + 0.1f);
					} else if (
						MG::IsAtBorder(ctx, x, y, z) && (!cfg.UseBorderNoise || MG::IsAtBorderEdge(ctx, x, z)) &&
						MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_UNSET) {
						// apply border
						if (MG::IsBelowCeiling(ctx, y) && cfg.ShowBorder) {
							// un-comment the following to remove noise contribution to border:
							// val = isoValue + 0.1f;
							// apply border tilt
							float tilt = clamp01(ctx.cfg.BorderTilt);
							float py = (1 - clamp01(GetFloorToCeilAmount(ctx, y)));
							float size = lerpf(mini(ctx.cfg.BorderSize, 1), ctx.cfg.BorderSize, clamp01(py));
							size = lerpf(ctx.cfg.BorderSize, size, tilt);
							float pdist = clamp01(MG::SignedDistFromBorder2(ctx, x, z, size)) / maxf(size, 1);
							val = lerpf(maxf(val, isoValue + 0.1f), val, pdist);
							// subtract from border if close to an empty border tile
							// calculate y thresholds
							// t = 0   => y = 0
							// t = 0.5 => y = activeY
							// t = 1   => y = ceiling
							float activeY = (float)MG::GetActivePlaneY(ctx);
							float vfloor = cfg.TileFloor; // user-selected range [0,1]
							float vfloorFalloff = lerpf(vfloor - CMP_EPSILON, 0, cfg.TileFloorFalloff);
							float t0 = clamp01(inverse_lerpf(0, activeY, y)) * 0.5f;
							float t1 = clamp01(inverse_lerpf(activeY, ceiling + CMP_EPSILON, y)) * 0.5f;
							float t = clamp01(t0 + t1);
							float t_floor = clamp01(inverse_lerpf(vfloorFalloff, vfloor, t));
							float borderGapSize = maxf((float)MAX_TILE_ERASE_SIZE * cfg.TileEraseSize, 1) + 1;
							float dist = MG::GetDistanceToEmptyBorderTile(ctx, x, z, borderGapSize);
							float distPct = clamp01(dist / borderGapSize);
							t_floor = lerpf(t_floor, 0, distPct);
							val = lerpf(val, 0, Easing::OutQuad(t_floor));
						} else {
							val = zeroValue;
						}
					} else if (!MG::IsBelowCeiling(ctx, y) && cfg.FalloffNearBorder > 0) {
						// apply falloff to noise above ceil && near border
						int borderSize = cfg.UseBorderNoise ? 1 : cfg.BorderSize;
						float dist = MG::DistFromBorder(ctx, x, z, borderSize);
						float size = minf(ctx.numCells.x, ctx.numCells.z) - cfg.BorderSize * 2;
						float distPct = dist / maxf(1, size * cfg.FalloffNearBorder);
						float t = MG::GetAboveCeilAmount(ctx, y, distPct) * (1 - clamp01(distPct));
						val = lerpf(val, zeroValue, t);
					}
					noiseSamples[MG::NoiseIndex(ctx, x, y, z)] = val;
				}
			}
		}
	}
	// - 4 - adjust areas around action y plane to improve readability
	{
		int activeY = int(MG::GetActivePlaneY(ctx)) + 1;
		float activeYSmoothing = cfg.ActiveYSmoothing;
		if (activeY > 1 && activeY + 1 < numCells.y - 1) {
			for (size_t z = 0; z < numCells.z; z++) {
				for (size_t x = 0; x < numCells.x; x++) {
					int y = activeY - 1;
					// clang-format off
					noiseBuffer[MG::NoiseIndex(ctx, x, y, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(1, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(0, 0.33333333f, activeYSmoothing)
					);
					noiseBuffer[MG::NoiseIndex(ctx, x, y + 1, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(1, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(0, 0.33333333f, activeYSmoothing)
					);
					noiseBuffer[MG::NoiseIndex(ctx, x, y + 2, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(1, 0.33333333f, activeYSmoothing)
					);
					// clang-format on
				}
			}
			for (size_t z = 0; z < numCells.z; z++) {
				for (size_t y = maxi(activeY - 1, 1); y <= activeY + 1 && y < numCells.y - 1; y++) {
					for (size_t x = 0; x < numCells.x; x++) {
						int i = MG::NoiseIndex(ctx, x, y, z);
						noiseSamples[i] = noiseBuffer[i];
					}
				}
			}
		}
	}

	// - 4b - remove overhangs
	{
		int distanceGrid2d[MAX_NOISE_NODES_2D];
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? nodes.right->IsoValue : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborRemoveOverhangsBlend = {
			hasNeighbor.up ? nodes.up->RemoveOverhangsBlend : MAXVAL,
			hasNeighbor.down ? nodes.down->RemoveOverhangsBlend : MAXVAL,
			hasNeighbor.left ? nodes.left->RemoveOverhangsBlend : MAXVAL,
			hasNeighbor.right ? nodes.right->RemoveOverhangsBlend : MAXVAL,
		};
		MG::NeighborPropertyFloat neighborRemoveOverhangsSlope = {
			hasNeighbor.up ? nodes.up->RemoveOverhangsSlope : MAXVAL,
			hasNeighbor.down ? nodes.down->RemoveOverhangsSlope : MAXVAL,
			hasNeighbor.left ? nodes.left->RemoveOverhangsSlope : MAXVAL,
			hasNeighbor.right ? nodes.right->RemoveOverhangsSlope : MAXVAL,
		};
		int activeY = int(MG::GetActivePlaneY(ctx)) + 1;
		// sample noise grid at activeY, where 0=active, 1=empty
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i2d = x + z * ctx.numCells.x;
				bool active = MG::IsPointActive(ctx, neighborIsoValue, noiseSamples, x, activeY, z);
				distanceGrid2d[i2d] = int(active);
			}
		}
		// get manhattan distance of nearest empty cell
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i2d = x + z * ctx.numCells.x;
				distanceGrid2d[i2d] = MG::GetDistanceToEmptyGridCell(ctx, distanceGrid2d, x, z);
			}
		}
		// remove overhangs above activeY
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = maxi(activeY + 2, 1); y < numCells.y - 1; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					int i = MG::NoiseIndex(ctx, x, y, z);
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.IsoValue, neighborIsoValue);
					float removeOverhangsBlend = MG::GetNeighborWeightedField(ctx, x, z, cfg.RemoveOverhangsBlend, neighborRemoveOverhangsBlend);
					float removeOverhangsSlope = MG::GetNeighborWeightedField(ctx, x, z, cfg.RemoveOverhangsSlope, neighborRemoveOverhangsSlope);
					if (removeOverhangsBlend <= 0 || removeOverhangsSlope <= 0) {
						continue;
					}
					int i2d = x + z * ctx.numCells.x;
					int dist = distanceGrid2d[i2d];
					removeOverhangsSlope = clampf(removeOverhangsSlope, 0.001f, 0.999f);
					// convert angle to slope
					float slope = maxf(tanf((1.0 - removeOverhangsSlope) * 0.5 * M_PI), 0.001);
					float y2 = slope * float(dist) + activeY; // y = mx + b
					float zeroVal = minf(noiseSamples[i], isoValue - 0.1f);
					float val = noiseSamples[i];
					float ymod = float(y - y2) / (slope * 2.0);
					val = (y2 < y) ? lerpf(val, zeroVal, ymod) : val;
					noiseSamples[i] = lerpf(noiseSamples[i], val, removeOverhangsBlend);
				}
			}
		}
	}

	//
	// - 5 - apply border noise
	//
	if (cfg.ShowBorder && cfg.UseBorderNoise && cfg.BorderSize > 1) {
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? nodes.right->IsoValue : MAXVAL,
		};
		bool normalize = cfg.NormalizeBorder;
		float ceiling = GetCeiling(ctx);
		float tilt = clamp01(ctx.cfg.BorderTilt);
		// - 5a - sample and normalize border noise on x walls
		//   step 0 => sample noise at x=0
		//   step 1 => normalize at x=0
		//   step 2 => sample noise at x=max
		//   step 3 => normalize at x=max
		for (int step = 0; step < 4; step++) {
			if (step == 0 || step == 2) {
				minV = INFINITY;
				maxV = -INFINITY;
			}
			for (int z = 0; z < numCells.z; z++) {
				for (int y = 1; y < numCells.y && y <= ceiling; y++) {
					int x = 0;
					if (step >= 2) {
						x = numCells.x - 1;
					}
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.IsoValue, neighborIsoValue);
					int i = NoiseIndex(ctx, x, y, z);
					if (step == 0 || step == 2) {
						float val = ctx.borderNoise.get_noise_3d(x, y, z);
						noiseBuffer[i] = val;
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
					} else {
						float val = normalize
							? clamp01(inverse_lerpf(minV, maxV, noiseBuffer[i]))
							: clamp01(inverse_lerpf(-1, 1, noiseBuffer[i]));
						if (MG::GetBorderTile(ctx, x, z) == RoomConfig::TILE_STATE_EMPTY) [[unlikely]] {
							val = minf(noiseSamples[i], isoValue - 0.1f);
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
		// - 5b - sample and normalize border noise on z walls
		//   step 0 => sample noise at z=0
		//   step 1 => normalize at z=0
		//   step 2 => sample noise at z=max
		//   step 3 => normalize at z=max
		for (int step = 0; step < 4; step++) {
			if (step == 0 || step == 2) {
				minV = INFINITY;
				maxV = -INFINITY;
			}
			for (int y = 1; y < numCells.y && y <= ceiling; y++) {
				for (int x = 0; x < numCells.x; x++) {
					int z = 0;
					if (step >= 2) {
						z = numCells.z - 1;
					}
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.IsoValue, neighborIsoValue);
					int i = NoiseIndex(ctx, x, y, z);
					if (step == 0 || step == 2) {
						float val = ctx.borderNoise.get_noise_3d(x, y, z);
						noiseBuffer[i] = val;
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
					} else {
						float val = normalize
							? clamp01(inverse_lerpf(minV, maxV, noiseBuffer[i]))
							: clamp01(inverse_lerpf(-1, 1, noiseBuffer[i]));
						if (MG::GetBorderTile(ctx, x, z) == RoomConfig::TILE_STATE_EMPTY) [[unlikely]] {
							val = minf(noiseSamples[i], isoValue - 0.1f);
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
		// - 5c: apply noise to border points
		//   side: x=0
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 2; y < numCells.y && y <= ceiling; y++) {
				float py = (1 - clamp01(GetFloorToCeilAmount(ctx, y)));
				float size = lerpf(mini(ctx.cfg.BorderSize, 1), ctx.cfg.BorderSize * 2, clamp01(py));
				size = lerpf(ctx.cfg.BorderSize, size, tilt);
				for (int x = 2; x < size + 1; x++) {
					if (MG::GetBorderTile(ctx, 1, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, 0, y, z)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, 0, y + j, z + k)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (x - 1) / (float)size;
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		//   side: z=max
		for (int y = 2; y < numCells.y && y <= ceiling; y++) {
			float py = (1 - clamp01(GetFloorToCeilAmount(ctx, y)));
			float size = lerpf(mini(ctx.cfg.BorderSize, 1), ctx.cfg.BorderSize * 2, clamp01(py));
			size = lerpf(ctx.cfg.BorderSize, size, tilt);
			for (int z = numCells.z - 3; z > numCells.z - size - 2; z--) {
				for (int x = 1; x < numCells.x - 1; x++) {
					if (MG::GetBorderTile(ctx, x, numCells.z - 2) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, x, y, numCells.z - 1)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, x + j, y + k, numCells.z - 1)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (numCells.z - 2 - z) / (float)(size - 1);
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		//   side: x=max
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 2; y < numCells.y && y <= ceiling; y++) {
				float py = (1 - clamp01(GetFloorToCeilAmount(ctx, y)));
				float size = lerpf(mini(ctx.cfg.BorderSize, 1), ctx.cfg.BorderSize * 2, clamp01(py));
				size = lerpf(ctx.cfg.BorderSize, size, tilt);
				for (int x = numCells.x - 3; x > numCells.x - size - 2; x--) {
					if (MG::GetBorderTile(ctx, numCells.x - 2, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, numCells.x - 1, y, z)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, numCells.x - 1, y + j, z + k)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (numCells.x - 2 - x) / (float)(size - 1);
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		//   side: z=0
		for (int y = 2; y < numCells.y && y <= ceiling; y++) {
			float py = (1 - clamp01(GetFloorToCeilAmount(ctx, y)));
			float size = lerpf(mini(ctx.cfg.BorderSize, 1), ctx.cfg.BorderSize * 2, clamp01(py));
			size = lerpf(ctx.cfg.BorderSize, size, tilt);
			for (int z = 2; z < size + 1; z++) {
				for (int x = 1; x < numCells.x - 1; x++) {
					if (MG::GetBorderTile(ctx, x, 1) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, x, y, 0)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					for (int j = -1; j <= 1; j++) {
						for (int k = -1; k <= 1; k++) {
							if (j == 0 && k == 0)
								continue;
							kernel += noiseBuffer[NoiseIndex(ctx, x + j, y + k, 0)] * 0.125f;
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (z - 1) / (float)size;
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
	}

	//
	// apply tile data
	//
	{
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? nodes.right->IsoValue : MAXVAL,
		};
		int activeY = int(MG::GetActivePlaneY(ctx));
		int ceiling = round(MG::GetCeiling(ctx));
		float tSmooth = cfg.TileSmoothing; // range [0,1]
		float vfloor = cfg.TileFloor; // range [0,1]
		float vfloorFalloff = lerpf(vfloor - CMP_EPSILON, 0, cfg.TileFloorFalloff);
		float vceil = cfg.TileCeiling; // range [0,1]
		float vceilFalloff = lerpf(vceil - CMP_EPSILON, 0, cfg.TileCeilingFalloff);
		for (int z = 2; z < numCells.z - 2; z++) {
			for (int y = 2; y < numCells.y - 2; y++) {
				for (int x = 2; x < numCells.x - 2; x++) {
					float isoValue = MG::GetNeighborWeightedField(ctx, x, z, cfg.IsoValue, neighborIsoValue);
					int i = MG::NoiseIndex(ctx, x, y, z);
					auto currentTile = MG::GetTile(ctx, x, z);
					float unsetNoiseVal = 1;
					// subtract noise if close to adjacent empty tile
					if (cfg.TileEraseSize > 0) {
						float dist = MG::GetDistanceToEmptyTile(ctx, x, z, MAX_TILE_ERASE_SIZE * cfg.TileEraseSize) - 1;
						float tDist = clamp01(dist / ((float)MAX_TILE_ERASE_SIZE * cfg.TileEraseSize));
						tDist = lerpf(1, tDist, clamp01(cfg.TileStrength));
						tDist = tDist >= 1 ? 1 : lerpf(tDist, 0, clamp01(cfg.TileStrength - 1));
						tDist = Easing::OutQuad(tDist);
						unsetNoiseVal = tDist;
					}
					// calculate tile kernel, where 0 => empty, 1 => unset, 2 => filled
					float kernel = 0.0f;
					for (int j = -1; j <= 1; j++) {
						for (int k = -1; k <= 1; k++) {
							bool isBorderTile = MG::IsBorderTile(ctx, x + j, z + k);
							int tile = isBorderTile ? currentTile : MG::GetTile(ctx, x + j, z + k, currentTile);
							float val = 0;
							if (tile == RoomConfig::TILE_STATE_UNSET) {
								val = unsetNoiseVal;
							} else if (tile == RoomConfig::TILE_STATE_FILLED) {
								val = 2;
							}
							// 1/9 = 0.1111111111
							kernel += val * lerpf(int(j == 0 && k == 0), 0.1111111111f, tSmooth);
						}
					}
					// calculate y thresholds
					// 0   => 0
					// 0.5 => activeY
					// 1   => ceiling
					float t0 = clamp01(inverse_lerpf(0, activeY, y)) * 0.5f;
					float t1 = clamp01(inverse_lerpf(activeY, ceiling + CMP_EPSILON, y)) * 0.5f;
					float t = clamp01(t0 + t1);
					float t_floor = clamp01(inverse_lerpf(vfloorFalloff, vfloor, t));
					float t_ceil = clamp01(inverse_lerpf(vceil, vceilFalloff, t));
					// calc empty noise target
					float targ_empty = 0.0f;
					targ_empty = minf(noiseSamples[i], isoValue - 0.1f);
					targ_empty = lerpf(noiseSamples[i], targ_empty, clamp01(cfg.TileStrength));
					targ_empty = lerpf(targ_empty, 0.0f, clamp01(cfg.TileStrength - 1));
					targ_empty = lerpf(noiseSamples[i], targ_empty, t_floor);
					// calc filled noise target
					float targ_filled = 0.0f;
					float zero = minf(noiseSamples[i], isoValue - 0.1f);
					// as tile strength approaches 2, reduce noise above ceil
					float fallback = lerpf(noiseSamples[i], zero, clamp01(cfg.TileStrength - 1));
					targ_filled = maxf(noiseSamples[i], isoValue + 0.1f);
					targ_filled = lerpf(noiseSamples[i], targ_filled, clamp01(cfg.TileStrength));
					targ_filled = lerpf(targ_filled, 1.0f, clamp01(cfg.TileStrength - 1));
					targ_filled = lerpf(fallback, targ_filled, t_ceil);
					// aggregate targets based on kernel results
					float targ_unset = noiseSamples[i];
					float targ;
					targ = lerpf(targ_empty, targ_unset, clamp01(kernel));
					targ = lerpf(targ, targ_filled, clamp01(kernel - 1));
					noiseSamples[i] = targ;
				}
			}
		}
	}

	//
	// remove orphans / islands - flood fill
	//
	if (cfg.RemoveOrphans) {
		MG::NeighborPropertyFloat neighborIsoValue = {
			hasNeighbor.up ? room->nodes.up->IsoValue : MAXVAL,
			hasNeighbor.down ? room->nodes.down->IsoValue : MAXVAL,
			hasNeighbor.left ? room->nodes.left->IsoValue : MAXVAL,
			hasNeighbor.right ? room->nodes.right->IsoValue : MAXVAL,
		};
		float orphanY = MG::GetActivePlaneY(ctx) * ctx.cfg.OrphanThreshold;
		// pick likely start point candidates
		auto candidates = PackedVector3Array();
		{
			int xstart = 1;
			int xmid = (numCells.x - 1) / 2;
			int xend = (numCells.x - 2);
			int ystart = 1;
			int ymid = MG::GetActivePlaneY(ctx) / 2;
			int yend = (numCells.y - 2);
			int zstart = 1;
			int zmid = (numCells.z - 1) / 2;
			int zend = (numCells.z - 2);
			candidates.append(Vector3i(xmid, ystart, zmid));
			candidates.append(Vector3i(xstart, ymid, zmid));
			candidates.append(Vector3i(xend, ymid, zmid));
			candidates.append(Vector3i(xmid, ymid, zstart));
			candidates.append(Vector3i(xstart, ymid, zend));
			candidates.append(Vector3i(xend, ymid, zstart));
			candidates.append(Vector3i(xend, ymid, zend));
			candidates.append(Vector3i(xstart, ymid, zstart));
			candidates.append(Vector3i(xmid, ymid, zend));
			candidates.append(Vector3i(xmid, ymid, zmid));
			candidates.append(Vector3i(xstart, ystart, zmid));
			candidates.append(Vector3i(xend, ystart, zmid));
			candidates.append(Vector3i(xmid, ystart, zstart));
			candidates.append(Vector3i(xmid, ystart, zend));
		}
		auto floodStarts = PackedVector3Array();
		int numCandidatesFound = 0;
		for (size_t i = 0; i < candidates.size() && numCandidatesFound < 5; i++) {
			Vector3 candidate = candidates.get(i);
			bool found = MG::IsPointActive(ctx, neighborIsoValue, noiseSamples, candidate.x, candidate.y, candidate.z);
			if (found) {
				floodStarts.append(candidate);
				numCandidatesFound++;
			}
		}
		bool foundCandidate = numCandidatesFound > 0;
		// initialize flood screen and set floodStart if not yet found
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 1; y < numCells.y - 1; y++) {
				for (int x = 1; x < numCells.x - 1; x++) {
					if (!foundCandidate && MG::IsPointActive(ctx, neighborIsoValue, noiseSamples, x, y, z)) {
						floodStarts.append(Vector3i(x, y, z));
						foundCandidate = true;
					}
					floodFillScreen[MG::NoiseIndex(ctx, x, y, z)] = y <= orphanY || MG::IsPointActive(ctx, neighborIsoValue, noiseSamples, x, y, z);
				}
			}
		}
		if (foundCandidate) {
			// flood fill from start, setting true nodes to false - remaining true nodes will be zeroed.
			for (size_t i = 0; i < floodStarts.size(); i++) {
				Vector3 floodStart = floodStarts.get(i);
				flood_fill_3d(ctx, floodFillScreen, floodStart, true, false);
			}
			for (int z = 1; z < numCells.z - 1; z++) {
				for (int y = 1; y < numCells.y - 1; y++) {
					for (int x = 1; x < numCells.x - 1; x++) {
						float isoValue = MG::GetNeighborWeightedField(ctx, x, z, ctx.cfg.IsoValue, neighborIsoValue);
						int i = MG::NoiseIndex(ctx, x, y, z);
						if (floodFillScreen[i]) {
							float zeroValue = minf(noiseSamples[i], isoValue - 0.1f);
							noiseSamples[i] = zeroValue;
						}
					}
				}
			}
		}
	}

	// copy noise back to room noise fields
	for (size_t i = 0; i < MAX_NOISE_NODES; i++) {
		room->noiseSamples[i] = noiseSamples[i];
		room->rawSamples[i] = rawSamples[i];
	}
	room->noiseCached = true;
}
#pragma endregion // process_noise

//
// march cubes
//
#pragma region march_cubes
void MeshGen::march_cubes(MG::Context ctx, RoomConfig *room, float noiseSamples[]) {
	auto ref = get_mesh();
	auto ptr = *(ref);
	ArrayMesh *mesh = Object::cast_to<ArrayMesh>(ptr);
	ERR_FAIL_NULL_EDMSG(mesh, "mesh cannot be null");

	mesh->clear_surfaces();
	auto surface_array = godot::Array();
	surface_array.resize(Mesh::ARRAY_MAX);

	MG::NeighborPropertyFloat neighborIsoValue = {
		room->nodes.up.is_valid() ? room->nodes.up->IsoValue : MAXVAL,
		room->nodes.down.is_valid() ? room->nodes.down->IsoValue : MAXVAL,
		room->nodes.left.is_valid() ? room->nodes.left->IsoValue : MAXVAL,
		room->nodes.right.is_valid() ? room->nodes.right->IsoValue : MAXVAL,
	};
	Vector3 roomPos = Vector3(
		(ctx.cfg.RoomWidth - ctx.cfg.CellSize) * room->GridPosition.x,
		0,
		(ctx.cfg.RoomDepth - ctx.cfg.CellSize) * room->GridPosition.y);

	auto verts = PackedVector3Array();
	auto uvs = PackedVector2Array();
	auto normals = PackedVector3Array();
	auto indices = PackedInt32Array(); // vertex indices, in groups of 3 to form triangles

	Vector3 points[3];
	auto numCells = ctx.numCells;
	int vertexIndex = 0;

	for (int z = 0; z < numCells.z - 1; z++) {
		for (int y = 0; y < numCells.y - 1; y++) {
			for (int x = 0; x < numCells.x - 1; x++) {
				int pointIndex = 0;
				int triIdx = MG::GetTriangulation(ctx, neighborIsoValue, noiseSamples, x, y, z);
				auto uv = Vector2(
					(x + 1) / (float)numCells.x,
					maxf(
						(y + 1) / (float)numCells.y,
						(z + 1) / (float)numCells.z));
				// iterate over triangulations
				for (size_t i = 0; i < 15; i++) {
					// get triangulation by row_index + col_index
					auto edgeIdx = TRIANGULATIONS[triIdx * 15 + i];
					if (edgeIdx < 0) {
						break;
					}
					// var (p0, p1) = Constants.EDGES[edgeIdx];
					int p0 = EDGES[edgeIdx * 2 + 0];
					int p1 = EDGES[edgeIdx * 2 + 1];
					// var (x0, y0, z0) = Constants.POINTS[p0];
					int x0 = POINTS[p0 * 3 + 0];
					int y0 = POINTS[p0 * 3 + 1];
					int z0 = POINTS[p0 * 3 + 2];
					// var (x1, y1, z1) = Constants.POINTS[p1];
					int x1 = POINTS[p1 * 3 + 0];
					int y1 = POINTS[p1 * 3 + 1];
					int z1 = POINTS[p1 * 3 + 2];
					Vector3 a = Vector3i(x + x0, y + y0, z + z0);
					Vector3 b = Vector3i(x + x1, y + y1, z + z1);
					Vector3 p = MG::InterpolateMeshPoints(ctx, neighborIsoValue, noiseSamples, a, b);
					p = MG::ClampMeshBorderPoint(ctx, p);
					points[pointIndex] = Vector3(p.x, p.y, p.z);
					pointIndex++;
					if (pointIndex == 3) {
						pointIndex = 0;
						Vector3 p1 = points[0];
						Vector3 p2 = points[1];
						Vector3 p3 = points[2];
						Vector3 normal = -(p2 - p1).cross(p3 - p1).normalized();
						for (size_t i = 0; i < 3; i++) {
							Vector3 point = points[i];
							float x = point.x * ctx.cfg.CellSize;
							float y = point.y * ctx.cfg.CellSize;
							float z = point.z * ctx.cfg.CellSize;
							Vector3 offset = Vector3(0, -(MG::GetActivePlaneYF(ctx) + 1) * int(ctx.cfg.MoveActivePlaneToOrigin), 0);
							Vector3 vert = roomPos + Vector3(x, y, z) + offset;
							verts.append(vert);
							uvs.append(uv);
							normals.append(normal);
							indices.append(vertexIndex);
							vertexIndex++;
						}
					}
				}
			}
		}
	}

	surface_array[Mesh::ARRAY_VERTEX] = verts;
	surface_array[Mesh::ARRAY_TEX_UV] = uvs;
	surface_array[Mesh::ARRAY_NORMAL] = normals;
	surface_array[Mesh::ARRAY_INDEX] = indices;
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
}
#pragma endregion // march_cubes
