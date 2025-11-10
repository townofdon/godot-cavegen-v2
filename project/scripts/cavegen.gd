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
@onready var dirty_timer:Timer = %DirtyTimer
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
	room_select_overlay.add_neighbor_requested.connect(_add_room)
	room_select_overlay.select_neighbor_requested.connect(_select_neighbor_room)
	room_select_overlay.move_room_requested.connect(_move_room)
	room_select_overlay.delete_room_requested.connect(_delete_room)
	# setup ui
	side_toolbar.initialize(self)
	export_dialog.initialize(self)
	# setup timers
	notif_timer.autostart = false
	notif_timer.one_shot = true
	notif_timer.timeout.connect(func()->void: regenerate(current_room_idx))
	notif_timer.stop()
	dirty_timer.autostart = false
	dirty_timer.one_shot = true
	dirty_timer.timeout.connect(_regen_dirty_rooms)
	dirty_timer.stop()
	# setup meshsaver
	OBJExporter.export_progress_updated.connect(_on_export_progress_updated)
	OBJExporter.export_completed.connect(_on_export_completed)
	# load data
	var data := SaveUtil.load_data(SaveUtil.clone(initial_save_data))
	_initialize(data)

func _initialize(data: CaveGenData) -> void:
	set_mode(Mode.RoomConfig)
	current_room_idx = 0
	save_data = data
	room_idx_lookup.clear()
	room_idx_lookup[_idx_key(Vector2i.ZERO)] = 0
	save_data.validate(0, room_idx_lookup)
	assert(!save_data.cfg.on_changed.is_connected(_on_global_config_changed))
	save_data.cfg.on_changed.connect(_on_global_config_changed)
	_clear_meshes()
	arr_meshgen.clear()
	arr_meshgen.append(root_meshgen)
	_setup_room()
	initializing = true
	await _setup_initial_meshes()
	initializing = false

func _setup_room() -> void:
	if initializing: return
	assert(save_data)
	save_data.validate(current_room_idx, room_idx_lookup)
	var cfg := save_data.cfg
	var room:RoomConfig = save_data.arr_room.get(current_room_idx)
	var noise:FastNoiseLite = save_data.arr_noise.get(current_room_idx)
	var border_noise:FastNoiseLite = save_data.arr_border_noise.get(current_room_idx)
	var meshgen:MeshGen = arr_meshgen.get(current_room_idx)
	assert(meshgen)
	# connect room signal
	assert(!room.on_changed.is_connected(_on_room_config_changed))
	assert(!room.dirtied.is_connected(_on_dirtied))
	room.on_changed.connect(_on_room_config_changed)
	room.dirtied.connect(_on_dirtied)
	# init UI, camera
	var coords := room.internal__grid_position
	var room_position := Vector3(
		(cfg.room_width - cfg.get_adjusted_cell_size()) * coords.x,
		0,
		(cfg.room_depth - cfg.get_adjusted_cell_size()) * coords.y)
	cameraman.initialize(self, cfg, room_position)
	file_menu.initialize(cfg)
	# init tilemap, roomconfig
	tilemap_editor.initialize(cfg, room)
	room_config_form.initialize(room, noise, border_noise)
	room_select_overlay.set_room(room)
	# init meshgen
	notif_timer.stop()
	regenerate(current_room_idx)
	dirty_timer.start()

func regenerate(idx: int) -> void:
	if Engine.is_editor_hint(): return
	assert(save_data)
	if !save_data.validate(idx, room_idx_lookup): return
	var cfg := save_data.cfg
	var room:RoomConfig = save_data.arr_room.get(idx)
	var noise:FastNoiseLite = save_data.arr_noise.get(idx)
	var border_noise:FastNoiseLite = save_data.arr_border_noise.get(idx)
	var meshgen:MeshGen = arr_meshgen.get(idx)
	assert(meshgen)
	meshgen.generate(cfg, room, noise, border_noise)
	room.set_dirty(false)
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

# iterate through dirty rooms in order to blend rooms
func _regen_dirty_rooms() -> void:
	await get_tree().process_frame
	if !dirty_timer.is_stopped: return
	if !notif_timer.is_stopped(): return
	if initializing: return
	var dirty_rooms:Array[RoomConfig] = []
	for room:RoomConfig in save_data.arr_room:
		if room.get_dirty():
			dirty_rooms.append(room)
	if len(dirty_rooms) == 0: return
	for i in range(4):
		for room:RoomConfig in dirty_rooms:
			if initializing: return
			if !dirty_timer.is_stopped: return
			if !notif_timer.is_stopped(): return
			regenerate(room.get_room_idx())
			await get_tree().process_frame
		regenerate(current_room_idx)
		await get_tree().process_frame

