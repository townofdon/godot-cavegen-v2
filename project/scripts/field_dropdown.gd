class_name DropdownField
extends Control

signal value_changed(val: int)

@onready var label: Label = $BaseField/MarginContainer/HBoxContainer/Label
@onready var check_button: CheckButton = $BaseField/MarginContainer/HBoxContainer/CheckButton
@onready var line_edit: LineEdit = $BaseField/MarginContainer/HBoxContainer/LineEdit
@onready var spin_box: SpinBox = $BaseField/MarginContainer/HBoxContainer/SpinBox
@onready var h_slider: HSlider = $BaseField/MarginContainer/HBoxContainer/HSlider
@onready var option_button: OptionButton = $BaseField/MarginContainer/HBoxContainer/OptionButton
@onready var reset_button: Button = $BaseField/MarginContainer/HBoxContainer/Label/ResetButton

var _get_value: Callable
var _default_value: int

func _ready() -> void:
	reset_button.hide()
	check_button.hide()
	line_edit.hide()
	spin_box.hide()
	h_slider.hide()
	option_button.show()
	option_button.item_selected.connect(_on_item_selected)
	reset_button.pressed.connect(func() -> void: _on_internal_change(_default_value))

func initialize(enum_dict: Dictionary[String, int], field_name: String, p_get_value: Callable, p_default_value: int)->void:
	_get_value = p_get_value
	_default_value = p_default_value
	assert(_get_value)
	@warning_ignore("untyped_declaration")
	var test_value = _get_value.call()
	assert(test_value is int, "_get_value must yield an int. field=%s,value=%s" % [field_name, test_value])
	# remove placeholder options
	option_button.clear()
	# add options from enum
	for key:String in enum_dict.keys():
		assert(key is String)
		assert(enum_dict[key] is int)
		var id:int = enum_dict[key]
		option_button.add_item(key, id)
	label.text = field_name
	# set initial value
	_on_internal_change(_get_value.call())

func _on_item_selected(idx: int) -> void:
	assert(idx >= 0)
	var id:int = option_button.get_item_id(idx)
	value_changed.emit(id)
	_render_reset_button(_default_value != id)

func _on_internal_change(id: int) -> void:
	var idx:int = option_button.get_item_index(id)
	assert(idx >= 0)
	option_button.select(idx)
	_render_reset_button(_default_value != id)

func _render_reset_button(should_show: bool) -> void:
	if should_show:
		reset_button.show()
		reset_button.disabled = false
	else:
		reset_button.hide()
		reset_button.disabled = true
