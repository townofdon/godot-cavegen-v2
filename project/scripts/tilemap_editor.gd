extends TileMapLayer
class_name TileMapEditor

@onready var selectionLayer:TileMapLayer = %SelectionLayer

enum NodeState {
	Unset = 0,
	Filled,
	Empty,
}
enum NodeTileMapping {
	InheritNoise = 0,
	InnerFilled = 3,
	InnerEmpty = 4,
	WallFilled = 5,
	WallEmpty = 6,
}

const TILEMAP_SOURCE_ID := 2

signal on_set_cell(position: Vector2i, newState: NodeState)

var prevTileSet := Vector2i(-1,-1)

func _process(_delta: float) -> void:
	selectionLayer.clear()
	var pos := local_to_map(get_local_mouse_position())

	selectionLayer.set_cell(pos, 3, Vector2i(1, 0))

	# get mouse position
	# clear selection layer
	# show selection at mouse hover
	# get L mouse pressed
	# get R mouse pressed
	var atlasCoords = get_cell_atlas_coords(pos)
	if(Input.is_action_pressed("mouse_btn_left")):
		fill_cell_at(pos, atlasCoords)
	elif(Input.is_action_pressed("mouse_btn_right")):
		clear_cell_at(pos, atlasCoords)
	else:
		prevTileSet = Vector2i(-1,-1)

func fill_cell_at(coords: Vector2i, atlasCoords: Vector2i) -> void:
	var tileData := get_cell_tile_data(coords)
	if !tileData:
		return
	if atlasCoords.x == NodeTileMapping.InheritNoise:
		set_cell_at(coords, NodeTileMapping.InnerFilled)
	elif atlasCoords.x == NodeTileMapping.InnerEmpty:
		set_cell_at(coords, NodeTileMapping.InheritNoise)
	elif atlasCoords.x == NodeTileMapping.WallEmpty:
		set_cell_at(coords, NodeTileMapping.WallFilled)

func clear_cell_at(coords: Vector2i, atlasCoords: Vector2i) -> void:
	var tileData := get_cell_tile_data(coords)
	if !tileData:
		return
	if atlasCoords.x == NodeTileMapping.InheritNoise:
		set_cell_at(coords, NodeTileMapping.InnerEmpty)
	elif atlasCoords.x == NodeTileMapping.InnerFilled:
		set_cell_at(coords, NodeTileMapping.InheritNoise)
	elif atlasCoords.x == NodeTileMapping.WallFilled:
		set_cell_at(coords, NodeTileMapping.WallEmpty)

func set_cell_at(coords: Vector2i, tile: NodeTileMapping) -> void:
	if coords == prevTileSet:
		return
	set_cell(coords, TILEMAP_SOURCE_ID, Vector2i(tile, 0))
	prevTileSet = coords
