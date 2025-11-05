#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "sizing_data.hpp"

using namespace godot;

class GlobalConfig : public Resource {
	GDCLASS(GlobalConfig, Resource)

protected:
	static void _bind_methods();

public:
	GlobalConfig();
	~GlobalConfig();

	// delete the copy constructor to avoid accidental assignment by value
	GlobalConfig(const GlobalConfig &) = delete;

	bool IsNewLevel;
	godot::String LevelName;

	float RoomWidth;
	float RoomHeight;
	float RoomDepth;
	float CellSize;
	float Ceiling;
	float ActivePlaneOffset;
	bool MoveActivePlaneToOrigin;

	bool GetIsNewLevel();
	void SetIsNewLevel(bool p_IsNewLevel);
	String GetLevelName();
	void SetLevelName(String p_LevelName);
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
	bool GetMoveActivePlaneToOrigin();
	void SetMoveActivePlaneToOrigin(float p_MoveActivePlaneToOrigin);

	SizingData GetSizingData();
	Vector3i GetNumCells();
	Vector2i GetNumCells2d();
};
