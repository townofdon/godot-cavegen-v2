class_name CaveGen
extends Node3D

@export var initial_save_data:CaveGenData = CaveGenData.new()

@export var toast_bg:Color = Color(0, 0, 0, 0.7)
@export var toast_bg_error:Color = Color(0, 0, 0, 0.7)
@export var toast_success:Color = Color(1,1,1,1)
@export var toast_error:Color = Color.ORANGE_RED

@onready var mesh_container: Node3D = %MeshContainer
@onready var root_meshgen:MeshGen = %MeshGen
@onready var notif_timer:Timer = %Timer
@onready var test_cube:CSGBox3D = %TestCube
@onready var tilemap_editor:TileMapEditor = %TileMapEditor
@onready var cameraman: CameraMan = %Cameraman

# UI
@onready var file_menu: FileMenu = %FileMenu
@onready var tile_editor_container: Control = %TileEditorContainer
@onready var tile_editor_tools: HBoxContainer = %TileEditorTools
@onready var room_select_overlay: RoomSelectOverlay = %RoomSelectOverlay
@onready var room_config_form: RoomConfigForm = %RoomConfigForm
@onready var side_toolbar: SideToolbar = %SideToolbar
@onready var export_dialog: ExportDialog = %ExportDialog
@onready var save_file_dialog: FileDialog = %SaveFileDialog
@onready var load_file_dialog: FileDialog = %LoadFileDialog

signal export_started
signal export_progress(pct: float)
signal export_completed(files: PackedStringArray)
signal mode_changed(mode: Mode)

var mode:Mode = Mode.RoomConfig
var save_data:CaveGenData
var arr_meshgen:Array[MeshGen]
var current_room_idx:int = 0
var room_idx_lookup: Dictionary[String, int] = {}
var initializing:bool = false

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
	room_config_form.hide()
	room_select_overlay.hide()
	if mode == Mode.TileEditor:
		tile_editor_container.show()
		tile_editor_tools.show()
	elif mode == Mode.RoomConfig:
		room_config_form.show()
	elif mode == Mode.RoomSelect:
		room_select_overlay.show()
	call_deferred("notify_mode_changed", mode)

func notify_mode_changed(p_mode: Mode) -> void:
	mode_changed.emit(p_mode)

func _on_new_file_pressed() -> void:
	if initializing: return
	_initialize(SaveUtil.clone(initial_save_data))

func _on_open_file_pressed() -> void:
	if initializing: return
	if OS.has_feature("web"):
		tilemap_editor.disable()
		var ok := await SaveUtil.import_from_file_web()
		if ok:
			_initialize(SaveUtil.load_data(SaveUtil.clone(save_data)))
			_notify_success("File loaded")
		tilemap_editor.enable()
	else:
		tilemap_editor.disable()
		load_file_dialog.access = FileDialog.ACCESS_FILESYSTEM
		load_file_dialog.current_dir = OS.get_system_dir(OS.SYSTEM_DIR_DOWNLOADS)
		load_file_dialog.show()

func _on_open_file(path: String) -> void:
	tilemap_editor.enable()
	if SaveUtil.import_from_file(path):
		_initialize(SaveUtil.load_data(SaveUtil.clone(save_data)))
		_notify_success("File loaded")
	else:
		_notify_error("Failed to load file")

func _on_save_file_pressed() -> void:
	if initializing: return
	SaveUtil.save(save_data)
	_notify_success("File saved")

func _on_save_file_as_pressed() -> void:
	if initializing: return
	assert(save_data)
	assert(save_data.cfg)
	var cfg := save_data.cfg
	if OS.has_feature("web"):
		SaveUtil.save(save_data)
		SaveUtil.download_file_web(cfg.level_name)
		_notify_success("File saved")
	else:
		tilemap_editor.disable()
		save_file_dialog.access = FileDialog.ACCESS_FILESYSTEM
		save_file_dialog.current_dir = OS.get_system_dir(OS.SYSTEM_DIR_DOWNLOADS)
		save_file_dialog.get_line_edit().text = cfg.level_name
		save_file_dialog.show()

