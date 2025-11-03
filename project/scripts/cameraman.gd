class_name CameraMan
extends Node3D

@export var acceleration := 13.0
@export var moveSpeed := 5.0
@export var lookSpeed := 5.0

@onready var camera: Camera3D = %Camera3D

var _is_mouse_in_window := false
var move:Vector3 = Vector3.ZERO
var look:Vector2 = Vector2.ZERO
var prev_position:Vector3 = Vector3.ZERO
var velocity:Vector3 = Vector3.ZERO
var looking:bool = false

var control_disabled := true
var mode := CameraMode.Fly

enum CameraMode {
	Orbit,
	Fly,
	Preview,
}

var room_position:Vector3 = Vector3.ZERO
var cfg:GlobalConfig

func initialize(cavegen: CaveGen, p_cfg: GlobalConfig, p_room_position: Vector3) -> void:
	cfg = p_cfg
	cavegen.mode_changed.connect(_on_cavegen_mode_changed)
	cfg.on_changed.connect(func(_cfg):
		var target := room_position + _get_target_offset(_cfg)
		camera.global_position.y = target.y
		_on_cavegen_mode_changed(cavegen.mode))
	room_position = p_room_position
	camera.global_position = p_room_position + _get_target_offset(cfg)

func _ready() -> void:
	_initialize_look()

func _initialize_look() -> void:
	# x and y are swapped here bc they represent the axis being rotated around.
	look.x = camera.transform.basis.get_euler().y / -0.001
	look.y = camera.transform.basis.get_euler().x / -0.001

func _get_active_plane_y() -> float:
	var ceiling_y := cfg.room_height * cfg.ceiling
	var active_y := ceiling_y - cfg.active_plane_offset
	return maxf(active_y, 2)

func _get_target_offset(_cfg: GlobalConfig = cfg) -> Vector3:
	var offset := -_get_active_plane_y() * int(cfg.move_active_plane_to_origin)
	var y := cfg.room_height * cfg.ceiling - cfg.active_plane_offset + offset + 27
	return Vector3(_cfg.room_width / 2, y, _cfg.room_depth / 2)

func _tween_to_position(p_offset:Vector3, r_offset:Vector3, c_offset: Vector2, size: float, duration: float) -> void:
	var target_pos := room_position + _get_target_offset(cfg) + p_offset
	var target_rot := Vector3(
		deg_to_rad(-90),
		deg_to_rad(0),
		deg_to_rad(0)
	) + r_offset
	var target_size := (maxf(cfg.room_width, cfg.room_depth) + 10) * size
	var tween := get_tree().create_tween().set_parallel(true)
	tween.set_ease(Tween.EASE_OUT)
	tween.set_trans(Tween.TRANS_QUINT)
	tween.tween_property(camera, "global_position", target_pos, duration)
	tween.tween_property(camera, "rotation", target_rot, duration)
	tween.tween_property(camera, "size", target_size, duration)
	tween.tween_property(camera, "h_offset", c_offset.x, duration)
	tween.tween_property(camera, "v_offset", c_offset.y, duration)
	tween.chain().tween_callback(func(): _initialize_look())

func _on_cavegen_mode_changed(cavegen_mode: CaveGen.Mode) -> void:
	var extent := maxf(cfg.room_width, cfg.room_depth)
	var t := inverse_lerp(30, 50, extent)
	var x := lerpf(18.17, 26.67, t)
	var y := lerpf(0, -0.86, t)
	control_disabled = true
	if cavegen_mode == CaveGen.Mode.RoomSelect:
		camera.projection = Camera3D.PROJECTION_ORTHOGONAL
		_tween_to_position(Vector3.ZERO, Vector3.ZERO, Vector2.ZERO, 2, .7)
	elif cavegen_mode == CaveGen.Mode.RoomConfig:
		var p_offset := Vector3(0, 0, 15.65)
		var r_offset := Vector3(deg_to_rad(30), 0, 0)
		camera.projection = Camera3D.PROJECTION_ORTHOGONAL
		_tween_to_position(p_offset, r_offset, Vector2(x, y), 1.0, .7)
	elif cavegen_mode == CaveGen.Mode.TileEditor:
		camera.projection = Camera3D.PROJECTION_ORTHOGONAL
		_tween_to_position(Vector3.ZERO, Vector3.ZERO, Vector2(x, y), 1.0, .7)
	elif cavegen_mode == CaveGen.Mode.Preview:
		_tween_to_position(Vector3.ZERO, Vector3.ZERO, Vector2.ZERO, 1.0, .7)
		camera.projection = Camera3D.PROJECTION_PERSPECTIVE
		control_disabled = false

func _physics_process(delta: float) -> void:
	if control_disabled:
		velocity = global_position - prev_position
		prev_position = global_position
		_initialize_look()
	elif mode == CameraMode.Fly:
		_handle_fly_movement(delta)

func _handle_fly_movement(delta: float) -> void:
	if (Input.is_action_pressed("looking")):
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
		looking = true
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		looking = false

	# Mouse movement.
	if looking:
		look.y = clamp(look.y, -1560, 1560)
		camera.transform.basis = Basis.from_euler(Vector3(look.y * -0.001, look.x * -0.001, 0))

	# Keyboard input
	var inputH := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
	var inputV := 0.0
	if Input.is_action_pressed("move_down"):
		inputV -= 1.0
	if Input.is_action_pressed("move_up"):
		inputV += 1.0
	var movement := camera.transform.basis * (Vector3(inputH.x, inputV, inputH.y))
	movement = Vector3(movement.x, movement.y, movement.z) * moveSpeed * 0.1
	velocity = velocity.lerp(movement, 1 - exp(-acceleration * delta))
	camera.global_position = camera.global_position + velocity
	

func _input(event):
	if mode == CameraMode.Fly:
		if (event is InputEventMouseMotion) && looking:
			look += event.relative

# I think this connects to the SceneTree notifs
# see: https://www.reddit.com/r/godot/comments/e36zyq/how_can_i_detect_when_the_mouse_cursor_is_inside/
func _notification(msg):
	match msg:
		NOTIFICATION_WM_MOUSE_EXIT:
			_is_mouse_in_window = false
		NOTIFICATION_WM_MOUSE_ENTER:
			_is_mouse_in_window = true
