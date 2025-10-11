extends TileMapLayer
class_name TileMapEditor

@onready var selectionLayer:TileMapLayer = %SelectionLayer

var cfg:GlobalConfig
var room:RoomConfig

enum NodeTileMapping {
	InheritNoise = 0,
	InnerFilled = 3,
	InnerEmpty = 4,
	WallFilled = 5,
	WallEmpty = 6,
}

const TILEMAP_SOURCE_ID := 2

var prevTileSet := Vector2i(-1,-1)
var prevNumCells := Vector2i(-1,-1)

func _process(_delta: float) -> void:
	selectionLayer.clear()
	var pos := local_to_map(get_local_mouse_position())
	selectionLayer.set_cell(pos, 3, Vector2i(1, 0))
	var atlasCoords = get_cell_atlas_coords(pos)
	if(Input.is_action_pressed("mouse_btn_left")):
		_user_fill_cell_at(pos, atlasCoords)
	elif(Input.is_action_pressed("mouse_btn_right")):
		_user_clear_cell_at(pos, atlasCoords)
	else:
		prevTileSet = Vector2i(-1,-1)

func _user_fill_cell_at(coords: Vector2i, atlasCoords: Vector2i) -> void:
	var tileData := get_cell_tile_data(coords)
	if !tileData:
		return
	if atlasCoords.x == NodeTileMapping.InheritNoise:
		_user_set_cell_at(coords, NodeTileMapping.InnerFilled)
	elif atlasCoords.x == NodeTileMapping.InnerEmpty:
		_user_set_cell_at(coords, NodeTileMapping.InheritNoise)
	elif atlasCoords.x == NodeTileMapping.WallEmpty:
		_user_set_cell_at(coords, NodeTileMapping.WallFilled)

func _user_clear_cell_at(coords: Vector2i, atlasCoords: Vector2i) -> void:
	var tileData := get_cell_tile_data(coords)
	if !tileData:
		return
	if atlasCoords.x == NodeTileMapping.InheritNoise:
		_user_set_cell_at(coords, NodeTileMapping.InnerEmpty)
	elif atlasCoords.x == NodeTileMapping.InnerFilled:
		_user_set_cell_at(coords, NodeTileMapping.InheritNoise)
	elif atlasCoords.x == NodeTileMapping.WallFilled:
		_user_set_cell_at(coords, NodeTileMapping.WallEmpty)

func _user_set_cell_at(coords: Vector2i, tile: NodeTileMapping) -> void:
	if coords == prevTileSet:
		return
	var numCells := _get_num_cells()
	_set_cell_at(coords, tile, numCells)
	prevTileSet = coords

func _set_cell_at(coords: Vector2i, tile: NodeTileMapping, numCells: Vector2i) -> void:
	if numCells == Vector2i.ZERO: return
	var isWall := coords.x==0 || coords.x==numCells.x-1 || coords.y==0 || coords.y==numCells.y-1
	tile = _maybe_convert_to_wall(tile, isWall)
	set_cell(coords, TILEMAP_SOURCE_ID, Vector2i(tile, 0))
	if room: room.set_tile(numCells, coords, _get_tile_state_from_atlas_x(tile))

func _erase_cell_at(coords: Vector2i, numCells: Vector2i) -> void:
	if numCells == Vector2i.ZERO: return
	erase_cell(coords)
	if room: room.set_tile(numCells, coords, RoomConfig.TILE_STATE_UNSET)

func _maybe_convert_to_wall(atlasX:NodeTileMapping, isWall:bool) -> NodeTileMapping:
	if atlasX == NodeTileMapping.InheritNoise && isWall:
		return NodeTileMapping.WallFilled
	if atlasX == NodeTileMapping.InnerFilled && isWall:
		return NodeTileMapping.WallFilled
	if atlasX == NodeTileMapping.InnerEmpty && isWall:
		return NodeTileMapping.WallEmpty
	return atlasX

func _get_tile_state_from_atlas_x(atlasX:int) -> int:
	if atlasX == NodeTileMapping.InheritNoise:
		return RoomConfig.TILE_STATE_UNSET
	if atlasX == NodeTileMapping.InnerFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == NodeTileMapping.InnerEmpty:
		return RoomConfig.TILE_STATE_EMPTY
	if atlasX == NodeTileMapping.WallFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == NodeTileMapping.WallEmpty:
		return RoomConfig.TILE_STATE_EMPTY
	return RoomConfig.TILE_STATE_UNSET

