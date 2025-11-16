class_name FloatField
extends Control

signal value_changed(val: float)

@onready var label: Label = $BaseField/MarginContainer/HBoxContainer/Label
@onready var check_button: CheckButton = $BaseField/MarginContainer/HBoxContainer/CheckButton
@onready var line_edit: LineEdit = $BaseField/MarginContainer/HBoxContainer/LineEdit
@onready var spin_box: SpinBox = $BaseField/MarginContainer/HBoxContainer/SpinBox
@onready var h_slider: HSlider = $BaseField/MarginContainer/HBoxContainer/HSlider
@onready var reset_button: Button = $BaseField/MarginContainer/HBoxContainer/Label/ResetButton
@onready var option_button: OptionButton = $BaseField/MarginContainer/HBoxContainer/OptionButton

var _get_value: Callable
var _default_value: float

func _ready() -> void:
	line_edit.hide()
	check_button.hide()
	option_button.hide()
	spin_box.value_changed.connect(_on_user_change)
	h_slider.value_changed.connect(_on_user_change)
	reset_button.pressed.connect(func() -> void: _on_user_change(_default_value))

func initialize(field_name: String, p_get_value: Callable, p_default_value: float, min_value: float, max_value: float, step: float) -> void:
	_get_value = p_get_value
	_default_value = p_default_value
	assert(_get_value)
	@warning_ignore("untyped_declaration")
	var test_value = _get_value.call()
	assert(test_value is float, "_get_value must yield a float. field=%s,value=%s" % [field_name, test_value])
	assert(min_value != max_value)
	assert(max_value > min_value)
	label.text = field_name
	h_slider.min_value = min_value
	h_slider.max_value = max_value
	h_slider.step = step
	spin_box.min_value = min_value
	spin_box.max_value = max_value
	spin_box.step = step
	update_val()

func set_exponential_edit(exp_edit: bool) -> void:
	h_slider.exp_edit = exp_edit

func update_val() -> void:
	if !_get_value: return
	@warning_ignore("untyped_declaration")
	var newval = _get_value.call()
	if !(newval is float): return
	spin_box.set_value_no_signal(newval)
	h_slider.set_value_no_signal(newval)
	_render_reset_button(!is_equal_approx(newval, _default_value))

func _on_user_change(newval: float) -> void:
	spin_box.set_value_no_signal(newval)
	h_slider.set_value_no_signal(newval)
	value_changed.emit(newval)
	_render_reset_button(!is_equal_approx(newval, _default_value))

func _render_reset_button(should_show: bool) -> void:
	if should_show:
		reset_button.show()
		reset_button.disabled = false
	else:
		reset_button.hide()
		reset_button.disabled = true
