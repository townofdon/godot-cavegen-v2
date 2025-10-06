extends Camera3D

@export var acceleration := 13.0
@export var moveSpeed := 5.0
@export var lookSpeed := 5.0

var _is_mouse_in_window := false
var move:Vector3 = Vector3.ZERO
var look:Vector2 = Vector2.ZERO
var velocity:Vector3 = Vector3.ZERO
var looking:bool = false

func _ready() -> void:
	# Note to future Don - x and y are swapped, because humans associate x with horizontal, and y with vertical.
	# Thinking in terms of rotation, rotating around the x axis would produce "vertical" rotation, and rotating about the y axis produces "horizontal".
	look.x = transform.basis.get_euler().y / -0.001
	look.y = transform.basis.get_euler().x / -0.001

func _physics_process(delta: float) -> void:
	if (Input.is_action_pressed("looking")):
		Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
		looking = true
	else:
		Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		looking = false

	# Mouse movement.
	if looking:
		look.y = clamp(look.y, -1560, 1560)
		transform.basis = Basis.from_euler(Vector3(look.y * -0.001, look.x * -0.001, 0))

	# Keyboard input
	var inputH := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
	var inputV := 0.0
	if Input.is_action_pressed("move_down"):
		inputV -= 1.0
	if Input.is_action_pressed("move_up"):
		inputV += 1.0
	var movement := transform.basis * (Vector3(inputH.x, inputV, inputH.y))
	movement = Vector3(movement.x, movement.y, movement.z) * moveSpeed * 0.1
	velocity = velocity.lerp(movement, 1 - exp(-acceleration * delta))
	global_position = global_position + velocity


func _input(event):
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
