extends TileMapLayer
class_name TileMapSelection

@export var fillColor := Color.AZURE
@export var eraseColor := Color.RED

var editor:TileMapEditor

enum SelectTile {
	Null = -1,
	Fill = 1,
	Erase = 2,
	Anchor = 3,
}

const SELECT_SOURCE_ID := 3

func initialize(p_editor: TileMapEditor) -> void:
	editor = p_editor

func _draw() -> void:
	if !editor: return
	var anchorCoords:Vector2i = editor.anchorCoords
	var anchorTile:TileMapEditor.Tile = editor.anchorTile
	var mode:TileMapEditor.EditorMode = editor.mode
	var coords := local_to_map(get_local_mouse_position())
	var has_anchor := anchorCoords != coords && anchorCoords != Vector2i(-1,-1)
	var alt_pressed := Input.is_action_pressed("tile_alt")
	var shift_pressed := Input.is_action_pressed("tile_shift")
	var mouse_r_pressed := Input.is_action_pressed("mouse_btn_right")

	var shouldPreviewLine := \
		(mode == TileMapEditor.EditorMode.Line && anchorTile != TileMapEditor.Tile.Null && !editor.cancel) ||\
		(mode == TileMapEditor.EditorMode.Draw && has_anchor && shift_pressed && !editor.cancel)

	if shouldPreviewLine:
		var color := fillColor
		if (alt_pressed || mouse_r_pressed):
			color = eraseColor
		var from := anchorCoords - Vector2i(1,1)
		var to := anchorCoords + Vector2i(1,1)
		var fromLocal:Vector2i = map_to_local(from) + Vector2(10, 10)
		var toLocal:Vector2i = map_to_local(to) - Vector2(10, 10)
		draw_rect(Rect2(fromLocal, toLocal - fromLocal), color, false, 5)
	elif mode == TileMapEditor.EditorMode.Rect && anchorTile != TileMapEditor.Tile.Null && !editor.cancel:
		var color := fillColor
		if (alt_pressed || mouse_r_pressed):
			color = eraseColor
		var from := Vector2i(
			mini(anchorCoords.x, coords.x) - 1,
			mini(anchorCoords.y, coords.y) - 1
		)
		var to := Vector2i(
			maxi(anchorCoords.x, coords.x) + 1,
			maxi(anchorCoords.y, coords.y) + 1
		)
		var fromLocal:Vector2i = map_to_local(from) + Vector2(10, 10)
		var toLocal:Vector2i = map_to_local(to) - Vector2(10, 10)
		draw_rect(Rect2(fromLocal, toLocal - fromLocal), color, false, 5)

func _process(_delta: float) -> void:
	if !editor: return
	if !enabled || !visible || !is_visible_in_tree():
		return
	var mode:TileMapEditor.EditorMode = editor.mode
	var anchorCoords:Vector2i = editor.anchorCoords
	var anchorTile:TileMapEditor.Tile = editor.anchorTile
	var coords := local_to_map(get_local_mouse_position())
	var alt_pressed := Input.is_action_pressed("tile_alt")
	var shift_pressed := Input.is_action_pressed("tile_shift")
	var mouse_r_pressed := Input.is_action_pressed("mouse_btn_right")
	var has_anchor := anchorCoords != coords && anchorCoords != Vector2i(-1,-1)
	var numCells:Vector2i = editor.cfg.get_num_cells_2d()

	var coords2 := Vector2i(
		clampi(coords.x, 0, numCells.x - 1),
		clampi(coords.y, 0, numCells.y - 1),
	)
	var mouse_inside := coords2 == coords
	coords = coords2

	clear()
	if mode == TileMapEditor.EditorMode.Draw:
		# anchor / select
		if (shift_pressed && has_anchor):
			_line_preview_at(anchorCoords, coords, editor.anchorTile)
			#_select_cell_at(anchorCoords, SelectTile.Anchor)
		else:
			if (alt_pressed):
				if mouse_inside: _select_cell_at(coords, SelectTile.Erase)
			else:
				if mouse_inside: _select_cell_at(coords, SelectTile.Fill)
	elif mode == TileMapEditor.EditorMode.Line:
		if anchorTile != TileMapEditor.Tile.Null && !editor.cancel:
			_line_preview_at(anchorCoords, coords, editor.anchorTile)
		else:
			if (alt_pressed || mouse_r_pressed):
				if mouse_inside: _select_cell_at(coords, SelectTile.Erase)
			else:
				if mouse_inside: _select_cell_at(coords, SelectTile.Fill)
	elif mode == TileMapEditor.EditorMode.Fill:
		_fill_preview_at(coords, !alt_pressed)
		if (alt_pressed):
			if mouse_inside: _select_cell_at(coords, SelectTile.Erase)
		else:
			if mouse_inside: _select_cell_at(coords, SelectTile.Fill)
	elif mode == TileMapEditor.EditorMode.Rect:
		if anchorTile != TileMapEditor.Tile.Null && !editor.cancel:
			_rect_preview_at(anchorCoords, coords, editor.anchorTile)
		else:
			if (alt_pressed || mouse_r_pressed):
				if mouse_inside: _select_cell_at(coords, SelectTile.Erase)
			else:
				if mouse_inside: _select_cell_at(coords, SelectTile.Fill)
	queue_redraw()

func _select_cell_at(coords: Vector2i, tile: SelectTile) -> void:
	set_cell(coords, SELECT_SOURCE_ID, Vector2i(tile, 0))

func _preview_tile(coords: Vector2i, tile: TileMapEditor.Tile) -> void:
	tile = editor._get_tile_to_place(coords, tile)
	if tile != TileMapEditor.Tile.Null:
		set_cell(coords, TileMapEditor.TILEMAP_SOURCE_ID, Vector2i(tile, 0))

func _line_preview_at(from: Vector2i, to: Vector2i, tile: TileMapEditor.Tile) -> void:
	var numSteps := maxi(absi(from.x - to.x), absi(from.y - to.y)) * 2
	if (numSteps <= 0):
		return
	elif (numSteps <= 1):
		_preview_tile(from, tile)
	else:
		for i in range(numSteps):
			var x := roundi(lerpf(from.x, to.x, i / float(numSteps - 1)))
			var y := roundi(lerpf(from.y, to.y, i / float(numSteps - 1)))
			var coords := Vector2i(x, y)
			_preview_tile(coords, tile)

func _rect_preview_at(from: Vector2i, to: Vector2i, tile: TileMapEditor.Tile) -> void:
	for y in range(mini(from.y, to.y), maxi(from.y, to.y) + 1):
		for x in range(mini(from.x, to.x), maxi(from.x, to.x) + 1):
			var coords := Vector2i(x, y)
			_preview_tile(coords, tile)

func _fill_preview_at(coords: Vector2i, fill: bool) -> void:
	var currentTile := editor.get_cell_atlas_coords(coords).x as TileMapEditor.Tile
	var fillTile:TileMapEditor.Tile = editor._get_next_tile_to_lay(currentTile, fill);
	if currentTile == fillTile:
		return
	var screen := PackedInt32Array()
	var numCells:Vector2i = editor.cfg.get_num_cells_2d()
	for y in range(numCells.y):
		for x in range(numCells.x):
			var tile := editor.get_cell_atlas_coords(Vector2i(x, y)).x as TileMapEditor.Tile
			screen.append(tile)
	FloodFill.fill(screen, numCells, coords, currentTile, fillTile, 4)
	for y in range(numCells.y):
		for x in range(numCells.x):
			var i := x + y * numCells.x
			_preview_tile(Vector2i(x, y), screen.get(i))
