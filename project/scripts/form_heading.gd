class_name FormHeading
extends Control

@onready var collapse_button: TextureButton = %CollapseButton

var expanded := true
var siblings: Array[Control] = []

func _ready() -> void:
	visibility_changed.connect(_on_visibility_changed)
	collapse_button.toggled.connect(_on_toggle_expand)
	collapse_button.button_pressed = expanded
	_collect_siblings()
	_rerender()

func _collect_siblings()->void:
	siblings.clear()
	if !get_parent(): return
	# start collecting siblings after self, and end at next heading
	var collecting := false
	for node:Node in get_parent().get_children():
		if node == self:
			collecting = true
			continue
		if !collecting:
			continue
		if node is FormHeading || node.name.containsn("heading"):
			break
		if node is Control:
			siblings.append(node)

func _on_toggle_expand(toggled_on: bool) -> void:
	expanded = toggled_on
	_rerender()

func _on_visibility_changed() -> void:
	call_deferred("_rerender")
	pass

func _rerender()->void:
	collapse_button.button_pressed = expanded
	for node:Control in siblings:
		node.visible = expanded && visible
