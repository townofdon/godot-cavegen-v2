#pragma once

#include "godot_cpp/classes/resource.hpp"

using namespace godot;

class GlobalConfig : public Resource {
	GDCLASS(GlobalConfig, Resource)

protected:
	static void _bind_methods();

public:
	GlobalConfig();
	~GlobalConfig();

	float RoomWidth;
	float RoomHeight;
	float RoomDepth;
	float CellSize;
	float Ceiling;
	float ActivePlaneOffset;

	float GetRoomWidth();
	void SetRoomWidth(float p_RoomWidth);
	float GetRoomHeight();
	void SetRoomHeight(float p_RoomHeight);
	float GetRoomDepth();
	void SetRoomDepth(float p_RoomDepth);
	float GetCellSize();
	void SetCellSize(float p_CellSize);
	float GetCeiling();
	void SetCeiling(float p_Ceiling);
	float GetActivePlaneOffset();
	void SetActivePlaneOffset(float p_ActivePlaneOffset);
};
