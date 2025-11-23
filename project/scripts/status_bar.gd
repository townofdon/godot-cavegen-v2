class_name StatusBar
extends Panel

const ICONS_NSWE = "[img region=144,0,16,16]res://assets/icons.png[/img][img region=160,0,16,16]res://assets/icons.png[/img][img region=128,0,16,16]res://assets/icons.png[/img][img region=112,0,16,16]res://assets/icons.png[/img]"
const ICONS_LMOUSE = "[img region=176,0,16,16]res://assets/icons.png[/img]"

const HELPER_TEXT_MODE = "1=Select Room, 2=Config, 3=Tilemap Editor, 3=Preview"
const HELPER_TEXT_ROOM_SELECT = ICONS_NSWE + " Add / Select Room"
const HELPER_TEXT_ROOM_MOVE = ICONS_NSWE + " Move Room"
const HELPER_TEXT_LMOUSE_LOOK = ICONS_LMOUSE + " Look"
const HELPER_TEXT_TILE_MODE = "B - Brush		L - Line		R - Rect		G - Fill"
const HELPER_TEXT_TILE_ERASE = "ALT - Erase"
const HELPER_TEXT_WASD = "WASD - Move"

const TEXT_GROUP_SPACER := "				"
const LABELS_EMPTY: Array[Array] = [[" "]]
const LABELS_CHOOSE_MODE: Array[Array] = [[HELPER_TEXT_MODE]]
const LABELS_CHOOSE_TILE: Array[Array] = [[HELPER_TEXT_TILE_MODE, HELPER_TEXT_TILE_ERASE]]
const LABELS_SELECT_ROOM: Array[Array] = [[HELPER_TEXT_ROOM_SELECT]]
const LABELS_MOVE_ROOM: Array[Array] = [[HELPER_TEXT_ROOM_MOVE]]
const LABELS_PREVIEW_FLY: Array[Array] = [[HELPER_TEXT_WASD, HELPER_TEXT_LMOUSE_LOOK]]

@onready var rich_text_label_1: RichTextLabel = %RichTextLabel1
@onready var rich_text_label_2: RichTextLabel = %RichTextLabel2
@onready var rich_text_label_3: RichTextLabel = %RichTextLabel3

var mode:CaveGen.Mode
var room_select_mode:RoomSelectOverlay.Mode
# TODO: ADD VIEW MODE
var enabled:bool = true
var cmd_key_pressed:bool = false

func _ready() -> void:
	StatusBarNotifs.mode_changed.connect(_on_mode_changed)
	StatusBarNotifs.room_select_mode_changed.connect(_on_room_select_mode_changed)
	StatusBarNotifs.load_started.connect(_disable)
	StatusBarNotifs.load_finished.connect(_enable)
	StatusBarNotifs.export_started.connect(_disable)
	StatusBarNotifs.export_finished.connect(_enable)
	StatusBarNotifs.cmd_key_just_pressed.connect(_on_cmd_key_pressed.bind(true))
	StatusBarNotifs.cmd_key_released.connect(_on_cmd_key_pressed.bind(false))
	rich_text_label_1.horizontal_alignment = HORIZONTAL_ALIGNMENT_LEFT
	rich_text_label_2.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	rich_text_label_3.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT

func _on_mode_changed(p_mode:CaveGen.Mode) -> void:
	mode = p_mode
	_rerender()

func _on_room_select_mode_changed(p_mode:RoomSelectOverlay.Mode) -> void:
	room_select_mode = p_mode
	_rerender()

func _on_cmd_key_pressed(p_pressed:bool) -> void:
	cmd_key_pressed = p_pressed
	_rerender()

func _enable() -> void:
	enabled = true
	_rerender()

func _disable() -> void:
	enabled = false
	_rerender()

func _rerender() -> void:
	rich_text_label_1.hide()
	rich_text_label_2.hide()
	rich_text_label_3.hide()
	if !enabled:
		return
	if cmd_key_pressed:
		_render_labels(LABELS_CHOOSE_MODE)
	elif mode == CaveGen.Mode.RoomSelect:
		if room_select_mode == RoomSelectOverlay.Mode.Select:
			_render_labels(LABELS_SELECT_ROOM)
		elif room_select_mode == RoomSelectOverlay.Mode.Move:
			_render_labels(LABELS_MOVE_ROOM)
	elif mode == CaveGen.Mode.RoomConfig:
		_render_labels(LABELS_EMPTY)
	elif mode == CaveGen.Mode.TileEditor:
		_render_labels(LABELS_CHOOSE_TILE)
	elif mode == CaveGen.Mode.Preview:
		_render_labels(LABELS_PREVIEW_FLY)
	else:
		_render_labels(LABELS_EMPTY)

func _render_labels(text_groups: Array[Array]) -> void:
	var i:int = 1
	for group:Array[String] in text_groups:
		if !group || len(group) == 0: continue
		assert(group is Array)
		assert(i <= 3)
		if i == 1: _display_label(rich_text_label_1, group)
		if i == 2: _display_label(rich_text_label_2, group)
		if i == 3: _display_label(rich_text_label_3, group)
		i += 1

func _display_label(label: RichTextLabel, text_group: Array) -> void:
	assert(text_group.get(0) && text_group.get(0) is String)
	var text := TEXT_GROUP_SPACER.join(text_group)
	label.show()
	label.text = text
