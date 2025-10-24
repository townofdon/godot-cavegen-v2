extends TileMapLayer
class_name TileMapEditor

@onready var selectionLayer:TileMapLayer = %SelectionLayer

signal on_mode_changed(mode: EditorMode)

var cfg:GlobalConfig
var room:RoomConfig

enum NodeTileMapping {
	Null = -1,
	InnerEmpty = 0,
	InnerNoise = 1,
	InnerFilled = 2,
	WallEmpty = 3,
	WallNoise = 4,
	WallFilled = 5,
}

enum EditorMode {
	Draw,
	Line,
	Rect,
	Fill,
}

const TILEMAP_SOURCE_ID := 2

var prevNumCells := Vector2i(-1,-1)
var lastTileDrawnCoords := Vector2i(-1,-1)
var drawTile := NodeTileMapping.Null

var mode := EditorMode.Draw

func set_editor_mode(p_mode: EditorMode) -> void:
	mode = p_mode
	on_mode_changed.emit(mode)

func _process(_delta: float) -> void:
	var coords := local_to_map(get_local_mouse_position())
	# draw mode
	if mode == EditorMode.Draw:
		selectionLayer.clear()
		selectionLayer.set_cell(coords, 3, Vector2i(1, 0))
		var atlasCoords := get_cell_atlas_coords(coords)
		if(Input.is_action_pressed("mouse_btn_right")):
			drawTile = _get_next_tile_to_lay(atlasCoords, false);
			_user_set_cell_at(coords, drawTile)
		elif(Input.is_action_pressed("mouse_btn_left")):
			drawTile = _get_next_tile_to_lay(atlasCoords, true);
			_user_set_cell_at(coords, drawTile)
		else:
			lastTileDrawnCoords = Vector2i(-1,-1)
			drawTile = NodeTileMapping.Null
	elif mode == EditorMode.Line:
		# TODO: ADD LINE MODE
		pass
	elif mode == EditorMode.Fill:
		# TODO: ADD FILL MODE
		pass

func _get_next_tile_to_lay(atlasCoords: Vector2i, fill: bool) -> NodeTileMapping:
	if drawTile != NodeTileMapping.Null:
		return drawTile
	if fill:
		if atlasCoords.x == NodeTileMapping.InnerFilled:
			return NodeTileMapping.InnerFilled
		if atlasCoords.x == NodeTileMapping.InnerNoise:
			return NodeTileMapping.InnerFilled
		elif atlasCoords.x == NodeTileMapping.InnerEmpty:
			return NodeTileMapping.InnerNoise
		elif atlasCoords.x == NodeTileMapping.WallEmpty:
			return NodeTileMapping.WallNoise
		elif atlasCoords.x == NodeTileMapping.WallNoise:
			return NodeTileMapping.WallFilled
		elif atlasCoords.x == NodeTileMapping.WallFilled:
			return NodeTileMapping.WallFilled
	else:
		if atlasCoords.x == NodeTileMapping.InnerEmpty:
			return NodeTileMapping.InnerEmpty
		if atlasCoords.x == NodeTileMapping.InnerNoise:
			return NodeTileMapping.InnerEmpty
		elif atlasCoords.x == NodeTileMapping.InnerFilled:
			return NodeTileMapping.InnerNoise
		elif atlasCoords.x == NodeTileMapping.WallFilled:
			return NodeTileMapping.WallNoise
		elif atlasCoords.x == NodeTileMapping.WallNoise:
			return NodeTileMapping.WallEmpty
		elif atlasCoords.x == NodeTileMapping.WallEmpty:
			return NodeTileMapping.WallEmpty
	return NodeTileMapping.Null

func _user_set_cell_at(coords: Vector2i, tile: NodeTileMapping) -> void:
	if coords == lastTileDrawnCoords: return
	if tile == NodeTileMapping.Null: return
	var numCells := _get_num_cells()
	_set_cell_at(coords, tile, numCells)
	lastTileDrawnCoords = coords
	if room: room.notify_changed()

func _set_cell_at(coords: Vector2i, tile: NodeTileMapping, numCells: Vector2i) -> void:
	if numCells == Vector2i.ZERO: return
	if coords.x < 0 || coords.x >= numCells.x || coords.y < 0 || coords.y >= numCells.y:
		return
	var isWall := coords.x==0 || coords.x==numCells.x-1 || coords.y==0 || coords.y==numCells.y-1
	tile = _maybe_convert_tile(tile, isWall)
	set_cell(coords, TILEMAP_SOURCE_ID, Vector2i(tile, 0))
	if room: room.set_tile(numCells, coords, _get_tile_state_from_atlas_x(tile))