func _get_num_cells() -> Vector2i:
	if !cfg: return Vector2i(0, 0)
	if !room: return Vector2i(0, 0)
	var numCells3dRaw := cfg.get_num_cells()
	# remove bounds (which is always just empty space)
	var numCells3dAdjusted := Vector3i(numCells3dRaw.x - 2, 1, numCells3dRaw.z - 2)
	var numCells := Vector2i(numCells3dAdjusted.x, numCells3dAdjusted.z);
	return numCells

func initialize(p_cfg: GlobalConfig, p_room: RoomConfig) -> void:
	if !p_cfg: return
	if !p_room: return
	cfg = p_cfg
	room = p_room
	var numCells := _get_num_cells()
	clear()
	for y in range(numCells.y):
		for x in range(numCells.x):
			if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
				_set_cell_at(Vector2i(x, y), NodeTileMapping.WallFilled, numCells)
			else:
				_set_cell_at(Vector2i(x, y), NodeTileMapping.InheritNoise, numCells)
	prevNumCells = numCells

func handle_room_size_change() -> void:
	if !cfg: return
	if !room: return
	var numCells := _get_num_cells();
	if numCells == prevNumCells:
		return
	if numCells.x <= 0 || numCells.y <= 0:
		return
	if prevNumCells.x < 0 || prevNumCells.y < 0:
		printerr("invalid prevNumCells (", prevNumCells.x, ",", prevNumCells.y, ") - did you forget to call initialize()?")
		return
	# unset previous border cells, and copy data to new border cells
	if numCells.x != prevNumCells.x:
		for y in range(numCells.y):
			var prevCoords := Vector2i(prevNumCells.x - 1, y)
			var nextCoords := Vector2i(numCells.x - 1, y)
			var currentAtlas := get_cell_atlas_coords(prevCoords)
			var currentTile:NodeTileMapping = currentAtlas.x as NodeTileMapping\
				if currentAtlas.x > -1\
				else NodeTileMapping.WallFilled
			# unset previous border
			if numCells.x > prevNumCells.x:
				_set_cell_at(prevCoords, NodeTileMapping.InheritNoise, numCells)
			else:
				_erase_cell_at(prevCoords, numCells)
			# copy result to new border
			_set_cell_at(nextCoords, currentTile, numCells)
	if prevNumCells.y != numCells.y:
		for x in range(numCells.x):
			var prevCoords := Vector2i(x, prevNumCells.y - 1)
			var nextCoords := Vector2i(x, numCells.y - 1)
			var currentAtlas := get_cell_atlas_coords(prevCoords)
			var currentTile:NodeTileMapping = currentAtlas.x as NodeTileMapping\
				if currentAtlas.x > -1\
				else NodeTileMapping.WallFilled
			# unset previous border
			if numCells.y > prevNumCells.y:
				_set_cell_at(prevCoords, NodeTileMapping.InheritNoise, numCells)
			else:
				_erase_cell_at(prevCoords, numCells)
			# copy result to new border
			_set_cell_at(nextCoords, currentTile, numCells)
	# set new inner cells
	if numCells.x > prevNumCells.x:
		for y in range(numCells.y):
			for x in range(prevNumCells.x, numCells.x-1):
				if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.WallFilled, numCells)
				else:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.InheritNoise, numCells)
	if numCells.y > prevNumCells.y:
		for y in range(prevNumCells.y, numCells.y-1):
			for x in range(numCells.x):
				if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.WallFilled, numCells)
				else:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.InheritNoise,numCells)
	# clear prev outer cells
	if numCells.x < prevNumCells.x:
		for y in range(numCells.y):
			for x in range(numCells.x, prevNumCells.x):
				erase_cell(Vector2i(x, y))
	if numCells.y < prevNumCells.y:
		for y in range(numCells.y, prevNumCells.y):
			for x in range(numCells.x):
				erase_cell(Vector2i(x, y))
	prevNumCells = numCells
