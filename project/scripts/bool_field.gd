class_name BoolField
extends Control

signal value_changed(val: bool)

@onready var label: Label = $BaseField/MarginContainer/HBoxContainer/Label
@onready var check_button: CheckButton = $BaseField/MarginContainer/HBoxContainer/CheckButton
@onready var line_edit: LineEdit = $BaseField/MarginContainer/HBoxContainer/LineEdit
@onready var spin_box: SpinBox = $BaseField/MarginContainer/HBoxContainer/SpinBox
@onready var h_slider: HSlider = $BaseField/MarginContainer/HBoxContainer/HSlider

var _get_value: Callable

func _ready() -> void:
	check_button.show()
	line_edit.hide()
	spin_box.hide()
	h_slider.hide()
	check_button.toggled.connect(_on_internal_change)

func initialize(field_name: String, p_get_value: Callable) -> void:
	_get_value = p_get_value
	assert(_get_value)
	var test_value = _get_value.call()
	assert(test_value is bool, "_get_value must yield a bool. field=%s,value=%s" % [field_name, test_value])
	label.text = field_name
	update_val()

func update_val() -> void:
	if !_get_value: return
	var newval = _get_value.call()
	if !(newval is bool): return
	check_button.button_pressed = newval

func _on_internal_change(newval: bool) -> void:
	value_changed.emit(!!newval)
