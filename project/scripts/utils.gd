class_name Utils
extends Object

class Conn:
	static func disconnect_all(sig:Signal)->void:
		for dict:Dictionary in sig.get_connections():
			sig.disconnect(dict.callable)
