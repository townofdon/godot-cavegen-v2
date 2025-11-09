class_name EditNoiseForm
extends VBoxContainer

signal noise_changed

@onready var enum_noise_type: DropdownField = %EnumNoiseType
@onready var int_seed: IntField = %IntSeed
@onready var float_frequency: FloatField = %FloatFrequency
@onready var vector_3_offset: Vector3Field = %Vector3Offset

@onready var heading_cellular: FormHeading = %HeadingCellular
@onready var enum_cell_distance_fn: DropdownField = %EnumCellDistanceFn
@onready var float_cell_jitter: FloatField = %FloatCellJitter
@onready var enum_cell_return_type: DropdownField = %EnumCellReturnType

@onready var enum_frac_type: DropdownField = %EnumFracType
@onready var int_frac_octaves: IntField = %IntFracOctaves
@onready var float_frac_lacunarity: FloatField = %FloatFracLacunarity
@onready var float_frac_gain: FloatField = %FloatFracGain
@onready var float_frac_w_strength: FloatField = %FloatFracWStrength

@onready var bool_warp_enabled: BoolField = %BoolWarpEnabled
@onready var enum_warp_type: DropdownField = %EnumWarpType
@onready var float_warp_amplitude: FloatField = %FloatWarpAmplitude
@onready var float_warp_frequency: FloatField = %FloatWarpFrequency
@onready var enum_warp_frac_type: DropdownField = %EnumWarpFracType
@onready var int_warp_frac_octaves: IntField = %IntWarpFracOctaves
@onready var float_warp_frac_lacunarity: FloatField = %FloatWarpFracLacunarity
@onready var float_warp_frac_gain: FloatField = %FloatWarpFracGain

const NOISE_TYPE_DICT: Dictionary[String, int] = {
	"SIMPLEX": FastNoiseLite.NoiseType.TYPE_SIMPLEX,
	"SIMPLEX_SMOOTH": FastNoiseLite.NoiseType.TYPE_SIMPLEX_SMOOTH,
	"CELLULAR": FastNoiseLite.NoiseType.TYPE_CELLULAR,
	"PERLIN": FastNoiseLite.NoiseType.TYPE_PERLIN,
	"VALUE": FastNoiseLite.NoiseType.TYPE_VALUE,
	"VALUE_CUBIC": FastNoiseLite.NoiseType.TYPE_VALUE_CUBIC,
}
const FRAC_TYPE_DICT: Dictionary[String, int] = {
	"NONE": FastNoiseLite.FractalType.FRACTAL_NONE,
	"FBM": FastNoiseLite.FractalType.FRACTAL_FBM,
	"RIDGED": FastNoiseLite.FractalType.FRACTAL_RIDGED,
	"PING_PONG": FastNoiseLite.FractalType.FRACTAL_PING_PONG,
}
const CELL_DIST_FUNC_DICT: Dictionary[String, int] = {
	"EUCLIDEAN": FastNoiseLite.CellularDistanceFunction.DISTANCE_EUCLIDEAN,
	"EUCLIDEAN_SQUARED": FastNoiseLite.CellularDistanceFunction.DISTANCE_EUCLIDEAN_SQUARED,
	"MANHATTAN": FastNoiseLite.CellularDistanceFunction.DISTANCE_MANHATTAN,
	"HYBRID": FastNoiseLite.CellularDistanceFunction.DISTANCE_HYBRID,
}
const CELL_RETURN_TYPE_DICT: Dictionary[String, int] = {
	"CELL_VALUE": FastNoiseLite.CellularReturnType.RETURN_CELL_VALUE,
	"DISTANCE": FastNoiseLite.CellularReturnType.RETURN_DISTANCE,
	"DISTANCE2": FastNoiseLite.CellularReturnType.RETURN_DISTANCE2,
	"DISTANCE2_ADD": FastNoiseLite.CellularReturnType.RETURN_DISTANCE2_ADD,
	"DISTANCE2_SUB": FastNoiseLite.CellularReturnType.RETURN_DISTANCE2_SUB,
	"DISTANCE2_MUL": FastNoiseLite.CellularReturnType.RETURN_DISTANCE2_MUL,
	"DISTANCE2_DIV": FastNoiseLite.CellularReturnType.RETURN_DISTANCE2_DIV,
}
const DOMAIN_WARP_TYPE_DICT: Dictionary[String, int] = {
	"SIMPLEX": FastNoiseLite.DomainWarpType.DOMAIN_WARP_SIMPLEX,
	"SIMPLEX_REDUCED": FastNoiseLite.DomainWarpType.DOMAIN_WARP_SIMPLEX_REDUCED,
	"BASIC_GRID": FastNoiseLite.DomainWarpType.DOMAIN_WARP_BASIC_GRID,
}
const DOMAIN_WARP_FRAC_TYPE_DICT: Dictionary[String, int] = {
	"NONE": FastNoiseLite.DomainWarpFractalType.DOMAIN_WARP_FRACTAL_NONE,
	"PROGRESSIVE": FastNoiseLite.DomainWarpFractalType.DOMAIN_WARP_FRACTAL_PROGRESSIVE,
	"INDEPENDENT": FastNoiseLite.DomainWarpFractalType.DOMAIN_WARP_FRACTAL_INDEPENDENT,
}
const LABEL_OVERRIDES: Dictionary[String, String] = {
	"cellular_distance_function": "distance_function",
	"cellular_jitter": "jitter",
	"cellular_return_type": "return_type",
	"fractal_octaves": "fractal_octaves",
	"fractal_lacunarity": "fractal_lacunarity",
	"fractal_gain": "fractal_gain",
	"fractal_weighted_strength": "weighted_strength",
	"domain_warp_amplitude": "amplitude",
	"domain_warp_frequency": "frequency",
	"domain_warp_fractal_type": "fractal_type",
	"domain_warp_fractal_octaves": "fractal_octaves",
	"domain_warp_fractal_lacunarity": "fractal_lacunarity",
	"domain_warp_fractal_gain": "fractal_gain",
}

