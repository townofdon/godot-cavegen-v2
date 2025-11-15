class_name RoomConfigForm
extends Control

@onready var toggle_show_noise: BoolField = %ToggleShowNoise
@onready var toggle_show_border: BoolField = %ToggleShowBorder
@onready var toggle_show_floor: BoolField = %ToggleShowFloor
@onready var toggle_show_outer_walls: BoolField = %ToggleShowOuterWalls

@onready var toggle_normalize: BoolField = %ToggleNormalize
@onready var field_iso_value: FloatField = %FloatIsoValue
@onready var float_noise_floor: FloatField = %FloatNoiseFloor
@onready var float_noise_ceil: FloatField = %FloatNoiseCeil
@onready var float_curve: FloatField = %FloatCurve
@onready var float_bass_boost: FloatField = %FloatBassBoost
@onready var float_tilt_y: FloatField = %FloatTiltY
@onready var float_tilt_x: FloatField = %FloatTiltX
@onready var float_tilt_z: FloatField = %FloatTiltZ
@onready var float_offset_y: FloatField = %FloatOffsetY
@onready var float_smooth_crystalize: FloatField = %FloatSmoothCrystalize
@onready var float_falloff_above_ceiling: FloatField = %FloatFalloffAboveCeiling
@onready var float_falloff_near_border: FloatField = %FloatFalloffNearBorder
@onready var float_interpolate: FloatField = %FloatInterpolate
@onready var float_active_y_smoothing: FloatField = %FloatActiveYSmoothing
@onready var float_floor_level: FloatField = %FloatFloorLevel
@onready var float_rm_overhangs_blend: FloatField = %FloatRmOverhangsBlend
@onready var float_rm_overhangs_slope: FloatField = %FloatRmOverhangsSlope
@onready var toggle_remove_orphans: BoolField = %ToggleRemoveOrphans
@onready var float_orphan_threshold: FloatField = %FloatOrphanThreshold

@onready var toggle_use_border_noise: BoolField = %ToggleUseBorderNoise
@onready var toggle_normalize_border: BoolField = %ToggleNormalizeBorder
@onready var int_border_size: IntField = $"Panel/MarginContainer/TabContainer/Room Config/VBoxContainer/IntBorderSize"
@onready var float_border_noise_strength: FloatField = %FloatBorderNoiseStrength
@onready var float_smooth_border_noise: FloatField = %FloatSmoothBorderNoise
@onready var float_border_tilt: FloatField = %FloatBorderTilt
@onready var border_noise_options: VBoxContainer = %BorderNoiseOptions

@onready var float_tile_strength: FloatField = %FloatTileStrength
@onready var float_tile_smoothing: FloatField = %FloatTileSmoothing
@onready var float_tile_ceiling: FloatField = %FloatTileCeiling
@onready var float_tile_ceil_blend: FloatField = %FloatTileCeilBlend
@onready var float_tile_floor: FloatField = %FloatTileFloor
@onready var float_tile_floor_blend: FloatField = %FloatTileFloorBlend
@onready var float_tile_erase_size: FloatField = %FloatTileEraseSize

@onready var float_neighbor_blend_up: FloatField = %FloatNeighborBlendUp
@onready var float_neighbor_blend_down: FloatField = %FloatNeighborBlendDown
@onready var float_neighbor_blend_left: FloatField = %FloatNeighborBlendLeft
@onready var float_neighbor_blend_right: FloatField = %FloatNeighborBlendRight

@onready var tab_container: TabContainer = %TabContainer
@onready var noise_preview_window: MovableWindow = $NoisePreviewWindow
@onready var noise_preview: TextureRect = $NoisePreviewWindow/MarginContainer/AspectRatioContainer/NoisePreview
@onready var base_noise_form: EditNoiseForm = %BaseNoiseForm
@onready var edit_noise_form: EditNoiseForm = %EditNoiseForm

var default_room: RoomConfig
var show_preview: bool = false

enum Tab {
	ROOM_CONFIG = 0,
	BASE_NOISE = 1,
	BORDER_NOISE = 2,
}

func _ready() -> void:
	default_room = RoomConfig.new()
	default_room.reset_state()
	noise_preview_window.hide()
	tab_container.tab_selected.connect(_on_tab_selected)
	_on_tab_selected(tab_container.current_tab)

