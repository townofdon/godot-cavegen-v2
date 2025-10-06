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

	double RoomWidth;
	double RoomHeight;
	double RoomDepth;
	double CellSize;
	double Ceiling;
	double ActivePlaneOffset;

	double GetRoomWidth();
	void SetRoomWidth(double p_RoomWidth);
	double GetRoomHeight();
	void SetRoomHeight(double p_RoomHeight);
	double GetRoomDepth();
	void SetRoomDepth(double p_RoomDepth);
	double GetCellSize();
	void SetCellSize(double p_CellSize);
	double GetCeiling();
	void SetCeiling(double p_Ceiling);
	double GetActivePlaneOffset();
	void SetActivePlaneOffset(double p_ActivePlaneOffset);
};