func _erase_cell_at(coords: Vector2i, numCells: Vector2i) -> void:
	if numCells == Vector2i.ZERO: return
	if coords.x < 0 || coords.x >= numCells.x || coords.y < 0 || coords.y >= numCells.y:
		return
	erase_cell(coords)
	if room: room.set_tile(numCells, coords, RoomConfig.TILE_STATE_UNSET)

func _maybe_convert_tile(atlasX:NodeTileMapping, isWall:bool) -> NodeTileMapping:
	if atlasX == NodeTileMapping.InnerNoise && isWall:
		return NodeTileMapping.WallFilled
	if atlasX == NodeTileMapping.InnerFilled && isWall:
		return NodeTileMapping.WallFilled
	if atlasX == NodeTileMapping.InnerEmpty && isWall:
		return NodeTileMapping.WallEmpty
	if atlasX == NodeTileMapping.WallFilled && !isWall:
		return NodeTileMapping.InnerFilled
	if atlasX == NodeTileMapping.WallNoise && !isWall:
		return NodeTileMapping.InnerNoise
	if atlasX == NodeTileMapping.WallEmpty && !isWall:
		return NodeTileMapping.InnerEmpty
	return atlasX

func _get_tile_state_from_atlas_x(atlasX:int) -> int:
	if atlasX == NodeTileMapping.InnerNoise:
		return RoomConfig.TILE_STATE_UNSET
	if atlasX == NodeTileMapping.InnerFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == NodeTileMapping.InnerEmpty:
		return RoomConfig.TILE_STATE_EMPTY
	if atlasX == NodeTileMapping.WallFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == NodeTileMapping.WallNoise:
		return RoomConfig.TILE_STATE_UNSET
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

var initialized:bool = false;
func initialize(p_cfg: GlobalConfig, p_room: RoomConfig) -> void:
	if !p_cfg: return
	if !p_room: return
	if processing: return
	processing = true
	cfg = p_cfg
	room = p_room
	var numCells := _get_num_cells()
	clear()
	for y in range(numCells.y):
		for x in range(numCells.x):
			if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
				_set_cell_at(Vector2i(x, y), NodeTileMapping.WallFilled, numCells)
			else:
				_set_cell_at(Vector2i(x, y), NodeTileMapping.InnerNoise, numCells)
	update_internals()
	prevNumCells = numCells
	processing = false
	initialized = true

var processing:bool
func handle_room_size_change() -> void:
	if !cfg: return
	if !room: return
	if !initialized: return
	if processing: return
	var numCells := _get_num_cells();
	if numCells == prevNumCells:
		return
	if numCells.x <= 0 || numCells.y <= 0:
		return
	if prevNumCells.x < 0 || prevNumCells.y < 0:
		printerr("invalid prevNumCells (", prevNumCells.x, ",", prevNumCells.y, ") - did you forget to call initialize()?")
		return
	processing = true
	# convert existing tile data from prev coords to new coords
	for y in range(prevNumCells.y):
		for x in range(prevNumCells.x):
			var coords := Vector2i(x, y)
			var tile := get_cell_atlas_coords(coords).x as NodeTileMapping
			if room: room.set_tile(numCells, coords, _get_tile_state_from_atlas_x(tile))
	# unset previous border tile, and copy tile to new border tile
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
				_set_cell_at(prevCoords, NodeTileMapping.InnerNoise, numCells)
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
				_set_cell_at(prevCoords, NodeTileMapping.InnerNoise, numCells)
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
					_set_cell_at(Vector2i(x, y), NodeTileMapping.InnerNoise, numCells)
	if numCells.y > prevNumCells.y:
		for y in range(prevNumCells.y, numCells.y-1):
			for x in range(numCells.x):
				if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.WallFilled, numCells)
				else:
					_set_cell_at(Vector2i(x, y), NodeTileMapping.InnerNoise,numCells)
	# clear prev outer cells
	if numCells.x < prevNumCells.x:
		for y in range(numCells.y):
			for x in range(numCells.x, prevNumCells.x):
				erase_cell(Vector2i(x, y))
	if numCells.y < prevNumCells.y:
		for y in range(numCells.y, prevNumCells.y):
			for x in range(numCells.x):
				erase_cell(Vector2i(x, y))
	if room: room.notify_changed()
	update_internals()
	prevNumCells = numCells
	processing = false
