// include our mesh's header file and implement the functions signatures here
#include "mesh.h"
// include C's standard library
#include <stdlib.h>

void mesh_upload(mesh_t *mesh) {
  // function to upload vertex data to the GPU

  // upload / send the actual vertex data / array to the GPU
  mesh->vertex_buffer = sg_make_buffer(&(sg_buffer_desc){
      .data =
          {
              .ptr = mesh->vertices,
              .size = sizeof(vertex_t) * mesh->vertex_count,
          },
  });

  // upload / send the actual index data / array to the GPU
  mesh->index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .data =
          {
              .ptr = mesh->indices,
              .size = sizeof(uint32_t) * mesh->index_count,
          },
  });
}

void mesh_destroy(mesh_t *mesh) {
  // function to delete the resources inside of GPU and then the CPU

  // free up the GPU buffers
  sg_destroy_buffer(mesh->vertex_buffer);
  sg_destroy_buffer(mesh->index_buffer);

  // free CPU vertex and index arrays
  free(mesh->vertices);
  free(mesh->indices);

  // NOTE: set the structure to `0` again to avoid dangling pointers
  *mesh = (mesh_t){0};
}
