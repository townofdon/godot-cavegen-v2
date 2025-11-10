#ifndef FLOOD_FILL_3D_HPP
#define FLOOD_FILL_3D_HPP

#include "constants.h"
#include "godot_cpp/variant/vector3i.hpp"
#include "mesh_gen_util.hpp"

namespace godot {

// note - this mutates `screen`
inline int flood_fill_3d(MG::Context ctx, int screen[], Vector3i start, int next, int maxIterations = 5) {
	int sx = ctx.numCells.x;
	int sy = ctx.numCells.y;
	int sz = ctx.numCells.z;

	// lambda function
	auto apply = [&, sx, sy, sz, next](int x, int y, int z) {
		int i = x + y * sx + z * sy * sx;
		if (
			x < 0 ||
			x >= sx ||
			y < 0 ||
			y >= sy ||
			z < 0 ||
			z >= sz ||
			screen[i] == 0 ||
			screen[i] == next) {
			return false;
		}
		screen[i] = next;
		return true;
	};

	Vector3i queue[MAX_NOISE_NODES] = {
		start,
	};
	int iter = 0;
	int neighborsFound = 0;
	int queueSize = 1;

	screen[MG::NoiseIndex(ctx, start.x, start.y, start.z)] = next;

	while (queueSize > 0 && iter < maxIterations) {
		iter++;
		Vector3i currPixel = queue[queueSize - 1];
		queueSize--;

		int x = currPixel.x;
		int y = currPixel.y;
		int z = currPixel.z;

		if (apply(x + 1, y, z)) {
			queue[queueSize] = Vector3i(x + 1, y, z);
			queueSize++;
			neighborsFound++;
		}
		if (apply(x - 1, y, z)) {
			queue[queueSize] = Vector3i(x - 1, y, z);
			queueSize++;
			neighborsFound++;
		}
		if (apply(x, y + 1, z)) {
			queue[queueSize] = Vector3i(x, y + 1, z);
			queueSize++;
			neighborsFound++;
		}
		if (apply(x, y - 1, z)) {
			queue[queueSize] = Vector3i(x, y - 1, z);
			queueSize++;
			neighborsFound++;
		}
		if (apply(x, y, z + 1)) {
			queue[queueSize] = Vector3i(x, y, z + 1);
			queueSize++;
			neighborsFound++;
		}
		if (apply(x, y, z - 1)) {
			queue[queueSize] = Vector3i(x, y, z - 1);
			queueSize++;
			neighborsFound++;
		}
	}
	return neighborsFound;
}

} //namespace godot

#endif // FLOOD_FILL_3D_HPP