func _on_tab_selected(tab: int) -> void:
	_rerender_noise_preview(tab == Tab.BASE_NOISE || tab == Tab.BORDER_NOISE)

func initialize(room: RoomConfig, noise: FastNoiseLite, border_noise: FastNoiseLite) -> void:
	call_deferred("_initialize", room, noise, border_noise)

func _initialize(room: RoomConfig, noise: FastNoiseLite, border_noise: FastNoiseLite) -> void:
	_setup_room_bool(toggle_show_noise, room, "debug__show_noise")
	_setup_room_bool(toggle_show_border, room, "debug__show_border")
	_setup_room_bool(toggle_show_floor, room, "debug__show_floor")
	_setup_room_bool(toggle_show_outer_walls, room, "debug__show_outer_walls")

	_setup_room_bool(toggle_normalize, room, "room_noise__normalize")
	_setup_room_float(field_iso_value, room, "room_noise__iso_value", 0, 1, 0.001)
	_setup_room_float(float_noise_floor, room, "room_noise__noise_floor", 0, 1, 0.001)
	_setup_room_float(float_noise_ceil, room, "room_noise__noise_ceil", 0, 1, 0.001)
	_setup_room_float(float_curve, room, "room_noise__curve", 0, 2, 0.001)
	_setup_room_float(float_bass_boost, room, "room_noise__bass_boost", 0, 1, 0.001)
	_setup_room_float(float_tilt_y, room, "room_noise__tilt_y", 0, 2, 0.001)
	_setup_room_float(float_tilt_x, room, "room_noise__tilt_x", 0, 2, 0.001)
	_setup_room_float(float_tilt_z, room, "room_noise__tilt_z", 0, 2, 0.001)
	_setup_room_float(float_offset_y, room, "room_noise__offset_y", 0, 1, 0.001)

	_setup_room_float(float_smooth_crystalize, room, "room_noise__smooth_crystalize", -10, 10, 0.01)
	_setup_room_float(float_falloff_above_ceiling, room, "room_noise__falloff_above_ceiling", 0, 1, 0.01)
	_setup_room_float(float_falloff_near_border, room, "room_noise__falloff_near_border", 0, 2, 0.01)
	_setup_room_float(float_interpolate, room, "room_noise__interpolate", 0, 1, 0.01)
	_setup_room_float(float_active_y_smoothing, room, "room_noise__active_y_smoothing", 0, 1, 0.01)
	_setup_room_float(float_floor_level, room, "room_noise__floor_level", 0, 1, 0.01)
	_setup_room_float(float_rm_overhangs_blend, room, "room_noise__remove_overhangs_blend", 0, 1, 0.01)
	_setup_room_float(float_rm_overhangs_slope, room, "room_noise__remove_overhangs_slope", 0, 1, 0.01)
	_setup_room_bool(toggle_remove_orphans, room, "room_noise__remove_orphans")
	_setup_room_float(float_orphan_threshold, room, "room_noise__orphan_threshold", 0, 1, 0.01)

	_setup_room_bool(toggle_use_border_noise, room, "room_border__use_border_noise")
	_setup_room_bool(toggle_normalize_border, room, "room_border__normalize_border_noise")
	_setup_room_int(int_border_size, room, "room_border__border_size", 0, 10, 1)
	_setup_room_float(float_border_noise_strength, room, "room_border__border_noise_strength", 0, 1, 0.01)
	_setup_room_float(float_smooth_border_noise, room, "room_border__smooth_border_noise", 0, 1, 0.01)
	_setup_room_float(float_border_tilt, room, "room_border__border_tilt", 0, 1, 0.01)

	_setup_room_float(float_tile_strength, room, "tile_apply__tile_strength", 0, 2, 0.01)
	_setup_room_float(float_tile_smoothing, room, "tile_apply__tile_smoothing", 0, 1, 0.01)
	_setup_room_float(float_tile_ceiling, room, "tile_apply__tile_ceiling", 0, 1, 0.01)
	_setup_room_float(float_tile_ceil_blend, room, "tile_apply__tile_ceil_blend", 0, 1, 0.01)
	_setup_room_float(float_tile_floor, room, "tile_apply__tile_floor", 0, 1, 0.01)
	_setup_room_float(float_tile_floor_blend, room, "tile_apply__tile_floor_blend", 0, 1, 0.01)
	_setup_room_float(float_tile_erase_size, room, "tile_apply__tile_erase_size", 0, 1, 0.01)

	_setup_room_float(float_neighbor_blend_up, room, "neighbors__neighbor_blend_up", 0, 1, 0.001)
	_setup_room_float(float_neighbor_blend_down, room, "neighbors__neighbor_blend_down", 0, 1, 0.001)
	_setup_room_float(float_neighbor_blend_left, room, "neighbors__neighbor_blend_left", 0, 1, 0.001)
	_setup_room_float(float_neighbor_blend_right, room, "neighbors__neighbor_blend_right", 0, 1, 0.001)

	Utils.Conn.disconnect_all(noise_preview_window.closed)
	_setup_noise_form(base_noise_form, noise, room)
	_setup_noise_form(edit_noise_form, border_noise, room)

	tab_container.current_tab = 0
	_on_tab_selected(tab_container.current_tab)
	_rerender_fields(room)

