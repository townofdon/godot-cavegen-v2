#include "global_config.h"
#include "constants.h"

void GlobalConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_RoomWidth"), &GlobalConfig::GetRoomWidth);
	ClassDB::bind_method(D_METHOD("set_RoomWidth", "p_RoomWidth"), &GlobalConfig::SetRoomWidth);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_width", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomWidth", "get_RoomWidth");

	ClassDB::bind_method(D_METHOD("get_RoomHeight"), &GlobalConfig::GetRoomHeight);
	ClassDB::bind_method(D_METHOD("set_RoomHeight", "p_RoomHeight"), &GlobalConfig::SetRoomHeight);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_height", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomHeight", "get_RoomHeight");

	ClassDB::bind_method(D_METHOD("get_RoomDepth"), &GlobalConfig::GetRoomDepth);
	ClassDB::bind_method(D_METHOD("set_RoomDepth", "p_RoomDepth"), &GlobalConfig::SetRoomDepth);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_depth", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomDepth", "get_RoomDepth");

	ClassDB::bind_method(D_METHOD("get_CellSize"), &GlobalConfig::GetCellSize);
	ClassDB::bind_method(D_METHOD("set_CellSize", "p_CellSize"), &GlobalConfig::SetCellSize);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_size", PROPERTY_HINT_RANGE, "0.5,5.0,0.5"), "set_CellSize", "get_CellSize");

	ClassDB::bind_method(D_METHOD("get_Ceiling"), &GlobalConfig::GetCeiling);
	ClassDB::bind_method(D_METHOD("set_Ceiling", "p_Ceiling"), &GlobalConfig::SetCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_Ceiling", "get_Ceiling");

	ClassDB::bind_method(D_METHOD("get_ActivePlaneOffset"), &GlobalConfig::GetActivePlaneOffset);
	ClassDB::bind_method(D_METHOD("set_ActivePlaneOffset", "p_ActivePlaneOffset"), &GlobalConfig::SetActivePlaneOffset);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "active_plane_offset", PROPERTY_HINT_RANGE, "0,50,0.01"), "set_ActivePlaneOffset", "get_ActivePlaneOffset");

	ADD_SIGNAL(MethodInfo("on_changed"));
	// ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");

	ClassDB::bind_method(D_METHOD("get_num_cells"), &GlobalConfig::GetNumCells);
}

GlobalConfig::GlobalConfig() {
	// Initialize any variables here.
	RoomWidth = 30.0f;
	RoomHeight = 30.0f;
	RoomDepth = 30.0f;
	CellSize = 1.0f;
	Ceiling = 0.75f;
	ActivePlaneOffset = 10;
}

GlobalConfig::~GlobalConfig() {
	// Add your cleanup here.
}

float GlobalConfig::GetRoomWidth() {
	return RoomWidth;
}

void GlobalConfig::SetRoomWidth(float p_RoomWidth) {
	RoomWidth = p_RoomWidth;
	emit_signal("on_changed");
}

float GlobalConfig::GetRoomHeight() {
	return RoomHeight;
}

void GlobalConfig::SetRoomHeight(float p_RoomHeight) {
	RoomHeight = p_RoomHeight;
	emit_signal("on_changed");
}

float GlobalConfig::GetRoomDepth() {
	return RoomDepth;
}

void GlobalConfig::SetRoomDepth(float p_RoomDepth) {
	RoomDepth = p_RoomDepth;
	emit_signal("on_changed");
}

float GlobalConfig::GetCellSize() {
	return CellSize;
}

void GlobalConfig::SetCellSize(float p_CellSize) {
	CellSize = p_CellSize;
	emit_signal("on_changed");
}

float GlobalConfig::GetCeiling() {
	return Ceiling;
}

void GlobalConfig::SetCeiling(float p_Ceiling) {
	Ceiling = p_Ceiling;
	emit_signal("on_changed");
}

float GlobalConfig::GetActivePlaneOffset() {
	return ActivePlaneOffset;
}

void GlobalConfig::SetActivePlaneOffset(float p_ActivePlaneOffset) {
	ActivePlaneOffset = p_ActivePlaneOffset;
	emit_signal("on_changed");
}

SizingData GlobalConfig::GetSizingData() {
	struct Vector3i numCells;
	float cellSize = CellSize - 0.5f;
	do {
		cellSize = cellSize + 0.5f;
		numCells.x = (int)floor(RoomWidth / cellSize) + 2;
		numCells.y = (int)floor(RoomHeight / cellSize) + 2;
		numCells.z = (int)floor(RoomDepth / cellSize) + 2;
	} while (numCells.x * numCells.y * numCells.z > MAX_NOISE_NODES);
	struct SizingData sizing = {
		numCells,
		cellSize,
	};
	return sizing;
}

Vector3i GlobalConfig::GetNumCells() {
	return GlobalConfig::GetSizingData().numCells;
}
