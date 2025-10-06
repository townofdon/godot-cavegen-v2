#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class GlobalConfig : public Resource {
	GDCLASS(GlobalConfig, Resource)

protected:
	static void _bind_methods();

public:
	GlobalConfig() = default;
	~GlobalConfig() override = default;
};