func _on_save_file_as(path: String) -> void:
	tilemap_editor.enable()
	save_file_dialog.hide()
	SaveUtil.save(save_data)
	SaveUtil.download_file(path)
	_notify_success("File saved")

func _on_file_dialog_cancel() -> void:
	tilemap_editor.enable()

func _notify_success(text: String) -> void:
	ToastParty.show({
		"text": text,
		"bgcolor": toast_bg,
		"color": toast_success,
		"text_size": 18,
	})

func _notify_error(text: String) -> void:
	ToastParty.show({
		"text": text,
		"bgcolor": toast_bg_error,
		"color": toast_error,
		"text_size": 18,
	})

func _ready() -> void:
	test_cube.queue_free()
	set_mode(Mode.RoomConfig)
	# setup signals
	file_menu.new_file_pressed.connect(_on_new_file_pressed)
	file_menu.open_file_pressed.connect(_on_open_file_pressed)
	file_menu.save_file_pressed.connect(_on_save_file_pressed)
	file_menu.save_file_as_pressed.connect(_on_save_file_as_pressed)
	export_dialog.user_start_export.connect(func()->void: _export_mesh())
	export_dialog.dialog_opened.connect(func()->void: tilemap_editor.disable())
	export_dialog.dialog_closed.connect(func()->void: tilemap_editor.enable())
	save_file_dialog.file_selected.connect(_on_save_file_as)
	save_file_dialog.visibility_changed.connect(func()->void:save_file_dialog.get_line_edit().grab_focus())
	save_file_dialog.canceled.connect(_on_file_dialog_cancel)
	load_file_dialog.file_selected.connect(_on_open_file)
	load_file_dialog.canceled.connect(_on_file_dialog_cancel)
	# setup ui
	side_toolbar.initialize(self)
	export_dialog.initialize(self)
	# setup notification timer
	notif_timer.autostart = false
	notif_timer.one_shot = true
	notif_timer.timeout.connect(func()->void: regenerate(current_room_idx))
	notif_timer.stop()
	# setup meshsaver
	OBJExporter.export_progress_updated.connect(_on_export_progress_updated)
	OBJExporter.export_completed.connect(_on_export_completed)
	# load data
	var data := SaveUtil.load_data(SaveUtil.clone(initial_save_data))
	_initialize(data)

func _initialize(data: CaveGenData) -> void:
	save_data = data
	save_data.validate(0)
	arr_meshgen.clear()
	arr_meshgen.append(root_meshgen)
	current_room_idx = 0
	room_idx_lookup.clear()
	room_idx_lookup.set(_idx_key(Vector2.ZERO), 0)
	_setup_room()
	initializing = true
	await _setup_initial_meshes()
	initializing = false

func _setup_room() -> void:
	if initializing: return
	assert(save_data)
	save_data.validate(current_room_idx)
	var cfg := save_data.cfg
	var room:RoomConfig = save_data.arr_room.get(current_room_idx)
	var noise:FastNoiseLite = save_data.arr_noise.get(current_room_idx)
	var border_noise:FastNoiseLite = save_data.arr_noise.get(current_room_idx)
	var meshgen:MeshGen = arr_meshgen.get(current_room_idx)
	assert(meshgen)
	# connect cfg, room signals
	assert(!cfg.on_changed.is_connected(_notify_change))
	assert(!room.on_changed.is_connected(_notify_change))
	cfg.on_changed.connect(_notify_change)
	room.on_changed.connect(_notify_change)
	# init UI, camera
	cameraman.initialize(self, cfg, meshgen.global_position)
	file_menu.initialize(cfg)
	# init tilemap, roomconfig
	tilemap_editor.initialize(cfg, room)
	room_config_form.initialize(room, noise, border_noise)
	room_select_overlay.set_room(room)
	# init meshgen
	notif_timer.stop()
	regenerate(current_room_idx)

