class_name Vector3Field
extends Control

signal value_changed(val: Vector3)

@onready var float_field_x: FloatField = %FloatFieldX
@onready var float_field_y: FloatField = %FloatFieldY
@onready var float_field_z: FloatField = %FloatFieldZ

var _get_value: Callable
var _default_value: Vector3

func initialize(field_name: String, p_get_value: Callable, p_default_value: Vector3, minv: float, maxv: float, step: float) -> void:
	_get_value = p_get_value
	_default_value = p_default_value
	assert(_get_value)
	@warning_ignore("untyped_declaration")
	var test_value = _get_value.call()
	assert(test_value is Vector3, "_get_value must yield a Vector3. field=%s,value=%s" % [field_name, test_value])
	assert(minv != maxv)
	assert(maxv > minv)
	var get_value_x := Callable(self, "get_component_value").bind(Vector3(1,0,0))
	var get_value_y := Callable(self, "get_component_value").bind(Vector3(0,1,0))
	var get_value_z := Callable(self, "get_component_value").bind(Vector3(0,0,1))
	float_field_x.initialize(field_name, get_value_x, _default_value.x, minv, maxv, step)
	float_field_y.initialize("", get_value_y, _default_value.y, minv, maxv, step)
	float_field_z.initialize("", get_value_z, _default_value.z, minv, maxv, step)
	Utils.Conn.disconnect_all(float_field_x.value_changed)
	Utils.Conn.disconnect_all(float_field_y.value_changed)
	Utils.Conn.disconnect_all(float_field_z.value_changed)
	float_field_x.value_changed.connect(_on_change.bind(Vector3(1,0,0)))
	float_field_y.value_changed.connect(_on_change.bind(Vector3(0,1,0)))
	float_field_z.value_changed.connect(_on_change.bind(Vector3(0,0,1)))

func get_component_value(mask: Vector3) -> float:
	assert(
		mask == Vector3(1,0,0) ||
		mask == Vector3(0,1,0) ||
		mask == Vector3(0,0,1)
	)
	var val:Vector3 = _get_value.call()
	assert(val is Vector3)
	var cmp := val * mask
	return cmp.x + cmp.y + cmp.z

# handle change event for x, y, or z component
func _on_change(val: float, mask: Vector3) -> void:
	assert(
		mask == Vector3(1,0,0) ||
		mask == Vector3(0,1,0) ||
		mask == Vector3(0,0,1)
	)
	var inverted_mask := Vector3(1,1,1) - mask
	var oldval:Vector3 = _get_value.call()
	assert(oldval is Vector3)
	var newval:Vector3 = oldval * inverted_mask + mask * val
	value_changed.emit(newval)
