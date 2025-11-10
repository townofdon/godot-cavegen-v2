class_name Utils
extends Object

class Conn:
	static func disconnect_all(sig:Signal)->void:
		for dict:Dictionary in sig.get_connections():
			sig.disconnect(dict.callable)

class Vector:
	## Project a onto b. Result is parallel to b.
	static func project(a:Vector2, b:Vector2) -> Vector2:
		if a.is_zero_approx() || b.is_zero_approx():
			return Vector2.ZERO
		var projection := a.project(b)
		if is_nan(projection.x) || is_nan(projection.y):
			return Vector2.ZERO
		return projection
