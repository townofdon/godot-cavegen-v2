class_name SaveData
extends Resource

@export var cfg:GlobalConfig
@export var arr_room: Array[RoomConfig]
@export var arr_noise: Array[FastNoiseLite]
@export var arr_border_noise: Array[FastNoiseLite]

const SAVE_PATH := "user://save-data.tres"

static func get_default_data() -> SaveData:
	var data: SaveData = SaveData.new()
	data.cfg = GlobalConfig.new()
	data.arr_room = Array()
	data.arr_noise = Array()
	data.arr_border_noise = Array()
	data.arr_room.append(RoomConfig.new())
	data.arr_noise.append(FastNoiseLite.new())
	data.arr_border_noise.append(FastNoiseLite.new())
	return data

static func write_to_file(data: SaveData, file_name: String) -> void:
	# save resource file as a convenient way to serialize the data.
	ResourceSaver.save(data, SAVE_PATH)
	_download_file(ProjectSettings.globalize_path(SAVE_PATH), file_name)

static func import_from_file(path:String) -> bool:
	return _import_from_file(path)

# open browser native file picker - imported file will overwrite SAVE_PATH
static func import_from_file_web() -> bool:
	return await _import_from_file_web()

# note - import the save file before calling load_data()
static func load_data() -> SaveData:
	var data: SaveData
	if ResourceLoader.exists(SAVE_PATH):
		data = ResourceLoader.load(SAVE_PATH, "SaveData", ResourceLoader.CACHE_MODE_IGNORE)
	else:
		data = get_default_data()
	return data

static func _download_file(path:String, file_name: String) -> void:
	if OS.has_feature("web"):
		_download_file_web(path, file_name)
		return
	var rfile := FileAccess.open(path, FileAccess.READ)
	if rfile == null:
		push_error("Failed to load file: %s" % path)
		return
	var downloads_path = OS.get_system_dir(OS.SYSTEM_DIR_DOWNLOADS)
	var file_path = downloads_path.path_join(file_name)
	var wfile = FileAccess.open(downloads_path.path_join(file_name), FileAccess.WRITE)
	if wfile != null:
		wfile.store_buffer(rfile.get_buffer(rfile.get_len()))
		wfile.close()
		print("File saved to: " + file_path)
	else:
		print("Error saving file to: " + file_path)
	rfile.close()

static func _download_file_web(path:String, file_name: String) -> void:
	var file := FileAccess.open(path, FileAccess.READ)
	if file == null:
		push_error("Failed to load file %s" % path)
		return
	# Read the whole file to memory.
	var buffer := file.get_buffer(file.get_len())
	# Prompt the user to download the file (will have the same name as the input file).
	JavaScriptBridge.download_buffer(buffer, file_name)

static func _import_from_file(path:String) -> bool:
	assert(!OS.has_feature("web"))
	if OS.has_feature("web"):
		return false
	var rfile := FileAccess.open(path, FileAccess.READ)
	if rfile == null:
		push_error("Failed to load file: %s" % path)
		return false
	var file_path := ProjectSettings.globalize_path(SAVE_PATH)
	var wfile = FileAccess.open(file_path, FileAccess.WRITE)
	if wfile != null:
		wfile.store_buffer(rfile.get_buffer(rfile.get_len()))
		wfile.close()
		rfile.close()
		print("File saved to: " + file_path)
		return true
	else:
		print("Error importing file: " + path)
		return false

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
