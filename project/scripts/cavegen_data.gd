class_name CaveGenData
extends Resource

@export var cfg:GlobalConfig
@export var arr_room: Array[RoomConfig]
@export var arr_noise: Array[FastNoiseLite]
@export var arr_border_noise: Array[FastNoiseLite]

func validate(current_room_idx: int, room_idx_lookup: Dictionary[String, int]) -> bool:
	assert(len(arr_room) >= 1)
	assert(len(arr_noise) >= 1)
	assert(len(arr_border_noise) >= 1)
	assert(len(arr_room) == len(arr_noise))
	assert(len(arr_noise) == len(arr_border_noise))
	var room:RoomConfig = arr_room.get(current_room_idx)
	var room_idx:int = room_idx_lookup.get(room_idx_key(room.internal__grid_position), -1)
	assert(room.internal__precedence == current_room_idx, "room index is wrong (%s) at position (%s)" % [room.internal__precedence, room.internal__grid_position])
	assert(room_idx == current_room_idx, "lookup is malformed - tried position (%s)" % room.internal__grid_position)
	var noise:FastNoiseLite = arr_noise.get(current_room_idx)
	var border_noise:FastNoiseLite = arr_noise.get(current_room_idx)
	assert(cfg && cfg is GlobalConfig)
	assert(room && room is RoomConfig)
	assert(noise && noise is FastNoiseLite)
	assert(border_noise && border_noise is FastNoiseLite)
	if !cfg || !room || !noise || !border_noise:
		return false
	else:
		return true

static func room_idx_key(coords: Vector2i) -> String:
	return "%s_%s" % [coords.x, coords.y]
