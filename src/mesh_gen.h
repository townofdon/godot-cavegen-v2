#pragma once

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class MeshGen : public MeshInstance3D {
	GDCLASS(MeshGen, MeshInstance3D)

protected:
	static void _bind_methods();

public:
	MeshGen() = default;
	~MeshGen() override = default;
};
