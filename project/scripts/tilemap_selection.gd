extends TileMapLayer
class_name TileMapSelection

@export var lineColor := Color.AZURE

@onready var editor := %TileMapEditor

enum SelectTile {
	Null = -1,
	Fill = 1,
	Erase = 2,
	Anchor = 3,
}

const SELECT_SOURCE_ID := 3

#func _draw() -> void:
	#var anchorCoords:Vector2i = editor.anchorCoords
	#var mode:TileMapEditor.EditorMode = editor.mode
	#var coords := local_to_map(get_local_mouse_position())
	#var has_anchor := anchorCoords != coords && anchorCoords != Vector2i(-1,-1)
	#var shift_pressed := Input.is_action_pressed("tile_shift")

	#if mode == TileMapEditor.EditorMode.Draw:
		#if shift_pressed && has_anchor:
			#draw_line(map_to_local(anchorCoords), map_to_local(coords), lineColor, 10)

func _process(_delta: float) -> void:
	var mode:TileMapEditor.EditorMode = editor.mode
	var anchorCoords:Vector2i = editor.anchorCoords
	var coords := local_to_map(get_local_mouse_position())
	var alt_pressed := Input.is_action_pressed("tile_alt")
	var shift_pressed := Input.is_action_pressed("tile_shift")
	var has_anchor := anchorCoords != coords && anchorCoords != Vector2i(-1,-1)

	if mode == TileMapEditor.EditorMode.Draw:
		clear()
		# anchor / select
		if (shift_pressed && has_anchor):
			_line_preview_at(anchorCoords, coords, editor.anchorTile)
			_select_cell_at(anchorCoords, SelectTile.Anchor)
		else:
			if (alt_pressed):
				_select_cell_at(coords, SelectTile.Erase)
			else:
				_select_cell_at(coords, SelectTile.Fill)
	elif mode == TileMapEditor.EditorMode.Fill:
		clear()
		if (alt_pressed):
			_select_cell_at(coords, SelectTile.Erase)
		else:
			_select_cell_at(coords, SelectTile.Fill)
	queue_redraw()

func _select_cell_at(coords: Vector2i, tile: SelectTile) -> void:
	set_cell(coords, SELECT_SOURCE_ID, Vector2i(tile, 0))

func _draw_tile(coords: Vector2i, tile: TileMapEditor.Tile):
	tile = editor._get_tile_to_place(coords, tile)
	if tile != TileMapEditor.Tile.Null:
		set_cell(coords, TileMapEditor.TILEMAP_SOURCE_ID, Vector2i(tile, 0))

func _line_preview_at(from: Vector2i, to: Vector2i, tile: TileMapEditor.Tile):
	var numSteps := maxi(absi(from.x - to.x), absi(from.y - to.y)) * 2
	if (numSteps <= 0):
		return
	elif (numSteps <= 1):
		_draw_tile(from, tile)
	else:
		for i in range(numSteps):
			var x := roundi(lerpf(from.x, to.x, i / float(numSteps - 1)))
			var y := roundi(lerpf(from.y, to.y, i / float(numSteps - 1)))
			var coords := Vector2i(x, y)
			_draw_tile(coords, tile)
