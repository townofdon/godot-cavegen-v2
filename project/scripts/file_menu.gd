class_name FileMenu
extends MenuBar

@onready var export_dialog: ExportDialog = %ExportDialog
@onready var confirm_dialog: ConfirmDialog = %ConfirmDialog

enum Menu {
	File,
	View,
	Options,
}

enum FileItem {
	New,
	Load,
	Save,
	Export,
	Quit,
}

enum ViewItem {
	SidePanel,
	StatusBar,
}

enum OptionsItem {
	MoveActivePlaneToOrigin,
}

var cfg:GlobalConfig

func initialize(p_cfg:GlobalConfig) -> void:
	cfg = p_cfg
	_rerender()

func _ready() -> void:
	var num_children := len(get_children())
	for menu_idx in range(num_children):
		var popup := get_child(menu_idx)
		if popup is PopupMenu:
			popup.index_pressed.connect(func(item_idx: int):
				_on_menu_item_press(menu_idx, item_idx)
			)

func _on_menu_item_press(menu: int, item: int):
	if menu == Menu.File:
		if item == FileItem.New:
			_on_new()
		elif item == FileItem.Load:
			_on_load()
		elif item == FileItem.Save:
			_on_save()
		elif item == FileItem.Export:
			_on_export()
		elif item == FileItem.Quit:
			_on_quit()
	elif menu == Menu.View:
		if item == ViewItem.SidePanel:
			_on_toggle_side_panel()
		if item == ViewItem.StatusBar:
			_on_toggle_status_bar()
	elif menu == Menu.Options:
		if item == OptionsItem.MoveActivePlaneToOrigin:
			_on_toggle_move_active_plane_to_origin()

func _set_menu_item_checked(menu: int, item: int, checked: bool):
	var popup := get_child(menu)
	if popup is PopupMenu:
		pass
		popup.set_item_checked(item, checked)

func _rerender() -> void:
	_set_menu_item_checked(Menu.Options, OptionsItem.MoveActivePlaneToOrigin, cfg.move_active_plane_to_origin)

func _on_new() -> void:
	pass

func _on_load() -> void:
	pass

func _on_save() -> void:
	pass

func _on_quit() -> void:
	var data := ConfirmDialogData.new(func():
		get_tree().quit(0))
	data.title = "Quit Application?"
	data.text_body = "All unsaved data will be lost."
	data.text_btn_confirm = "Quit"
	confirm_dialog.open(data)

func _on_export() -> void:
	export_dialog.open()

func _on_toggle_side_panel() -> void:
	pass

func _on_toggle_status_bar() -> void:
	pass

func _on_toggle_move_active_plane_to_origin() -> void:
	cfg.move_active_plane_to_origin = !cfg.move_active_plane_to_origin
	_rerender()