func regenerate(idx: int) -> void:
	if Engine.is_editor_hint(): return
	assert(save_data)
	if !save_data.validate(idx): return
	var cfg := save_data.cfg
	var room:RoomConfig = save_data.arr_room.get(idx)
	var noise:FastNoiseLite = save_data.arr_noise.get(idx)
	var border_noise:FastNoiseLite = save_data.arr_noise.get(idx)
	var meshgen:MeshGen = arr_meshgen.get(idx)
	assert(meshgen)
	meshgen.generate(cfg, room, noise, border_noise)
	assert(meshgen.mesh is ArrayMesh)
	# apply mesh material
	if meshgen.mesh is ArrayMesh:
		var mat:ShaderMaterial = meshgen.material_override
		if mat && mat is ShaderMaterial:
			var offset := -_get_active_plane_y() * int(cfg.move_active_plane_to_origin)
			var max_y := cfg.room_height * cfg.ceiling - cfg.active_plane_offset
			mat.set_shader_parameter("y_ceil", max_y + offset)
			mat.set_shader_parameter("y_min", 0.0 + offset)
		# OBJExporter does not support shader material
		#meshgen.mesh.surface_set_material(0, meshgen.material_override)

func _setup_initial_meshes() -> void:
	assert(save_data)
	save_data.validate(0)
	var cfg := save_data.cfg
	for idx in range(1, len(save_data.arr_room)):
		var room:RoomConfig = save_data.arr_room.get(idx)
		var coords := room.internal__grid_position
		var new_meshgen := root_meshgen.duplicate()
		new_meshgen.global_position = Vector3(
			cfg.room_width * coords.x,
			0,
			cfg.room_depth * coords.y)
		arr_meshgen.append(new_meshgen)
		mesh_container.add_child(new_meshgen)
		regenerate(idx)
		await get_tree().process_frame

func _add_room(dir: Vector2i) -> void:
	assert(dir == Vector2i.UP || dir == Vector2i.DOWN || dir == Vector2i.LEFT || dir == Vector2i.RIGHT, "invalid dir: (%s,%s)" % [dir.x, dir.y])
	assert(save_data)
	if !save_data.validate(current_room_idx): return
	var cfg := save_data.cfg
	var existing_room:RoomConfig = save_data.arr_room.get(current_room_idx)
	# create new room
	var new_room := existing_room.duplicate()
	var coords:Vector2i = new_room.internal__grid_position + dir
	new_room.internal__grid_position = coords
	var idx_up:int = room_idx_lookup.get(_idx_key(coords + Vector2i.UP), -1)
	var idx_down:int = room_idx_lookup.get(_idx_key(coords + Vector2i.DOWN), -1)
	var idx_left:int = room_idx_lookup.get(_idx_key(coords + Vector2i.LEFT), -1)
	var idx_right:int = room_idx_lookup.get(_idx_key(coords + Vector2i.RIGHT), -1)
	new_room.internal__node_up = save_data.arr_room.get(idx_up)
	new_room.internal__node_down = save_data.arr_room.get(idx_down)
	new_room.internal__node_left = save_data.arr_room.get(idx_left)
	new_room.internal__node_right = save_data.arr_room.get(idx_right)
	## set precedence based on neighbors - see if idx solution works first
	#new_room.internal__precedence = new_room.internal__precedence + 1
	#for node:RoomConfig in [node_up, node_down, node_left, node_right]:
		#if node && node.internal__precedence >= new_room.internal__precedence:
			#new_room.internal__precedence = node.internal__precedence + 1
	# set precedence based on idx
	var new_idx := len(save_data.arr_room)
	new_room.internal__precedence = new_idx
	# set new offset, tilt based on dir
	if absi(dir.x) == 1:
		if dir.x == Vector2i.RIGHT.x && new_room.room_noise__tilt_x > 1:
			new_room.room_noise__offset_y = (1 - (new_room.room_noise__tilt_x - 1)) * 0.5
		elif dir.x == Vector2i.LEFT.x && new_room.room_noise__tilt_x < 1:
			new_room.room_noise__offset_y = new_room.room_noise__tilt_x * 0.5
		else:
			new_room.room_noise__offset_y = 0.5
		new_room.room_noise__tilt_x = 1
	else:
		if dir.y == Vector2i.UP.y && new_room.room_noise__tilt_z > 1:
			new_room.room_noise__offset_y = (1 - (new_room.room_noise__tilt_z - 1)) * 0.5
		elif dir.y == Vector2i.DOWN.y && new_room.room_noise__tilt_z < 1:
			new_room.room_noise__offset_y = new_room.room_noise__tilt_z * 0.5
		else:
			new_room.room_noise__offset_y = 0.5
		new_room.room_noise__tilt_z = 1
	# duplicate noise
	var existing_noise:FastNoiseLite = save_data.arr_noise.get(current_room_idx)
	var new_noise := existing_noise.duplicate()
	new_noise.offset = new_noise.offset + Vector3(
		cfg.room_width * dir.x,
		0,
		cfg.room_depth * dir.y)
	# duplicate border noise
	var existing_border_noise:FastNoiseLite = save_data.arr_noise.get(current_room_idx)
	var new_border_noise := existing_border_noise.duplicate()
	# duplicate meshgen
	var existing_meshgen:MeshGen = arr_meshgen.get(current_room_idx)
	var new_meshgen := existing_meshgen.duplicate()
	new_meshgen.global_position = Vector3(
			cfg.room_width * coords.x,
			0,
			cfg.room_depth * coords.y)
	mesh_container.add_child(new_meshgen)
	# update state to reflect everything
	save_data.arr_room.append(new_room)
	save_data.arr_noise.append(new_noise)
	save_data.arr_border_noise.append(new_border_noise)
	arr_meshgen.append(new_meshgen)
	save_data.validate(new_idx)
	current_room_idx = new_idx
	room_idx_lookup.set(_idx_key(coords), new_idx)
	_setup_room()