func _rerender_noise_preview(is_noise_tab: bool) -> void:
	if show_preview && is_noise_tab:
		noise_preview_window.show()
	else:
		noise_preview_window.hide()

func _setup_noise_form(form: EditNoiseForm, noise: FastNoiseLite, room: RoomConfig) -> void:
	Utils.Conn.disconnect_all(form.noise_changed)
	form.noise_changed.connect(func()->void:
		room.notify_changed()
		room.set_dirty(true)
	)
	form.visibility_changed.connect(func()->void:
		var texture:NoiseTexture2D = noise_preview.texture
		if form.visible && form.is_visible_in_tree() && texture is NoiseTexture2D:
			texture.in_3d_space = true
			texture.noise = noise
			form.sync_preview()
	)
	form.show_preview_changed.connect(func(showing: bool)->void:
		show_preview = showing
		_rerender_noise_preview(true)
	)
	form.initialize(noise, Callable(func()->bool: return show_preview))
	noise_preview_window.closed.connect(func()->void:
		show_preview = false
		form.sync_preview()
	)

func _setup_room_float(field: FloatField, room: RoomConfig, fieldname: String, minv: float, maxv: float, step: float) -> void:
	assert(field && field is FloatField, fieldname)
	assert(fieldname in room, fieldname)
	assert(_room_get(room, fieldname) is float, fieldname)
	field.initialize(
		fieldname.get_slice("__", 1),
		Callable(self, "_room_get").bind(room, fieldname),
		_room_get(default_room, fieldname),
		minv,
		maxv,
		step,
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: float)->void: _room_set(room, fieldname, val))
	room.on_changed.connect(func()->void: field.update_val())

func _setup_room_int(field: IntField, room: RoomConfig, fieldname: String, minv: int, maxv: int, step: int) -> void:
	assert(field && field is IntField, fieldname)
	assert(fieldname in room, fieldname)
	assert(_room_get(room, fieldname) is int, fieldname)
	field.initialize(
		fieldname.get_slice("__", 1),
		Callable(self, "_room_get").bind(room, fieldname),
		_room_get(default_room, fieldname),
		minv,
		maxv,
		step,
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: int)->void: _room_set(room, fieldname, val))
	room.on_changed.connect(func()->void: field.update_val())

func _setup_room_bool(field: BoolField, room: RoomConfig, fieldname: String) -> void:
	assert(field, fieldname)
	assert(fieldname in room, fieldname)
	assert(_room_get(room, fieldname) is bool, fieldname)
	field.initialize(
		fieldname.get_slice("__", 1),
		Callable(self, "_room_get").bind(room, fieldname),
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: bool)->void: _room_set(room, fieldname, val))
	room.on_changed.connect(func()->void: field.update_val())

func _room_get(room: RoomConfig, fieldname: String) -> Variant:
	assert(fieldname in room, fieldname)
	return room.get(fieldname)

@warning_ignore("untyped_declaration")
func _room_set(room: RoomConfig, fieldname: String, val) -> void:
	assert(fieldname in room, fieldname)
	assert(typeof(val) == typeof(_room_get(room, fieldname)))
	room.set(fieldname, val)
	room.set_dirty(true)
	_rerender_fields(room)

func _rerender_fields(room: RoomConfig) -> void:
	if room.room_border__use_border_noise:
		border_noise_options.show()
	else:
		border_noise_options.hide()
