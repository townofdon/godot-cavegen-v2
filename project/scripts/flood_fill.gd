extends Node

class_name FloodFill

static func fill(
	screen: PackedInt32Array,
	numCells: Vector2i,
	startAt: Vector2i,
	prev: int,
	next: int,
) -> void:
	var index := func(x: int, y: int) -> int:
		return x + y * numCells.x

	var isValid := func(x: int, y: int) -> bool:
		var i:int = index.call(x, y)
		if (
			x < 0 ||
			y < 0 ||
			x >= numCells.x ||
			y >= numCells.y ||
			screen.get(i) != prev ||
			screen.get(i) == next
		):
			return false
		return true

	var queue := PackedVector2Array()
	queue.append(startAt)
	screen.set(index.call(startAt.x, startAt.y), next)

	while len(queue) > 0:
		pass
		var curr:Vector2i = queue.get(len(queue) - 1)
		queue.remove_at(len(queue) - 1)
		var x:= curr.x
		var y:= curr.y
		if (isValid.call(x+1, y)):
			var i:int = index.call(x+1, y)
			screen.set(i, next)
			queue.append(Vector2i(x+1, y))
		if (isValid.call(x-1, y)):
			var i:int = index.call(x-1, y)
			screen.set(i, next)
			queue.append(Vector2i(x-1, y))
		if (isValid.call(x, y+1)):
			var i:int = index.call(x, y+1)
			screen.set(i, next)
			queue.append(Vector2i(x, y+1))
		if (isValid.call(x, y-1)):
			var i:int = index.call(x, y-1)
			screen.set(i, next)
			queue.append(Vector2i(x, y-1))
