extends Node3D
class_name CaveGen

@export var cfg:GlobalConfig
@export var room:RoomConfig
@export var noise:FastNoiseLite
@export var borderNoise:FastNoiseLite

@onready var meshGen:MeshGen = %MeshGen
@onready var notifTimer:Timer = %Timer
@onready var testCube:CSGBox3D = %TestCube
@onready var tilemapEditor:TileMapEditor = %TileMapEditor

var noiseB:FastNoiseLite
var borderNoiseB:FastNoiseLite

func _ready() -> void:
	testCube.queue_free()
	assert(cfg)
	assert(noise)
	assert(borderNoise)
	assert(meshGen)
	cfg.on_changed.connect(_notify_change)
	room.on_changed.connect(_notify_change)

	# setup notification timer
	notifTimer.autostart = false
	notifTimer.one_shot = true
	notifTimer.timeout.connect(regenerate)
	notifTimer.stop()

	# setup tilemap
	tilemapEditor.initialize(cfg, room)

	# setup meshgen
	regenerate()

	#setup noise
	noiseB = noise.duplicate()
	borderNoiseB = borderNoise.duplicate()
	
	# setup meshsaver
	OBJExporter.export_progress_updated.connect(_on_export_progress_updated)
	OBJExporter.export_completed.connect(_on_export_completed)

func regenerate():
	if Engine.is_editor_hint(): return
	if !meshGen: return
	if !cfg: return
	if !room: return
	if !noise: return
	if !borderNoise: return
	meshGen.generate(cfg, room, noise, borderNoise)

	if meshGen.mesh is ArrayMesh:
		var mat:ShaderMaterial = meshGen.material_override
		if mat && mat is ShaderMaterial:
			var y_ceil := cfg.room_height * cfg.ceiling - cfg.active_plane_offset
			mat.set_shader_parameter("y_ceil", y_ceil)
			mat.set_shader_parameter("y_min", 0.0)
		# OBJExporter does not support shader material
		#meshGen.mesh.surface_set_material(0, meshGen.material_override)

func _process(_delta: float) -> void:
	if _did_noise_change(noise, noiseB):
		_notify_change()
		noiseB = noise.duplicate()
	if _did_noise_change(borderNoise, borderNoiseB):
		_notify_change()
		borderNoiseB = borderNoise.duplicate()

func _notify_change():
	tilemapEditor.handle_room_size_change()
	if notifTimer.is_stopped(): notifTimer.start()

func _did_noise_change(a: FastNoiseLite, b: FastNoiseLite) -> bool:
	if !noise || !noiseB:
		return false
	if a.frequency != b.frequency: return true
	if a.fractal_octaves != b.fractal_octaves: return true
	if a.noise_type != b.noise_type: return true
	if a.seed != b.seed: return true
	if a.offset != b.offset: return true
	if a.fractal_type != b.fractal_type: return true
	if a.fractal_octaves != b.fractal_octaves: return true
	if a.fractal_lacunarity != b.fractal_lacunarity: return true
	if a.fractal_gain != b.fractal_gain: return true
	if a.fractal_weighted_strength != b.fractal_weighted_strength: return true
	if a.domain_warp_enabled != b.domain_warp_enabled: return true
	if a.domain_warp_type != b.domain_warp_type: return true
	if a.domain_warp_fractal_type != b.domain_warp_fractal_type: return true
	if a.domain_warp_amplitude != b.domain_warp_amplitude: return true
	if a.domain_warp_frequency != b.domain_warp_frequency: return true
	if a.domain_warp_fractal_octaves != b.domain_warp_fractal_octaves: return true
	if a.domain_warp_fractal_lacunarity != b.domain_warp_fractal_lacunarity: return true
	if a.domain_warp_fractal_gain != b.domain_warp_fractal_gain: return true
	if a.cellular_distance_function != b.cellular_distance_function: return true
	if a.cellular_jitter != b.cellular_jitter: return true
	if a.cellular_return_type != b.cellular_return_type: return true
	return false

var timeStarted:float = 0
var saving:bool = false
func _save_mesh() -> void:
	if saving:
		return
	if !meshGen:
		printerr("[TestSaver] MeshGen is null")
		return
	if !meshGen.mesh:
		printerr("[TestSaver] mesh is null")
		return
	var mesh := meshGen.mesh
	
	var dir = DirAccess.open("user://")
	if !dir.dir_exists("model"):
		dir.make_dir("model")
	print("saving mesh... 0% completed")
	timeStarted = Time.get_unix_time_from_system();
	OBJExporter.save_mesh_to_files(mesh, "user://model/", "cave")
	saving = true

func _on_button_pressed() -> void:
	_save_mesh()

func _on_export_progress_updated(_surf_idx:int, progress_value:float):
	print("saving mesh... ", progress_value * 100, "% completed")

func _on_export_completed(object_file, material_file):
	print("save complete. files=", object_file, "," , material_file)
	var timeEnded := Time.get_unix_time_from_system()
	var duration := timeEnded - timeStarted
	print ("took ", duration, "s")
	saving = false
