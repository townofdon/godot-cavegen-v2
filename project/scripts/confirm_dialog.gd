class_name ConfirmDialog
extends Panel

@onready var title: Label = $Panel/MarginContainer/VBoxContainer/Title
@onready var description: Label = $Panel/MarginContainer/VBoxContainer/Description
@onready var button_cancel: Button = $Panel/MarginContainer/VBoxContainer/Buttons/ContainerCancel/ButtonCancel
@onready var button_confirm: Button = $Panel/MarginContainer/VBoxContainer/Buttons/ContainerConfirm/ButtonConfirm

var data:ConfirmDialogData

func _ready() -> void:
	button_cancel.pressed.connect(_on_cancel_press)
	button_confirm.pressed.connect(_on_confirm_press)
	hide()

func open(p_data: ConfirmDialogData) -> void:
	data = p_data
	title.text = data.title
	description.text = data.text_body
	button_cancel.text = data.text_btn_cancel
	button_confirm.text = data.text_btn_confirm
	show()

func close() -> void:
	hide()

func _on_cancel_press() -> void:
	close()

func _on_confirm_press() -> void:
	if !visible:
		return
	data.callback.call_deferred()