func _clear_meshes()->void:
	for i in range(1, len(arr_meshgen)):
		var meshgen:MeshGen = arr_meshgen.get(i)
		if meshgen: meshgen.queue_free()

func _clone_meshgen() -> MeshGen:
	var flags := 0
	flags = flags << DuplicateFlags.DUPLICATE_GROUPS
	flags = flags << DuplicateFlags.DUPLICATE_SCRIPTS
	flags = flags << DuplicateFlags.DUPLICATE_USE_INSTANTIATION
	return root_meshgen.duplicate(flags)

func _setup_initial_meshes() -> void:
	assert(save_data)
	_recalculate_room_mappings()
	arr_meshgen.resize(1)
	for idx in range(1, len(save_data.arr_room)):
		var new_meshgen := _clone_meshgen()
		new_meshgen.mesh = ArrayMesh.new()
		mesh_container.add_child(new_meshgen)
		arr_meshgen.append(new_meshgen)
		regenerate(idx)
		await get_tree().process_frame
	# regenerate all rooms so that neighbor blend is correct
	for idx in range(0, len(save_data.arr_room)):
		regenerate(idx)
		await get_tree().process_frame

func _add_room(dir: Vector2i, border_mask: int) -> void:
	assert(dir == Vector2i.UP || dir == Vector2i.DOWN || dir == Vector2i.LEFT || dir == Vector2i.RIGHT, "invalid dir: (%s,%s)" % [dir.x, dir.y])
	assert(save_data)
	if !save_data.validate(current_room_idx, room_idx_lookup): return
	var cfg := save_data.cfg
	var existing_room:RoomConfig = save_data.arr_room.get(current_room_idx)
	Utils.Conn.disconnect_all(existing_room.on_changed)
	Utils.Conn.disconnect_all(existing_room.dirtied)
	# create new room
	var new_idx := len(save_data.arr_room)
	var new_room := existing_room.duplicate(true)
	var coords:Vector2i = new_room.internal__grid_position + dir
	new_room.internal__grid_position = coords
	new_room.tilemap__tiles = new_room.tilemap__tiles.duplicate()
	new_room.init_tiles_for_new_room(cfg.get_num_cells_2d(), dir, border_mask)
	# TODO: move this to c++
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
		(cfg.room_width - cfg.get_adjusted_cell_size()) * dir.x,
		0,
		(cfg.room_depth - cfg.get_adjusted_cell_size()) * dir.y)
	# duplicate border noise
	var existing_border_noise:FastNoiseLite = save_data.arr_border_noise.get(current_room_idx)
	var new_border_noise := existing_border_noise.duplicate()
	# duplicate meshgen
	var new_meshgen := _clone_meshgen()
	new_meshgen.mesh = ArrayMesh.new()
	mesh_container.add_child(new_meshgen)
	# update state to reflect everything
	save_data.arr_room.append(new_room)
	save_data.arr_noise.append(new_noise)
	save_data.arr_border_noise.append(new_border_noise)
	arr_meshgen.append(new_meshgen)
	existing_room.set_dirty(true)
	new_room.set_dirty(true)
	_recalculate_room_mappings()
	new_room.set_dirty(true)
	save_data.validate(new_idx, room_idx_lookup)
	current_room_idx = new_idx
	_setup_room()

func _delete_room() -> void:
	if current_room_idx == 0: return
	assert(save_data)
	if !save_data.validate(current_room_idx, room_idx_lookup): return
	var room:RoomConfig = save_data.arr_room.get(current_room_idx)
	Utils.Conn.disconnect_all(room.on_changed)
	Utils.Conn.disconnect_all(room.dirtied)
	var node_up:RoomConfig = room.get_node_up()
	var node_down:RoomConfig = room.get_node_down()
	var node_left:RoomConfig = room.get_node_left()
	var node_right:RoomConfig = room.get_node_right()
	# remove mesh
	var meshgen:MeshGen = arr_meshgen.get(current_room_idx)
	if meshgen: meshgen.queue_free()
	# update save data
	save_data.arr_room.remove_at(current_room_idx)
	save_data.arr_noise.remove_at(current_room_idx)
	save_data.arr_border_noise.remove_at(current_room_idx)
	arr_meshgen.remove_at(current_room_idx)
	_recalculate_room_mappings()
	# update current room idx
	current_room_idx = 0
	for node:RoomConfig in [node_up, node_down, node_left, node_right]:
		if node && node.get_room_idx() > current_room_idx:
			current_room_idx = node.get_room_idx()
			node.set_dirty(true)
	save_data.validate(current_room_idx, room_idx_lookup)
	_setup_room()

