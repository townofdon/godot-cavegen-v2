class_name CaveGen
extends Node3D

const SAVE_PATH := "user://save-data.tres"

#@export var cfg:GlobalConfig
#@export var arr_room: Array[RoomConfig]
#@export var arr_noise: Array[FastNoiseLite]
#@export var arr_border_noise: Array[FastNoiseLite]
@export var default_save_data:SaveData

@onready var mesh_container: Node3D = %MeshContainer
@onready var meshgen:MeshGen = %MeshGen
@onready var notif_timer:Timer = %Timer
@onready var test_cube:CSGBox3D = %TestCube
@onready var tilemap_editor:TileMapEditor = %TileMapEditor
@onready var cameraman: CameraMan = %Cameraman

# UI
@onready var file_menu: FileMenu = %FileMenu
@onready var tile_editor_container: Control = %TileEditorContainer
@onready var tile_editor_tools: HBoxContainer = %TileEditorTools
@onready var side_toolbar: SideToolbar = %SideToolbar
@onready var export_dialog: ExportDialog = %ExportDialog

signal export_started
signal export_progress(pct: float)
signal export_completed(files: PackedStringArray)
signal mode_changed(mode: Mode)

var save_data:SaveData
var mode:Mode = Mode.RoomConfig
var cfg:GlobalConfig
var room:RoomConfig
var noise:FastNoiseLite
var border_noise:FastNoiseLite
var noise_b:FastNoiseLite
var border_noise_b:FastNoiseLite

enum Mode {
	RoomSelect,
	RoomConfig,
	TileEditor,
	Preview,
}

func set_mode(p_mode: Mode) -> void:
	mode = p_mode
	tile_editor_container.hide()
	tile_editor_tools.hide()
	if mode == Mode.TileEditor:
		tile_editor_container.show()
		tile_editor_tools.show()
	call_deferred("notify_mode_changed", mode)

func notify_mode_changed(p_mode: Mode) -> void:
	mode_changed.emit(p_mode)

func _set_cfg(p_cfg: GlobalConfig) -> void:
	if cfg && cfg.on_changed.is_connected(_notify_change):
		cfg.on_changed.disconnect(_notify_change)
	cfg = p_cfg
	cfg.on_changed.connect(_notify_change)

func _set_room(p_room: RoomConfig) -> void:
	if room && room.on_changed.is_connected(_notify_change):
		room.on_changed.disconnect(_notify_change)
	room = p_room
	room.on_changed.connect(_notify_change)

func _on_save_data_loaded(data: SaveData) -> void:
	_initialize(data)

func _ready() -> void:
	test_cube.queue_free()
	_initialize(default_save_data)
	# setup meshsaver
	OBJExporter.export_progress_updated.connect(_on_export_progress_updated)
	OBJExporter.export_completed.connect(_on_export_completed)

func _initialize(data: SaveData) -> void:
	save_data = data
	assert(save_data)
	assert(len(save_data.arr_room) >= 1)
	assert(len(save_data.arr_noise) >= 1)
	assert(len(save_data.arr_border_noise) >= 1)
	assert(len(save_data.arr_room) == len(save_data.arr_noise))
	assert(len(save_data.arr_noise) == len(save_data.arr_border_noise))

	_set_cfg(save_data.cfg)
	_set_room(save_data.arr_room.get(0))
	noise = save_data.arr_noise.get(0)
	border_noise = save_data.arr_noise.get(0)
	assert(cfg)
	assert(room)
	assert(noise)
	assert(border_noise)
	assert(meshgen)

	# setup UI, camera
	cameraman.initialize(self, cfg, meshgen.position)
	file_menu.initialize(cfg)
	side_toolbar.initialize(self)
	export_dialog.initialize(self)
	export_dialog.user_start_export.connect(func(): _save_mesh())
	export_dialog.dialog_opened.connect(func(): tilemap_editor.disable())
	export_dialog.dialog_closed.connect(func(): tilemap_editor.enable())

	# setup notification timer
	notif_timer.autostart = false
	notif_timer.one_shot = true
	notif_timer.timeout.connect(regenerate)
	notif_timer.stop()

	# setup tilemap
	tilemap_editor.initialize(cfg, room)
	set_mode(Mode.RoomConfig)

	# setup meshgen
	regenerate()

	#setup noise
	noise_b = noise.duplicate()
	border_noise_b = border_noise.duplicate()

func regenerate() -> void:
	if Engine.is_editor_hint(): return
	if !meshgen: return
	if !cfg: return
	if !room: return
	if !noise: return
	if !border_noise: return
	meshgen.generate(cfg, room, noise, border_noise)

	if meshgen.mesh is ArrayMesh:
		var mat:ShaderMaterial = meshgen.material_override
		if mat && mat is ShaderMaterial:
			var offset := -_get_active_plane_y() * int(cfg.move_active_plane_to_origin)
			var max_y := cfg.room_height * cfg.ceiling - cfg.active_plane_offset
			mat.set_shader_parameter("y_ceil", max_y + offset)
			mat.set_shader_parameter("y_min", 0.0 + offset)
		# OBJExporter does not support shader material
		#meshgen.mesh.surface_set_material(0, meshgen.material_override)

func _get_active_plane_y() -> float:
	var ceiling_y := cfg.room_height * cfg.ceiling
	var active_y := ceiling_y - cfg.active_plane_offset
	return maxf(active_y, 2)

func _process(_delta: float) -> void:
	if _did_noise_change(noise, noise_b):
		_notify_change()
		noise_b = noise.duplicate()
	if _did_noise_change(border_noise, border_noise_b):
		_notify_change()
		border_noise_b = border_noise.duplicate()

func _notify_change():
	tilemap_editor.handle_room_size_change()
	if notif_timer.is_stopped(): notif_timer.start()

func _did_noise_change(a: FastNoiseLite, b: FastNoiseLite) -> bool:
	if !noise || !noise_b:
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
	if !meshgen:
		printerr("[TestSaver] MeshGen is null")
		return
	if !meshgen.mesh:
		printerr("[TestSaver] mesh is null")
		return
	var mesh := meshgen.mesh
	
	var dir = DirAccess.open("user://")
	if !dir.dir_exists("model"):
		dir.make_dir("model")
	export_started.emit()
	export_progress.emit(0)
	timeStarted = Time.get_unix_time_from_system();
	OBJExporter.save_mesh_to_files(mesh, "user://model/", "cave")
	saving = true

func _on_export_progress_updated(_surf_idx:int, progress_value:float):
	export_progress.emit(progress_value)

func _on_export_completed(object_file, material_file):
	var timeEnded := Time.get_unix_time_from_system()
	var duration := timeEnded - timeStarted
	print("save complete. files=", object_file, "," , material_file)
	print ("took ", duration, "s")
	saving = false
	var files = PackedStringArray()
	files.append(object_file)
	export_completed.emit(files)