var default_noise: FastNoiseLite

func _ready() -> void:
	default_noise = FastNoiseLite.new()
	default_noise.reset_state()

func initialize(noise: FastNoiseLite) -> void:
	_setup_noise_enum(enum_noise_type, noise, "noise_type", NOISE_TYPE_DICT)
	_setup_noise_int(int_seed, noise, "seed", 0, 999999, 1)
	_setup_noise_float(float_frequency, noise, "frequency", 0.001, 1, 0.001)
	_setup_noise_vector3(vector_3_offset, noise, "offset", -500, 500, 0.01)
	float_frequency.set_exponential_edit(true)

	_setup_noise_enum(enum_cell_distance_fn, noise, "cellular_distance_function", CELL_DIST_FUNC_DICT)
	_setup_noise_float(float_cell_jitter, noise, "cellular_jitter", 0, 10, 0.001)
	_setup_noise_enum(enum_cell_return_type, noise, "cellular_return_type", CELL_RETURN_TYPE_DICT)

	_setup_noise_enum(enum_frac_type, noise, "fractal_type", FRAC_TYPE_DICT)
	_setup_noise_int(int_frac_octaves, noise, "fractal_octaves", 1, 10, 1)
	_setup_noise_float(float_frac_lacunarity, noise, "fractal_lacunarity", 0, 10, 0.001)
	_setup_noise_float(float_frac_gain, noise, "fractal_gain", 0, 5, 0.001)
	_setup_noise_float(float_frac_w_strength, noise, "fractal_weighted_strength", 0, 1, 0.001)

	_setup_noise_bool(bool_warp_enabled, noise, "domain_warp_enabled")
	_setup_noise_enum(enum_warp_type, noise, "domain_warp_type", DOMAIN_WARP_TYPE_DICT)
	_setup_noise_float(float_warp_amplitude, noise, "domain_warp_amplitude", 0, 50, 0.01)
	_setup_noise_float(float_warp_frequency, noise, "domain_warp_frequency", 0, 1, 0.001)
	_setup_noise_enum(enum_warp_frac_type, noise, "domain_warp_fractal_type", DOMAIN_WARP_FRAC_TYPE_DICT)
	_setup_noise_int(int_warp_frac_octaves, noise, "domain_warp_fractal_octaves", 1, 10, 1)
	_setup_noise_float(float_warp_frac_lacunarity, noise, "domain_warp_fractal_lacunarity", 0, 10, 0.001)
	_setup_noise_float(float_warp_frac_gain, noise, "domain_warp_fractal_gain", 0, 4, 0.01)
	float_warp_frequency.set_exponential_edit(true)

	_rerender_fields(noise)

