extends Node3D
class_name CaveGen

@export var cfg:GlobalConfig
@export var room:RoomConfig
@export var noise:FastNoiseLite
@export var borderNoise:FastNoiseLite

@onready var meshGen:MeshGen = %MeshGen
@onready var notifTimer:Timer = %Timer
@onready var testCube:CSGBox3D = %TestCube

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

	# setup meshgen
	regenerate()

	#setup noise
	noiseB = noise.duplicate()
	borderNoiseB = borderNoise.duplicate()

func regenerate():
	if Engine.is_editor_hint(): return
	if !meshGen: return
	if !cfg: return
	if !room: return
	if !noise: return
	if !borderNoise: return
	meshGen.generate(cfg, room, noise, borderNoise)

	if meshGen.mesh is ImmediateMesh:
		var mat:ShaderMaterial = meshGen.material_override
		if mat && mat is ShaderMaterial:
			var y_ceil := cfg.room_height * cfg.ceiling - cfg.active_plane_offset
			mat.set_shader_parameter("y_ceil", y_ceil)
			mat.set_shader_parameter("y_min", 0.0)

func _process(_delta: float) -> void:
	if _did_noise_change(noise, noiseB):
		_notify_change()
		noiseB = noise.duplicate()
	if _did_noise_change(borderNoise, borderNoiseB):
		_notify_change()
		borderNoiseB = borderNoise.duplicate()

func _notify_change():
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