func _idx_key(coords: Vector2i) -> String:
	return "%s_%s" % [coords.x, coords.y]

func _get_active_plane_y() -> float:
	assert(save_data)
	save_data.validate(current_room_idx)
	var cfg := save_data.cfg
	var ceiling_y := cfg.room_height * cfg.ceiling
	var active_y := ceiling_y - cfg.active_plane_offset
	return maxf(active_y, 2)

# TODO: REMOVE
#func _process(_delta: float) -> void:
	#if _did_noise_change(noise, noise_b):
		#_notify_change()
		#noise_b = noise.duplicate()
	#if _did_noise_change(border_noise, border_noise_b):
		#_notify_change()
		#border_noise_b = border_noise.duplicate()

func _notify_change() -> void:
	if initializing: return
	tilemap_editor.handle_room_size_change()
	if notif_timer.is_stopped(): notif_timer.start()

var time_export_started:float = 0
var exporting:bool = false
func _export_mesh() -> void:
	# TODO: save meshes
	# - combine all meshes
	# - add each as a separate surface
	# - if > 256 surfaces, start a new mesh
	# save all newly-created meshes
	pass
	#if exporting:
		#return
	#assert(meshgen)
	#assert(meshgen.mesh)
	#var mesh := meshgen.mesh
#
	#var dir := DirAccess.open("user://")
	#if !dir.dir_exists("model"):
		#dir.make_dir("model")
	#export_started.emit()
	#export_progress.emit(0)
	#time_export_started = Time.get_unix_time_from_system();
	#OBJExporter.save_mesh_to_files(mesh, "user://model/", "cave")
	#exporting = true

func _on_export_progress_updated(_surf_idx:int, progress_value:float) -> void:
	export_progress.emit(progress_value)

func _on_export_completed(object_file: String, material_file: String) -> void:
	var time_export_ended := Time.get_unix_time_from_system()
	var duration := time_export_ended - time_export_started
	print("export complete. files:\n- %s\n- %s" % [
		ProjectSettings.globalize_path(object_file),
		ProjectSettings.globalize_path(material_file),
	])
	print ("took ", duration, "s")
	exporting = false
	var files := PackedStringArray()
	files.append(object_file)
	export_completed.emit(files)
