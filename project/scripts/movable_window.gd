class_name MovableWindow
extends Panel

signal closed

@onready var button_move: Button = %ButtonMove
@onready var button_resize_left: Button = %ButtonResizeLeft
@onready var button_resize_right: Button = %ButtonResizeRight
@onready var button_resize_top: Button = %ButtonResizeTop
@onready var button_resize_bottom: Button = %ButtonResizeBottom
@onready var button_resize_top_left: Button = %ButtonResizeTopLeft
@onready var button_resize_bottom_left: Button = %ButtonResizeBottomLeft
@onready var button_resize_bottom_right: Button = %ButtonResizeBottomRight
@onready var button_close: Button = %ButtonCloseNoisePreview

enum Mode {
	None,
	Move,
	ResizeLeft,
	ResizeRight,
	ResizeUp,
	ResizeDown,
	ResizeTopLeft,
	ResizeBottomLeft,
	ResizeBottomRight,
}

var _mode := Mode.None
var _anchor_point: Vector2
var _anchor_offset: Vector2
var _anchor_size: Vector2
var _initial_size: Vector2
var _initial_pos: Vector2

func _ready() -> void:
	button_close.pressed.connect(_on_close)
	button_move.button_down.connect(_reposition.bind(Mode.Move))
	button_move.button_up.connect(_cancel_reposition)
	button_resize_left.button_down.connect(_reposition.bind(Mode.ResizeLeft))
	button_resize_left.button_up.connect(_cancel_reposition)
	button_resize_top.button_down.connect(_reposition.bind(Mode.ResizeUp))
	button_resize_top.button_up.connect(_cancel_reposition)
	button_resize_right.button_down.connect(_reposition.bind(Mode.ResizeRight))
	button_resize_right.button_up.connect(_cancel_reposition)
	button_resize_bottom.button_down.connect(_reposition.bind(Mode.ResizeDown))
	button_resize_bottom.button_up.connect(_cancel_reposition)
	button_resize_top_left.button_down.connect(_reposition.bind(Mode.ResizeTopLeft))
	button_resize_top_left.button_up.connect(_cancel_reposition)
	button_resize_bottom_left.button_down.connect(_reposition.bind(Mode.ResizeBottomLeft))
	button_resize_bottom_left.button_up.connect(_cancel_reposition)
	button_resize_bottom_right.button_down.connect(_reposition.bind(Mode.ResizeBottomRight))
	button_resize_bottom_right.button_up.connect(_cancel_reposition)
	call_deferred("_set_initial_dimensions")

func _on_close()->void:
	hide()
	closed.emit()

func _reposition(p_mode: Mode) -> void:
	if _mode == Mode.None:
		_mode = p_mode
		_anchor_point = get_global_mouse_position()
		_anchor_offset = get_local_mouse_position()
		_anchor_size = size

func _cancel_reposition() -> void:
	_mode = Mode.None

func _set_initial_dimensions()->void:
	_initial_size = size
	_initial_pos = global_position

func _process(_delta: float) -> void:
	var pos := get_global_mouse_position()
	if _mode == Mode.Move:
		global_position = pos - _anchor_offset
		_clamp_position()
	elif _mode == Mode.ResizeLeft:
		_resize(Vector2.LEFT)
	elif _mode == Mode.ResizeRight:
		_resize(Vector2.RIGHT)
	elif _mode == Mode.ResizeUp:
		_resize(Vector2.UP)
	elif _mode == Mode.ResizeDown:
		_resize(Vector2.DOWN)
	elif _mode == Mode.ResizeTopLeft:
		_resize(Vector2(-1, -1))
	elif _mode == Mode.ResizeBottomLeft:
		_resize(Vector2(-1, 1))
	elif _mode == Mode.ResizeBottomRight:
		_resize(Vector2(1, 1))

func _resize(dir: Vector2) -> void:
	var pos := get_global_mouse_position()
	var delta := _anchor_point - pos
	delta = delta * dir.abs()
	if dir.abs() == Vector2(1,1):
		delta = Utils.Vector.project(delta, dir)
	if dir.x < 0:
		delta.x = maxf(delta.x, -absf(_anchor_size.x - _initial_size.x))
	elif dir.x > 0:
		delta.x = minf(delta.x, absf(_anchor_size.x - _initial_size.x))
	if dir.y < 0:
		delta.y = maxf(delta.y, -absf(_anchor_size.y - _initial_size.y))
	elif dir.y > 0:
		delta.y = minf(delta.y, absf(_anchor_size.y - _initial_size.y))
	size = _anchor_size + delta * -dir
	size = size.max(_initial_size)
	_clamp_size()
	global_position = _anchor_point - _anchor_offset - (delta * dir.clampf(-1, 0).abs())
	_clamp_position()

func _clamp_position() -> void:
	var min_extents := Vector2(0,0)
	var max_extents := Vector2(
		get_viewport_rect().size.x - size.x,
		get_viewport_rect().size.y - size.y)
	global_position.x = clampf(global_position.x, min_extents.x, max_extents.x)
	global_position.y = clampf(global_position.y, min_extents.y, max_extents.y)

func _clamp_size() -> void:
	var max_size := Vector2(
		get_viewport_rect().size.x - global_position.x,
		get_viewport_rect().size.y - global_position.y)
	size.x = clampf(size.x, _initial_size.x, max_size.x)
	size.y = clampf(size.y, _initial_size.y, max_size.y)
