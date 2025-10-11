@tool
class_name TestCube
extends CSGBox3D

@export var cfg:GlobalConfig

func _ready() -> void:
	pass

func _process(_delta: float) -> void:
	update_test_cube()

func update_test_cube():
	if !cfg: return
	size.x = cfg.room_width
	size.y = cfg.room_height * cfg.ceiling
	size.z = cfg.room_depth
	global_position = Vector3(size.x/2,size.y/2,size.z/2)
	if material && material is ShaderMaterial:
		var y_ceil := cfg.room_height * cfg.ceiling - cfg.active_plane_offset
		material.set_shader_parameter("y_ceil", y_ceil)
		material.set_shader_parameter("y_min", 0.0)
