class_name CaveGenData
extends Resource

@export var cfg:GlobalConfig
@export var arr_room: Array[RoomConfig]
@export var arr_noise: Array[FastNoiseLite]
@export var arr_border_noise: Array[FastNoiseLite]

func validate(current_room_idx: int, room_idx_lookup: Dictionary[String, int]) -> bool:
	assert(cfg && cfg is GlobalConfig)
	assert(len(arr_room) >= 1)
	assert(len(arr_noise) >= 1)
	assert(len(arr_border_noise) >= 1)
	assert(len(arr_room) == len(arr_noise))
	assert(len(arr_noise) == len(arr_border_noise))
	var room:RoomConfig = arr_room.get(current_room_idx)
	assert(room && room is RoomConfig)
	var room_idx:int = room_idx_lookup.get(room_idx_key(room.internal__grid_position), -1)
	assert(room.get_room_idx() == current_room_idx, "room index is wrong (%s) at position (%s)" % [room.get_room_idx(), room.internal__grid_position])
	assert(room_idx == current_room_idx, "lookup is malformed - tried position (%s)" % room.internal__grid_position)
	var noise:FastNoiseLite = arr_noise.get(current_room_idx)
	var border_noise:FastNoiseLite = arr_noise.get(current_room_idx)
	assert(noise && noise is FastNoiseLite)
	assert(border_noise && border_noise is FastNoiseLite)
	# assert neighbor relationships
	if room.get_node_up():
		assert(room.get_node_up().get_node_down() == room)
		assert(room.get_node_up().internal__grid_position == room.internal__grid_position + Vector2i.UP)
	if room.get_node_down():
		assert(room.get_node_down().get_node_up() == room)
		assert(room.get_node_down().internal__grid_position == room.internal__grid_position + Vector2i.DOWN)
	if room.get_node_left():
		assert(room.get_node_left().get_node_right() == room)
		assert(room.get_node_left().internal__grid_position == room.internal__grid_position + Vector2i.LEFT)
	if room.get_node_right():
		assert(room.get_node_right().get_node_left() == room)
		assert(room.get_node_right().internal__grid_position == room.internal__grid_position + Vector2i.RIGHT)

	if !cfg || !room || !noise || !border_noise:
		return false
	else:
		return true

static func room_idx_key(coords: Vector2i) -> String:
	return "%s_%s" % [coords.x, coords.y]
