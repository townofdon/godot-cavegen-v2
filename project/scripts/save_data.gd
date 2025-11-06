class_name SaveData
extends Resource

@export var cfg:GlobalConfig
@export var arr_room: Array[RoomConfig]
@export var arr_noise: Array[FastNoiseLite]
@export var arr_border_noise: Array[FastNoiseLite]

const SAVE_PATH := "user://save-data.tres"

static func get_default_data() -> SaveData:
	var data: SaveData = SaveData.new()
	var new_cfg := GlobalConfig.new()
	var new_room := RoomConfig.new()
	new_room.init_tiles(new_cfg.get_num_cells_2d())
	data.cfg = new_cfg
	data.arr_room = [new_room]
	data.arr_noise = [FastNoiseLite.new()]
	data.arr_border_noise = [FastNoiseLite.new()]
	return data

static func sanitize(data: SaveData) -> SaveData:
	var default_data := get_default_data()
	if !data: data = default_data
	if !data.cfg: data.cfg = default_data.cfg
	if len(data.arr_room) == 0: data.arr_room = default_data.arr_room
	if len(data.arr_noise) == 0: data.arr_noise = default_data.arr_noise
	if len(data.arr_border_noise) == 0: data.arr_border_noise = default_data.arr_border_noise
	assert(data.cfg)
	assert(len(data.arr_room) >= 1)
	assert(len(data.arr_noise) >= 1)
	assert(len(data.arr_border_noise) >= 1)
	var room:RoomConfig = data.arr_room.get(0)
	var num_cells:Vector2i = data.cfg.get_num_cells_2d()
	var expected_num_tiles := num_cells.x * num_cells.y
	if room.get_num_tiles() != expected_num_tiles:
		room.init_tiles(num_cells)
	assert(room.get_num_tiles() > 0)
	assert(room.get_num_tiles() == expected_num_tiles)
	return data

static func clone(data: SaveData) -> SaveData:
	return data.duplicate_deep(DEEP_DUPLICATE_ALL)

## save data to SAVE_PATH
static func save(data: SaveData) -> void:
	# save resource file as a convenient way to serialize the data.
	ResourceSaver.save(data, SAVE_PATH)

# export save data to user-chosen directory
static func download_file(path: String) -> void:
	_download_file(path)

# export save data to downloads directory
static func download_file_web(path: String) -> void:
	_download_file_web(path)

# save imported data to SAVE_PATH
static func import_from_file(path:String) -> bool:
	return _import_from_file(path)

# open browser native file picker - imported file will overwrite SAVE_PATH
static func import_from_file_web() -> bool:
	return await _import_from_file_web()

# note - import the save file before calling load_data()
static func load_data(default_data: SaveData) -> SaveData:
	var data: SaveData
	if ResourceLoader.exists(SAVE_PATH):
		data = ResourceLoader.load(SAVE_PATH, "SaveData", ResourceLoader.CACHE_MODE_IGNORE)
	else:
		data = default_data
	return sanitize(data)

static func _download_file(path:String) -> void:
	assert(!OS.has_feature("web"))
	if OS.has_feature("web"):
		return
	var rfile := FileAccess.open(ProjectSettings.globalize_path(SAVE_PATH), FileAccess.READ)
	if rfile == null:
		push_error("Failed to load file: %s" % path)
		return
	var wfile = FileAccess.open(path, FileAccess.WRITE)
	if wfile != null:
		wfile.store_buffer(rfile.get_buffer(rfile.get_length()))
		wfile.close()
		print("File saved to: " + path)
	else:
		print("Error saving file to: " + path)
	rfile.close()

static func _download_file_web(path:String) -> void:
	assert(OS.has_feature("web"))
	if !OS.has_feature("web"):
		return
	var file := FileAccess.open(path, FileAccess.READ)
	if file == null:
		push_error("Failed to load file %s" % path)
		return
	# Read the whole file to memory.
	var buffer := file.get_buffer(file.get_len())
	# Prompt the user to download the file (will have the same name as the input file).
	JavaScriptBridge.download_buffer(buffer, path.get_file())

static func _import_from_file(path:String) -> bool:
	assert(!OS.has_feature("web"))
	if OS.has_feature("web"):
		return false
	if ResourceLoader.exists(path):
		var data:SaveData = ResourceLoader.load(path, "SaveData", ResourceLoader.CACHE_MODE_IGNORE)
		ResourceSaver.save(data, SAVE_PATH)
		return true
	else:
		print("File does not exist: " + path)
		return false
	#var rfile := FileAccess.open(path, FileAccess.READ)
	#if rfile == null:
		#push_error("Failed to load file: %s" % path)
		#return false
	#var file_path := ProjectSettings.globalize_path(SAVE_PATH)
	#var wfile = FileAccess.open(file_path, FileAccess.WRITE)
	#if wfile != null:
		#wfile.store_buffer(rfile.get_buffer(rfile.get_len()))
		#wfile.close()
		#rfile.close()
		#print("File saved to: " + file_path)
		#return true
	#else:
		#print("Error importing file: " + path)
		#return false

# In browser contexts we do not have access to the filesystem directly.
# As a workaround, use the FileSystemAPI to load the file data into memory
# and manually copy it to the save file.
# Note that there is no protection against the save file getting corrupted as
# any `.tres` file is valid.
static func _import_from_file_web() -> bool:
	assert(OS.has_feature("web"))
	if !OS.has_feature("web"):
		return false
	var file_access_web: FileAccessWeb = FileAccessWeb.new()
	file_access_web.open(".tres")
	var result = await file_access_web.load_completed
	var status:int = result[0]
	var file_name: String = result[1]
	#var file_type: String = result[2]
	var base64_data: String = result[3]
	if status != FileAccessWeb.FileUploadStatus.Completed:
		return false
	var raw_data:= Marshalls.base64_to_utf8(base64_data)
	var wfile_path := ProjectSettings.globalize_path(SAVE_PATH)
	var wfile = FileAccess.open(wfile_path, FileAccess.WRITE)
	if wfile != null:
		wfile.store_string(raw_data)
		wfile.close()
		print("File saved to: " + wfile_path)
		return true
	else:
		print("Error importing file: " + file_name)
		return false
