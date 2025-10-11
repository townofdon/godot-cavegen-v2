extends MeshInstance3D

@export var offset:Vector2 = Vector2(2, 10)
@export var cfg: GlobalConfig

#@onready var viewport: SubViewport = %TilemapSubViewport

var prevOffset:Vector2

func _ready() -> void:
	update_positioning()
	cfg.on_changed.connect(update_positioning)
	#viewport.set_clear_mode(SubViewport.CLEAR_MODE_ONCE)
	if !mesh:
		printerr("mesh not set in " + name)
		return
	var mat: ShaderMaterial
	if mesh is PlaneMesh && mesh.material is ShaderMaterial:
		mat = mesh.material
	if !mat:
		printerr("material not set in " + name + ".mesh")
		return
	else:
		#mat.set_shader_parameter("TEXTURE", viewport.get_texture())
		pass

func _process(_delta: float) -> void:
	if !is_equal_approx(offset.x, prevOffset.x) || !is_equal_approx(offset.y, prevOffset.y):
		update_positioning()
	prevOffset = offset

func update_positioning() -> void:
	if mesh is PlaneMesh:
		mesh.size = Vector2(cfg.room_width, cfg.room_depth)
		mesh.center_offset = Vector3(cfg.room_width / 2, 0, cfg.room_depth / 2)
		position = Vector3(cfg.room_width + offset.x, cfg.room_height * cfg.ceiling + offset.y, 0)
