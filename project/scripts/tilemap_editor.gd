extends TileMapLayer
class_name TileMapEditor

@onready var selectionLayer:TileMapLayer = %SelectionLayer

signal on_mode_changed(mode: EditorMode)

var cfg:GlobalConfig
var room:RoomConfig

enum Tile {
	Null = -1,
	InnerEmpty = 0,
	InnerNoise = 1,
	InnerFilled = 2,
	WallEmpty = 3,
	WallNoise = 4,
	WallFilled = 5,
}

enum SelectTile {
	Null = -1,
	Fill = 1,
	Erase = 2,
	Anchor = 3,
}

enum EditorMode {
	Draw,
	Erase,
	Line,
	Rect,
	Fill,
}

const TILEMAP_SOURCE_ID := 2

var mode:EditorMode = EditorMode.Draw
var drawTile := Tile.Null
var anchorTile := Tile.Null
var prevNumCells := Vector2i(-1,-1)
var lastTileDrawnCoords := Vector2i(-1,-1)
var anchorCoords:Vector2i = Vector2i(-1,-1)

func set_editor_mode(p_mode: EditorMode) -> void:
	mode = p_mode
	on_mode_changed.emit(mode)

func _process(_delta: float) -> void:
	if !visible || !is_visible_in_tree():
		return
	var coords := local_to_map(get_local_mouse_position())
	var alt_pressed := Input.is_action_pressed("tile_alt")
	var shift_pressed := Input.is_action_pressed("tile_shift")
	var mouse_l_pressed := Input.is_action_pressed("mouse_btn_left")
	var mouse_r_pressed := Input.is_action_pressed("mouse_btn_right")
	var cancel_pressed := Input.is_action_pressed("ui_cancel")
	var has_anchor := anchorCoords != coords && anchorCoords != Vector2i(-1,-1)

	if (shift_pressed && has_anchor):
		drawTile = anchorTile

	# draw mode
	if mode == EditorMode.Draw:
		#selectionLayer.clear()
		## anchor / select
		#if (shift_pressed && has_anchor):
			#selectionLayer.set_cell(anchorCoords, 3, Vector2i(SelectTile.Anchor, 0))
			#selectionLayer.set_cell(coords, 3, Vector2i(SelectTile.Fill, 0))
		#else:
			#if (alt_pressed || mouse_r_pressed):
				#selectionLayer.set_cell(coords, 3, Vector2i(SelectTile.Erase, 0))
			#else:
				#selectionLayer.set_cell(coords, 3, Vector2i(SelectTile.Fill, 0))
		# fill tile
		if (mouse_l_pressed && !alt_pressed):
			var currentTile := get_cell_atlas_coords(coords).x as Tile
			if drawTile == Tile.Null: drawTile = _get_next_tile_to_lay(currentTile, true);
			if (shift_pressed && has_anchor):
				_line_at(anchorCoords, coords, drawTile)
			else:
				_draw_at(coords, drawTile)
			anchorCoords = coords
			anchorTile = drawTile
		# erase tile
		elif ((mouse_l_pressed && alt_pressed) || mouse_r_pressed):
			var currentTile := get_cell_atlas_coords(coords).x as Tile
			if drawTile == Tile.Null: drawTile = _get_next_tile_to_lay(currentTile, false);
			if (shift_pressed && has_anchor):
				_line_at(anchorCoords, coords, drawTile)
			else:
				_draw_at(coords, drawTile)
			anchorCoords = coords
			anchorTile = drawTile
		else:
			lastTileDrawnCoords = Vector2i(-1,-1)
			drawTile = Tile.Null
	elif mode == EditorMode.Erase:
		pass
	elif mode == EditorMode.Line:
		# TODO: ADD LINE MODE
		pass
	elif mode == EditorMode.Fill:
		# TODO: ADD FILL MODE
		pass
	elif mode == EditorMode.Rect:
		# TODO: ADD RECT MODE
		pass
	queue_redraw()

func _draw_at(coords: Vector2i, tile: Tile):
	_user_set_cell_at(coords, tile)

#func _erase_at(coords: Vector2i):
	#var currentTile := get_cell_atlas_coords(coords).x as Tile
	#drawTile = _get_next_tile_to_lay(currentTile, false);
	#_user_set_cell_at(coords, drawTile)

func _line_at(from: Vector2i, to: Vector2i, tile: Tile):
	var numSteps := maxi(absi(from.x - to.x), absi(from.y - to.y)) * 2
	if (numSteps <= 0):
		return
	elif (numSteps <= 1):
		_draw_at(from, tile)
		#if (erase):
			#_erase_at(from)
		#else:
			#_draw_at(from)
	else:
		for i in range(numSteps):
			var x := roundi(lerpf(from.x, to.x, i / float(numSteps - 1)))
			var y := roundi(lerpf(from.y, to.y, i / float(numSteps - 1)))
			var coords := Vector2i(x, y)
			_draw_at(coords, tile)
			#if (erase):
				#_erase_at(coords)
			#else:
				#_draw_at(coords)

func _user_rect_at(from: Vector2i, to: Vector2i):
	pass

func _user_fill_at(coords: Vector2i):
	pass