func _rerender_fields(noise: FastNoiseLite)->void:
	if noise.noise_type == FastNoiseLite.NoiseType.TYPE_CELLULAR:
		heading_cellular.show()
	else:
		heading_cellular.hide()

	if noise.fractal_type == FastNoiseLite.FractalType.FRACTAL_NONE:
		int_frac_octaves.hide()
		float_frac_lacunarity.hide()
		float_frac_gain.hide()
		float_frac_w_strength.hide()
	else:
		int_frac_octaves.show()
		float_frac_lacunarity.show()
		float_frac_gain.show()
		float_frac_w_strength.show()

	if noise.domain_warp_enabled:
		enum_warp_type.show()
		float_warp_amplitude.show()
		float_warp_frequency.show()
		enum_warp_frac_type.show()
		int_warp_frac_octaves.show()
		float_warp_frac_lacunarity.show()
		float_warp_frac_gain.show()
	else:
		enum_warp_type.hide()
		float_warp_amplitude.hide()
		float_warp_frequency.hide()
		enum_warp_frac_type.hide()
		int_warp_frac_octaves.hide()
		float_warp_frac_lacunarity.hide()
		float_warp_frac_gain.hide()

func _setup_noise_enum(field: DropdownField, noise: FastNoiseLite, fieldname: String, enum_dict: Dictionary[String, int]) -> void:
	assert(field && field is DropdownField, fieldname)
	assert(fieldname in noise, fieldname)
	assert(_noise_get(noise, fieldname) is int, fieldname)
	field.initialize(
		enum_dict,
		_label_get(fieldname),
		Callable(self, "_noise_get").bind(noise, fieldname),
		_noise_get(default_noise, fieldname),
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: int)->void: _noise_set(noise, fieldname, val))

func _setup_noise_int(field: IntField, noise: FastNoiseLite, fieldname: String, minv: int, maxv: int, step: int) -> void:
	assert(field && field is IntField, fieldname)
	assert(fieldname in noise, fieldname)
	assert(_noise_get(noise, fieldname) is int, fieldname)
	field.initialize(
		_label_get(fieldname),
		Callable(self, "_noise_get").bind(noise, fieldname),
		_noise_get(default_noise, fieldname),
		minv,
		maxv,
		step,
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: int)->void: _noise_set(noise, fieldname, val))

func _setup_noise_float(field: FloatField, noise: FastNoiseLite, fieldname: String, minv: float, maxv: float, step: float) -> void:
	assert(field && field is FloatField, fieldname)
	assert(fieldname in noise, fieldname)
	assert(_noise_get(noise, fieldname) is float, fieldname)
	field.initialize(
		_label_get(fieldname),
		Callable(self, "_noise_get").bind(noise, fieldname),
		_noise_get(default_noise, fieldname),
		minv,
		maxv,
		step,
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: float)->void: _noise_set(noise, fieldname, val))

func _setup_noise_vector3(field: Vector3Field, noise: FastNoiseLite, fieldname: String, minv: float, maxv: float, step: float) -> void:
	assert(field && field is Vector3Field, fieldname)
	assert(fieldname in noise, fieldname)
	assert(_noise_get(noise, fieldname) is Vector3, fieldname)
	field.initialize(
		_label_get(fieldname),
		Callable(self, "_noise_get").bind(noise, fieldname),
		_noise_get(default_noise, fieldname),
		minv,
		maxv,
		step,
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: Vector3)->void: _noise_set(noise, fieldname, val))

func _setup_noise_bool(field: BoolField, noise: FastNoiseLite, fieldname: String) -> void:
	assert(field && field is BoolField, fieldname)
	assert(fieldname in noise, fieldname)
	assert(_noise_get(noise, fieldname) is bool, fieldname)
	field.initialize(
		_label_get(fieldname),
		Callable(self, "_noise_get").bind(noise, fieldname),
	)
	Utils.Conn.disconnect_all(field.value_changed)
	field.value_changed.connect(func(val: bool)->void: _noise_set(noise, fieldname, val))

func _label_get(fieldname: String) -> String:
	if fieldname in LABEL_OVERRIDES:
		return LABEL_OVERRIDES[fieldname]
	return fieldname

func _noise_get(noise: FastNoiseLite, fieldname: String) -> Variant:
	assert(fieldname in noise, fieldname)
	return noise.get(fieldname)

@warning_ignore("untyped_declaration")
func _noise_set(noise: FastNoiseLite, fieldname: String, val) -> void:
	assert(fieldname in noise, fieldname)
	assert(typeof(val) == typeof(_noise_get(noise, fieldname)))
	noise.set(fieldname, val)
	noise_changed.emit()
	_rerender_fields(noise)
