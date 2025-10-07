# Cavegen V2

This picks up where https://github.com/townofdon/godot-cavegen left off.

CaveGen is a low-poly 2.5D cave generation tool. It allows creating a grid of cave rooms, each as its own 3D mesh, that can be exported as a collection of `.obj` files for further refinement in your favorite 3D editing software.

This was built with top-down genres in mind, but can easily be extended to apply to other use cases.

Features:

- Procedural mesh generation from noise functions
- Editing room-to-room connections and layouts using 2D tilemaps

This project was started from the [godot-cpp template](https://github.com/godotengine/godot-cpp-template).

## Project Structure

```
.
├── bin         - Compiled extension code
├── project     - Godot project
├── godot-cpp   - C++ bindings (git submodule)
└── src         - Extension source code
```

## Compiling

This project uses [SCons](https://scons.org/) for compiling binaries.

```bash
# ENVIRONMENT=windows|linux|macos
scons platform=<ENVIRONMENT>
```

## Actions

This repository comes with a GitHub action that builds the GDExtension for cross-platform use. It triggers automatically for each pushed change. You can find and edit it in [builds.yml](.github/workflows/builds.yml).
After a workflow run is complete, you can find the file `godot-cpp-template.zip` on the `Actions` tab on GitHub.