func _get_next_tile_to_lay(current: Tile, fill: bool) -> Tile:
	if fill:
		if current == Tile.InnerFilled:
			return Tile.InnerFilled
		if current == Tile.InnerNoise:
			return Tile.InnerFilled
		elif current == Tile.InnerEmpty:
			return Tile.InnerNoise
		elif current == Tile.WallEmpty:
			return Tile.WallNoise
		elif current == Tile.WallNoise:
			return Tile.WallFilled
		elif current == Tile.WallFilled:
			return Tile.WallFilled
	else:
		if current == Tile.InnerEmpty:
			return Tile.InnerEmpty
		if current == Tile.InnerNoise:
			return Tile.InnerEmpty
		elif current == Tile.InnerFilled:
			return Tile.InnerNoise
		elif current == Tile.WallFilled:
			return Tile.WallNoise
		elif current == Tile.WallNoise:
			return Tile.WallEmpty
		elif current == Tile.WallEmpty:
			return Tile.WallEmpty
	return Tile.Null

func _user_set_cell_at(coords: Vector2i, tile: Tile) -> void:
	if coords == lastTileDrawnCoords: return
	if tile == Tile.Null: return
	var numCells := _get_num_cells()
	_set_cell_at(coords, tile, numCells)
	lastTileDrawnCoords = coords
	if room: room.notify_changed()

func _get_tile_to_place(coords: Vector2i, tile: Tile) -> Tile:
	if tile == Tile.Null: return Tile.Null
	var numCells := _get_num_cells()
	if coords.x < 0 || coords.x >= numCells.x || coords.y < 0 || coords.y >= numCells.y:
		return Tile.Null
	var isWall := coords.x==0 || coords.x==numCells.x-1 || coords.y==0 || coords.y==numCells.y-1
	tile = _maybe_convert_tile(tile, isWall)
	return tile	

func _set_cell_at(coords: Vector2i, tile: Tile, numCells: Vector2i) -> void:
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

func _maybe_convert_tile(atlasX:Tile, isWall:bool) -> Tile:
	if atlasX == Tile.InnerNoise && isWall:
		return Tile.WallFilled
	if atlasX == Tile.InnerFilled && isWall:
		return Tile.WallFilled
	if atlasX == Tile.InnerEmpty && isWall:
		return Tile.WallEmpty
	if atlasX == Tile.WallFilled && !isWall:
		return Tile.InnerFilled
	if atlasX == Tile.WallNoise && !isWall:
		return Tile.InnerNoise
	if atlasX == Tile.WallEmpty && !isWall:
		return Tile.InnerEmpty
	return atlasX

func _get_tile_state_from_atlas_x(atlasX:int) -> int:
	if atlasX == Tile.InnerNoise:
		return RoomConfig.TILE_STATE_UNSET
	if atlasX == Tile.InnerFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == Tile.InnerEmpty:
		return RoomConfig.TILE_STATE_EMPTY
	if atlasX == Tile.WallFilled:
		return RoomConfig.TILE_STATE_FILLED
	if atlasX == Tile.WallNoise:
		return RoomConfig.TILE_STATE_UNSET
	if atlasX == Tile.WallEmpty:
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
				_set_cell_at(Vector2i(x, y), Tile.WallFilled, numCells)
			else:
				_set_cell_at(Vector2i(x, y), Tile.InnerNoise, numCells)
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
			var tile := get_cell_atlas_coords(coords).x as Tile
			if room: room.set_tile(numCells, coords, _get_tile_state_from_atlas_x(tile))
	# unset previous border tile, and copy tile to new border tile
	if numCells.x != prevNumCells.x:
		for y in range(numCells.y):
			var prevCoords := Vector2i(prevNumCells.x - 1, y)
			var nextCoords := Vector2i(numCells.x - 1, y)
			var currentAtlas := get_cell_atlas_coords(prevCoords)
			var currentTile:Tile = currentAtlas.x as Tile\
				if currentAtlas.x > -1\
				else Tile.WallFilled
			# unset previous border
			if numCells.x > prevNumCells.x:
				_set_cell_at(prevCoords, Tile.InnerNoise, numCells)
			else:
				_erase_cell_at(prevCoords, numCells)
			# copy result to new border
			_set_cell_at(nextCoords, currentTile, numCells)
	if prevNumCells.y != numCells.y:
		for x in range(numCells.x):
			var prevCoords := Vector2i(x, prevNumCells.y - 1)
			var nextCoords := Vector2i(x, numCells.y - 1)
			var currentAtlas := get_cell_atlas_coords(prevCoords)
			var currentTile:Tile = currentAtlas.x as Tile\
				if currentAtlas.x > -1\
				else Tile.WallFilled
			# unset previous border
			if numCells.y > prevNumCells.y:
				_set_cell_at(prevCoords, Tile.InnerNoise, numCells)
			else:
				_erase_cell_at(prevCoords, numCells)
			# copy result to new border
			_set_cell_at(nextCoords, currentTile, numCells)
	# set new inner cells
	if numCells.x > prevNumCells.x:
		for y in range(numCells.y):
			for x in range(prevNumCells.x, numCells.x-1):
				if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
					_set_cell_at(Vector2i(x, y), Tile.WallFilled, numCells)
				else:
					_set_cell_at(Vector2i(x, y), Tile.InnerNoise, numCells)
	if numCells.y > prevNumCells.y:
		for y in range(prevNumCells.y, numCells.y-1):
			for x in range(numCells.x):
				if x==0 || x==numCells.x-1 || y==0 || y==numCells.y-1:
					_set_cell_at(Vector2i(x, y), Tile.WallFilled, numCells)
				else:
					_set_cell_at(Vector2i(x, y), Tile.InnerNoise,numCells)
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
