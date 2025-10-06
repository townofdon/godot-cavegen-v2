#pragma once

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/noise.hpp"

#include "global_config.h"
#include "room_config.h"

using namespace godot;

class MeshGen : public MeshInstance3D {
	GDCLASS(MeshGen, MeshInstance3D)

protected:
	static void _bind_methods();

public:
	MeshGen() = default;
	~MeshGen() override = default;

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

	void generate(GlobalConfig *global_cfg, RoomConfig *room_cfg, Noise *noise, Noise *border_noise);

private:
	Context build_context(GlobalConfig *global_cfg, RoomConfig *room_cfg, Noise *noise, Noise *border_noise);
	void process_noise(MeshGen::Context ctx, float noiseSamples[]);
	int NoiseIndex(MeshGen::Context ctx, int x, int y, int z);
	float GetAboveCeilAmount(MeshGen::Context ctx, int y);
	float GetCeiling(MeshGen::Context ctx);
	float GetFloorToCeilAmount(MeshGen::Context ctx, int y);
};
