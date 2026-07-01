#include "model.h"

#include "cglm/cglm.h"

// model component and intialisation helpers

// create some defaults for the model
model model_defaults() {
  // create our model
  model m = {0};

  // update the scaling of the model
  glm_vec3_add(m.scale, (vec3){1, 1, 1}, m.scale);

  // return the default for the model
  return m;
}

// create default for triangle "animation"
void model_matrix(model *self, mat4 dest) {
  glm_mat4_identity(dest);

  // move the model using the "data" from model's default function
  glm_translate(dest, self->position);

  // setup the rotation for each of the axes that we have

  // x-axis
  glm_rotate(dest, self->rotation[0], (vec3){1.0f, 0.0f, 0.0f});
  // y-axis
  glm_rotate(dest, self->rotation[1], (vec3){0.0f, 1.0f, 0.0f});
  // z-axis
  glm_rotate(dest, self->rotation[2], (vec3){0.0f, 0.0f, 1.0f});

  // set the model's scale using data from the model's default function
  glm_scale(dest, self->scale);
}