func _move_room(dir: Vector2i) -> void:
	assert(dir == Vector2i.UP || dir == Vector2i.DOWN || dir == Vector2i.LEFT || dir == Vector2i.RIGHT, "invalid dir: (%s,%s)" % [dir.x, dir.y])
	assert(save_data)
	if !save_data.validate(current_room_idx, room_idx_lookup): return
	var room:RoomConfig = save_data.arr_room.get(current_room_idx)
	Utils.Conn.disconnect_all(room.on_changed)
	Utils.Conn.disconnect_all(room.dirtied)
	if dir == Vector2i.UP: assert(!room.get_node_up())
	if dir == Vector2i.DOWN: assert(!room.get_node_down())
	if dir == Vector2i.LEFT: assert(!room.get_node_left())
	if dir == Vector2i.RIGHT: assert(!room.get_node_right())
	room.internal__grid_position = room.internal__grid_position + dir
	room.set_dirty(true)
	_recalculate_room_mappings()
	room.set_dirty(true)
	_setup_room()

func _select_neighbor_room(dir: Vector2i) -> void:
	assert(dir == Vector2i.UP || dir == Vector2i.DOWN || dir == Vector2i.LEFT || dir == Vector2i.RIGHT, "invalid dir: (%s,%s)" % [dir.x, dir.y])
	assert(save_data)
	if !save_data.validate(current_room_idx, room_idx_lookup): return
	var existing_room:RoomConfig = save_data.arr_room.get(current_room_idx)
	Utils.Conn.disconnect_all(existing_room.on_changed)
	Utils.Conn.disconnect_all(existing_room.dirtied)
	var coords:Vector2i = existing_room.internal__grid_position + dir
	var new_idx:int = room_idx_lookup.get(_idx_key(coords), -1)
	var new_room:RoomConfig = save_data.arr_room.get(new_idx)
	assert(new_room, "no room found at new_coords: %s - old_coords=%s" % [_idx_key(coords), _idx_key(existing_room.internal__grid_position)])
	current_room_idx = new_idx
	_setup_room()

# this method takes 0.5-5ms.
func _recalculate_room_mappings() -> void:
	room_idx_lookup.clear()
	for idx in range(len(save_data.arr_room)):
		var room:RoomConfig = save_data.arr_room.get(idx)
		assert(room)
		var coords := room.internal__grid_position
		room.set_room_idx(idx)
		room_idx_lookup[_idx_key(coords)] = idx
	for idx in range(len(save_data.arr_room)):
		var room:RoomConfig = save_data.arr_room.get(idx)
		assert(room)
		var coords := room.internal__grid_position
		var idx_up:int = room_idx_lookup.get(_idx_key(coords + Vector2i.UP), -1)
		var idx_down:int = room_idx_lookup.get(_idx_key(coords + Vector2i.DOWN), -1)
		var idx_left:int = room_idx_lookup.get(_idx_key(coords + Vector2i.LEFT), -1)
		var idx_right:int = room_idx_lookup.get(_idx_key(coords + Vector2i.RIGHT), -1)
		room.set_node_up(save_data.arr_room.get(idx_up) if idx_up >= 0 else null)
		room.set_node_down(save_data.arr_room.get(idx_down) if idx_down >= 0 else null)
		room.set_node_left(save_data.arr_room.get(idx_left) if idx_left >= 0 else null)
		room.set_node_right(save_data.arr_room.get(idx_right) if idx_right >= 0 else null)
		room.set_noise_node_up(save_data.arr_noise.get(idx_up) if idx_up >= 0 else null)
		room.set_noise_node_down(save_data.arr_noise.get(idx_down) if idx_down >= 0 else null)
		room.set_noise_node_left(save_data.arr_noise.get(idx_left) if idx_left >= 0 else null)
		room.set_noise_node_right(save_data.arr_noise.get(idx_right) if idx_right >= 0 else null)
		if room.get_node_up(): room.get_node_up().set_node_down(room)
		if room.get_node_down(): room.get_node_down().set_node_up(room)
		if room.get_node_left(): room.get_node_left().set_node_right(room)
		if room.get_node_right(): room.get_node_right().set_node_left(room)
		save_data.validate(idx, room_idx_lookup)

func _idx_key(coords: Vector2i) -> String:
	return CaveGenData.room_idx_key(coords)

func _get_active_plane_y() -> float:
	assert(save_data)
	save_data.validate(current_room_idx, room_idx_lookup)
	var cfg := save_data.cfg
	var ceiling_y := cfg.room_height * cfg.ceiling
	var active_y := ceiling_y - cfg.active_plane_offset
	return maxf(active_y, 2)

func _on_global_config_changed() -> void:
	if initializing: return
	tilemap_editor.handle_room_size_change()
	if notif_timer.is_stopped(): notif_timer.start()

func _on_room_config_changed() -> void:
	if initializing: return
	if notif_timer.is_stopped(): notif_timer.start()

func _on_dirtied() -> void:
	if initializing: return
	dirty_timer.start()

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
