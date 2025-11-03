class_name ExportDialog
extends Panel

@onready var export_buttons: Control = %ExportButtons
@onready var export_progress_label: Label = %ExportProgressLabel
@onready var export_progress_bar: ProgressBar = %ExportProgressBar
@onready var button_export_start: Button = %ButtonExportStart
@onready var button_export_cancel: Button = %ButtonExportCancel

@onready var text_description: Label = $Panel/MarginContainer/VBoxContainer/TextExportDescription
@onready var text_success: Label = $Panel/MarginContainer/VBoxContainer/TextExportSuccess
@onready var text_detail: Label = $Panel/MarginContainer/VBoxContainer/TextExportDetail
@onready var button_export_close: Button = $Panel/MarginContainer/VBoxContainer/ButtonExportClose

signal user_start_export
signal dialog_opened
signal dialog_closed

var cavegen: CaveGen

enum RenderState {
	ReadyToExport,
	Exporting,
	ExportCompleted,
}

var state := RenderState.ReadyToExport

func initialize(p_cavegen: CaveGen) -> void:
	cavegen = p_cavegen
	cavegen.export_started.connect(_on_export_started)
	cavegen.export_progress.connect(_on_export_progress)
	cavegen.export_completed.connect(_on_export_completed)

func _ready() -> void:
	button_export_start.pressed.connect(_on_start_pressed)
	button_export_cancel.pressed.connect(_on_close_pressed)
	button_export_close.pressed.connect(_on_close_pressed)
	_set_state(RenderState.ReadyToExport)
	close()

func open() -> void:
	_set_state(RenderState.ReadyToExport)
	show()
	dialog_opened.emit()

func close() -> void:
	dialog_closed.emit()
	hide()

func _set_state(p_state: RenderState) -> void:
	state = p_state
	_rerender()

func _rerender() -> void:
	if state == RenderState.ReadyToExport:
		text_description.show()
		export_buttons.show()
		export_progress_label.hide()
		export_progress_bar.hide()
		text_success.hide()
		text_detail.hide()
		button_export_close.hide()
	elif state == RenderState.Exporting:
		text_description.show()
		export_buttons.hide()
		export_progress_label.show()
		export_progress_bar.show()
		text_success.hide()
		text_detail.hide()
		button_export_close.hide()
	elif state == RenderState.ExportCompleted:
		text_description.hide()
		export_buttons.hide()
		export_progress_label.hide()
		export_progress_bar.hide()
		text_success.show()
		text_detail.show()
		button_export_close.show()

func _on_start_pressed() -> void:
	user_start_export.emit()

func _on_close_pressed() -> void:
	if cavegen && cavegen.saving:
		return
	close()
	_set_state(RenderState.ReadyToExport)

func _on_export_started() -> void:
	_set_state(RenderState.Exporting)

func _on_export_completed(files: PackedStringArray) -> void:
	text_detail.text = "File(s) saved as:\n- " + "\n- ".join(files)
	_set_state(RenderState.ExportCompleted)

func _on_export_progress(pct: float) -> void:
	export_progress_bar.value = pct
