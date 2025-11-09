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
@onready var float_tilt_y: FloatField = %FloatTiltY
@onready var float_tilt_x: FloatField = %FloatTiltX
@onready var float_tilt_z: FloatField = %FloatTiltZ

@onready var base_noise_form: EditNoiseForm = %BaseNoiseForm

var default_room: RoomConfig

func _ready() -> void:
	default_room = RoomConfig.new()
	default_room.reset_state()

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
	_setup_room_float(float_tilt_y, room, "room_noise__tilt_y", 0, 2, 0.001)
	_setup_room_float(float_tilt_x, room, "room_noise__tilt_x", 0, 2, 0.001)
	_setup_room_float(float_tilt_z, room, "room_noise__tilt_z", 0, 2, 0.001)

	_setup_noise_form(base_noise_form, noise, room)

func _setup_noise_form(form: EditNoiseForm, noise: FastNoiseLite, room: RoomConfig) -> void:
	form.initialize(noise)
	Utils.Conn.disconnect_all(form.noise_changed)
	form.noise_changed.connect(func()->void:
		room.notify_changed()
		room.set_dirty(true)
	)

func _setup_room_float(field: FloatField, room: RoomConfig, fieldname: String, minv: float, maxv: float, step: float) -> void:
	assert(field, fieldname)
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
