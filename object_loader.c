// include the sokol header file --> simple GPU API wrapper - pixels, rendering
#include "dependencies/sokol/sokol_gfx.h"
// include our mesh's header file and implement the function signature here
#include "object_loader.h"
// include the 'fast_obj' header file --> object loading
#include "dependencies/fast_obj/fast_obj.h"
// include C's standard library
#include <stdlib.h>

// WARNING: 'fast_obj' arrays are 1-index based!

int obj_load(const char *path, mesh_t *out_mesh) {
  // function to load model file and upload them to the GPU

  // load the actual `.obj` file from the file path
  fastObjMesh *obj = fast_obj_read(path);

  // check if we are able to actually load the model at that path
  if (!obj) {
    return 0;
  }

  // calculate the total number of indices
  // each face contributes ( face_vertices - 2 ) triangles via fan triangulation
  // a triangle ( 3 verts ) --> 1 triangle  --> 3 indices
  // a quad     ( 4 verts ) --> 2 triangles --> 6 indices
  uint32_t index_count = 0;
  for (uint32_t i = 0; i < obj->face_count; i++) {
    index_count += (obj->face_vertices[i] - 2) * 3;
  }

  // allocate CPU side arrays
  // NOTE: every index gets its own vertex ( no de-duplication for simplicity )
  uint32_t vertex_count = index_count;
  vertex_t *vertices = malloc(sizeof(vertex_t) * vertex_count);
  uint32_t *indices = malloc(sizeof(uint32_t) * index_count);

  // if either allocation failed, clean up and return failure
  if (!vertices || !indices) {
    free(vertices);
    free(indices);
    fast_obj_destroy(obj);
    return 0;
  }

  // convert fast_obj's separate arrays into our interleaved vertex_t array
  uint32_t current = 0;
  uint32_t index_offset = 0;

  for (uint32_t i = 0; i < obj->face_count; i++) {
    uint32_t face_verts = obj->face_vertices[i];

    // triangulate the face using fan triangulation
    // anchor at vertex 0, then walk ( 1,2 ), ( 2,3 ), ( 3,4 ) etc.
    // works correctly for triangles and quads
    for (uint32_t j = 1; j + 1 < face_verts; j++) {
      // get the three fastObjIndex entries for this triangle
      // each fastObjIndex has .p ( position ), .n ( normal ), .t ( texcoord )
      fastObjIndex i0 = obj->indices[index_offset + 0];
      fastObjIndex i1 = obj->indices[index_offset + j];
      fastObjIndex i2 = obj->indices[index_offset + j + 1];

      // vertex 0 of this triangle
      // fast_obj indices start at 1, multiply by 3 to get flat array offset
      vertices[current].position[0] = obj->positions[i0.p * 3 + 0];
      vertices[current].position[1] = obj->positions[i0.p * 3 + 1];
      vertices[current].position[2] = obj->positions[i0.p * 3 + 2];
      // only copy normal if present ( index 0 means not present )
      if (i0.n) {
        vertices[current].normal[0] = obj->normals[i0.n * 3 + 0];
        vertices[current].normal[1] = obj->normals[i0.n * 3 + 1];
        vertices[current].normal[2] = obj->normals[i0.n * 3 + 2];
      }
      indices[current] = current;
      current++;

      // vertex 1 of this triangle
      vertices[current].position[0] = obj->positions[i1.p * 3 + 0];
      vertices[current].position[1] = obj->positions[i1.p * 3 + 1];
      vertices[current].position[2] = obj->positions[i1.p * 3 + 2];
      if (i1.n) {
        vertices[current].normal[0] = obj->normals[i1.n * 3 + 0];
        vertices[current].normal[1] = obj->normals[i1.n * 3 + 1];
        vertices[current].normal[2] = obj->normals[i1.n * 3 + 2];
      }
      indices[current] = current;
      current++;

      // vertex 2 of this triangle
      vertices[current].position[0] = obj->positions[i2.p * 3 + 0];
      vertices[current].position[1] = obj->positions[i2.p * 3 + 1];
      vertices[current].position[2] = obj->positions[i2.p * 3 + 2];
      if (i2.n) {
        vertices[current].normal[0] = obj->normals[i2.n * 3 + 0];
        vertices[current].normal[1] = obj->normals[i2.n * 3 + 1];
        vertices[current].normal[2] = obj->normals[i2.n * 3 + 2];
      }
      indices[current] = current;
      current++;
    }

    // advance offset by how many vertices this face had
    // after face 0 ( 3 verts ) offset = 3
    // after face 1 ( 4 verts ) offset = 7 etc.
    index_offset += face_verts;
  }

  // fast_obj is no longer needed -- free it before we return
  fast_obj_destroy(obj);

  // fill out the output mesh
  out_mesh->vertices = vertices;
  out_mesh->vertex_count = vertex_count;
  out_mesh->indices = indices;
  out_mesh->index_count = index_count;

  // upload CPU arrays to GPU buffers
  mesh_upload(out_mesh);

  return 1;
}
