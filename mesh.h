#ifndef MESH_H
#define MESH_H

// include 'clgm' higly optimsed math library for 2D and 3D stuff
#include "cglm/cglm.h"
// include the sokol header file --> simple GPU API wrapper - pixels, rendering
#include "dependencies/sokol/sokol_gfx.h"
// include other integer types from vanilla C
#include <stdint.h>

// a single vertex in our "see of vertices"
typedef struct {
  // where the model sits in 3D space meaning in terms of x, y and z
  vec3 position;
  // direction for calculating the lighting
  vec3 normal;
} vertex_t;

// the complete mesh structure for GPU to render
typedef struct {
  // CPU side ==> the vertices of the model, etc
  // NOTE: these `vertices` and `indices` are heap allocated!
  vertex_t *vertices;
  uint32_t vertex_count;
  uint32_t *indices;
  uint32_t index_count;
  // GPU side ==> buffer that sokol is going to point to --> upload to GPU VRAM
  sg_buffer vertex_buffer;
  sg_buffer index_buffer;
} mesh_t;

// function signature for the CPU to pass the mesh data to the GPU
void mesh_upload(mesh_t *mesh);

// function signature to cleanup the CPU arrays and GPU buffers
void mesh_destroy(mesh_t *mesh);

#endif
