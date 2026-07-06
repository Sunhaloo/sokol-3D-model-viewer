#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

// mesh_t struct and types needed for the output parameter
#include "mesh.h"

// function signature to load a `.obj` file and turn into `mesh_t` for the GPU
int obj_load(const char *path, mesh_t *out_mesh);

#endif
