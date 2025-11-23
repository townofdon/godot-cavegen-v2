class_name RoomSelectOverlay
extends Control

signal select_neighbor_requested(dir: Vector2i)
signal add_neighbor_requested(dir: Vector2i, border_mask: int)
signal move_room_requested(dir: Vector2i)
signal delete_room_requested

@onready var button_move: Button = %ButtonMove
@onready var button_delete: Button = %ButtonDelete
@onready var button_move_up: Button = %ButtonMoveUp
@onready var button_move_down: Button = %ButtonMoveDown
@onready var button_move_left: Button = %ButtonMoveLeft
@onready var button_move_right: Button = %ButtonMoveRight
@onready var button_close: Button = %ButtonClose
@onready var button_north_add_room: Button = %ButtonNorthAddRoom
@onready var button_north_select_room: Button = %ButtonNorthSelectRoom
@onready var button_south_add_room: Button = %ButtonSouthAddRoom
@onready var button_south_select_room: Button = %ButtonSouthSelectRoom
@onready var button_west_add_room: Button = %ButtonWestAddRoom
@onready var button_west_select_room: Button = %ButtonWestSelectRoom
@onready var button_east_add_room: Button = %ButtonEastAddRoom
@onready var button_east_select_room: Button = %ButtonEastSelectRoom

@onready var button_border_toggle_full: Button = %ButtonBorderToggleFull
@onready var border_indicator_up: TextureRect = %BorderIndicatorUp
@onready var border_indicator_down: TextureRect = %BorderIndicatorDown
@onready var border_indicator_left: TextureRect = %BorderIndicatorLeft
@onready var border_indicator_right: TextureRect = %BorderIndicatorRight
@onready var button_border_toggle_left: TextureButton = %ButtonBorderToggleLeft
@onready var button_border_toggle_right: TextureButton = %ButtonBorderToggleRight
@onready var button_border_toggle_up: TextureButton = %ButtonBorderToggleUp
@onready var button_border_toggle_down: TextureButton = %ButtonBorderToggleDown

@onready var buttons: Array[Button] = [
	button_move,
	button_delete,
	button_move_up,
	button_move_down,
	button_move_left,
	button_move_right,
	button_close,
	button_north_add_room,
	button_north_select_room,
	button_south_add_room,
	button_south_select_room,
	button_west_add_room,
	button_west_select_room,
	button_east_add_room,
	button_east_select_room,
]

enum Mode {
	Select,
	Move,
}
enum BorderMask {
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8,
}

var mode: Mode = Mode.Select
var room: RoomConfig = RoomConfig.new()
var border_mask := 0

func set_room(p_room: RoomConfig)->void:
	room = p_room
	call_deferred("_rerender")

func _set_mode(p_mode: Mode, should_rerender: bool = true)->void:
	mode = p_mode
	StatusBarNotifs.room_select_mode_changed.emit(p_mode)
	if should_rerender: _rerender()

func _set_full_mask(p_mask: int) -> void:
	border_mask = p_mask
	_rerender_mask_buttons()

func _set_mask_on(p_mask: int) -> void:
	border_mask = border_mask | p_mask
	_rerender_mask_buttons()

func _set_mask_off(p_mask: int) -> void:
	border_mask = border_mask & (~p_mask)
	_rerender_mask_buttons()

func _toggle_mask(p_mask: int) -> void:
	assert(
		p_mask == BorderMask.UP ||
		p_mask == BorderMask.DOWN ||
		p_mask == BorderMask.LEFT ||
		p_mask == BorderMask.RIGHT
	)
	border_mask = border_mask ^ p_mask

func _ready() -> void:
	button_move.pressed.connect(_set_mode.bind(Mode.Move))
	button_close.pressed.connect(_set_mode.bind(Mode.Select))
	button_delete.pressed.connect(_on_delete_room_requested)
	button_move_up.pressed.connect(_on_move_room_requested.bind(Vector2i.UP))
	button_move_down.pressed.connect(_on_move_room_requested.bind(Vector2i.DOWN))
	button_move_left.pressed.connect(_on_move_room_requested.bind(Vector2i.LEFT))
	button_move_right.pressed.connect(_on_move_room_requested.bind(Vector2i.RIGHT))
	button_north_select_room.pressed.connect(_on_select_neighbor.bind(Vector2i.UP))
	button_south_select_room.pressed.connect(_on_select_neighbor.bind(Vector2i.DOWN))
	button_west_select_room.pressed.connect(_on_select_neighbor.bind(Vector2i.LEFT))
	button_east_select_room.pressed.connect(_on_select_neighbor.bind(Vector2i.RIGHT))
	button_north_add_room.pressed.connect(_on_add_neighbor.bind(Vector2i.UP))
	button_south_add_room.pressed.connect(_on_add_neighbor.bind(Vector2i.DOWN))
	button_west_add_room.pressed.connect(_on_add_neighbor.bind(Vector2i.LEFT))
	button_east_add_room.pressed.connect(_on_add_neighbor.bind(Vector2i.RIGHT))

	button_border_toggle_full.pressed.connect(_on_toggle_full_mask)
	button_border_toggle_up.toggled.connect(_on_toggle_edge_mask.bind(BorderMask.UP))
	button_border_toggle_down.toggled.connect(_on_toggle_edge_mask.bind(BorderMask.DOWN))
	button_border_toggle_left.toggled.connect(_on_toggle_edge_mask.bind(BorderMask.LEFT))
	button_border_toggle_right.toggled.connect(_on_toggle_edge_mask.bind(BorderMask.RIGHT))

	visibility_changed.connect(_on_visibility_changed)
	_rerender()
	_rerender_mask_buttons()

