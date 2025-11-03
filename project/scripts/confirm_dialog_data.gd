class_name ConfirmDialogData
extends RefCounted

var title:String = "Confirm"
var text_body:String = "Are you sure?"
var text_btn_cancel:String = "Cancel"
var text_btn_confirm:String = "Ok"
var callback:Callable

func _init(p_callback: Callable) -> void:
	callback = p_callback
