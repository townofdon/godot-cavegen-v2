class_name CaveGenData
extends Resource

@export var cfg:GlobalConfig
@export var arr_room: Array[RoomConfig]
@export var arr_noise: Array[FastNoiseLite]
@export var arr_border_noise: Array[FastNoiseLite]

func validate(current_room_idx: int) -> bool:
	assert(len(arr_room) >= 1)
	assert(len(arr_noise) >= 1)
	assert(len(arr_border_noise) >= 1)
	assert(len(arr_room) == len(arr_noise))
	assert(len(arr_noise) == len(arr_border_noise))
	var room:RoomConfig = arr_room.get(current_room_idx)
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