func _process(_delta: float) -> void:
	# handle input
	if !visible || !is_visible_in_tree():
		return
	if !room:
		return
	var dir := Vector2i.ZERO
	if Input.is_action_just_pressed("ui_up"):
		dir = Vector2i.UP
	elif Input.is_action_just_pressed("ui_down"):
		dir = Vector2i.DOWN
	elif Input.is_action_just_pressed("ui_left"):
		dir = Vector2i.LEFT
	elif Input.is_action_just_pressed("ui_right"):
		dir = Vector2i.RIGHT
	if dir == Vector2i.ZERO:
		return
	if mode == Mode.Select:
		if _has_neighbor(dir):
			_on_select_neighbor(dir)
		else:
			_on_add_neighbor(dir)
	elif mode == Mode.Move:
		if !_has_neighbor(dir):
			_on_move_room_requested(dir)

func _has_neighbor(dir: Vector2i) -> bool:
	if !room:
		return false
	var has_neighbor_north: bool = !!room.get_node_up()
	var has_neighbor_south: bool = !!room.get_node_down()
	var has_neighbor_west: bool = !!room.get_node_left()
	var has_neighbor_east: bool = !!room.get_node_right()
	if dir == Vector2i.UP:
		return has_neighbor_north
	if dir == Vector2i.DOWN:
		return has_neighbor_south
	if dir == Vector2i.LEFT:
		return has_neighbor_west
	if dir == Vector2i.RIGHT:
		return has_neighbor_east
	return false

func _on_toggle_full_mask() -> void:
	if border_mask < 15:
		_set_full_mask(15)
	else:
		_set_full_mask(0)

func _on_toggle_edge_mask(toggled_on: bool, p_mask: int) -> void:
	if toggled_on:
		_set_mask_on(p_mask)
	else:
		_set_mask_off(p_mask)
	_rerender_mask_buttons()

func _on_visibility_changed()->void:
	_set_mode(Mode.Select, false)
	call_deferred("_rerender")
	call_deferred("_rerender_mask_buttons")

func _on_delete_room_requested()->void:
	delete_room_requested.emit()
	_disable_all_buttons()

func _on_move_room_requested(dir: Vector2i)->void:
	move_room_requested.emit(dir)
	_disable_all_buttons()

func _on_select_neighbor(dir: Vector2i)->void:
	select_neighbor_requested.emit(dir)
	_disable_all_buttons()

func _on_add_neighbor(dir: Vector2i)->void:
	add_neighbor_requested.emit(dir, border_mask)
	_disable_all_buttons()

func _rerender_mask_buttons() -> void:
	border_indicator_up.hide()
	border_indicator_down.hide()
	border_indicator_left.hide()
	border_indicator_right.hide()
	button_border_toggle_up.set_pressed_no_signal(false)
	button_border_toggle_down.set_pressed_no_signal(false)
	button_border_toggle_left.set_pressed_no_signal(false)
	button_border_toggle_right.set_pressed_no_signal(false)
	if (border_mask & BorderMask.UP) > 0:
		border_indicator_up.show()
		button_border_toggle_up.set_pressed_no_signal(true)
	if (border_mask & BorderMask.DOWN) > 0:
		border_indicator_down.show()
		button_border_toggle_down.set_pressed_no_signal(true)
	if (border_mask & BorderMask.LEFT) > 0:
		border_indicator_left.show()
		button_border_toggle_left.set_pressed_no_signal(true)
	if (border_mask & BorderMask.RIGHT) > 0:
		border_indicator_right.show()
		button_border_toggle_right.set_pressed_no_signal(true)

func _rerender() -> void:
	_enable_all_buttons()
	button_move.hide()
	button_delete.hide()
	button_move_up.hide()
	button_move_down.hide()
	button_move_left.hide()
	button_move_right.hide()
	button_close.hide()
	button_north_add_room.hide()
	button_north_select_room.hide()
	button_south_add_room.hide()
	button_south_select_room.hide()
	button_west_add_room.hide()
	button_west_select_room.hide()
	button_east_add_room.hide()
	button_east_select_room.hide()
	button_move_up.disabled = true
	button_move_down.disabled = true
	button_move_left.disabled = true
	button_move_right.disabled = true
	if !room:
		return
	var is_root: bool = room.internal__grid_position == Vector2i.ZERO
	var has_neighbor_north: bool = !!room.get_node_up()
	var has_neighbor_south: bool = !!room.get_node_down()
	var has_neighbor_west: bool = !!room.get_node_left()
	var has_neighbor_east: bool = !!room.get_node_right()
	if mode == Mode.Select:
		button_move.show()
		if !is_root:
			button_delete.show()
		if has_neighbor_north:
			button_north_select_room.show()
		else:
			button_north_add_room.show()
		if has_neighbor_south:
			button_south_select_room.show()
		else:
			button_south_add_room.show()
		if has_neighbor_west:
			button_west_select_room.show()
		else:
			button_west_add_room.show()
		if has_neighbor_east:
			button_east_select_room.show()
		else:
			button_east_add_room.show()
	elif mode == Mode.Move:
		button_close.show()
		button_move_up.show()
		button_move_down.show()
		button_move_left.show()
		button_move_right.show()
		if !has_neighbor_north:
			button_move_up.disabled = false
		if !has_neighbor_south:
			button_move_down.disabled = false
		if !has_neighbor_west:
			button_move_left.disabled = false
		if !has_neighbor_east:
			button_move_right.disabled = false

func _disable_all_buttons()->void:
	for button:Button in buttons:
		button.disabled = true

func _enable_all_buttons()->void:
	for button:Button in buttons:
		button.disabled = false
