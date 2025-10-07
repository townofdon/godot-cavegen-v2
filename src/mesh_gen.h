#pragma once

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/noise.hpp"

#include "gen_util.hpp"
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

	void generate(GlobalConfig *global_cfg, RoomConfig *room_cfg, Noise *noise, Noise *border_noise);

	static float *time_processNoise;

private:
	void process_noise(MG::Context ctx, float noiseSamples[]);
};
