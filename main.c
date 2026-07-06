// own the function implementation found in sokol's header file
#define SOKOL_IMPL
// Linux: using OpenGL's API to communicate with GPU
#define SOKOL_GLCORE
// NOTE: please read line 1154 of "our" `sokol_app.h` header file
#define SOKOL_NO_ENTRY
// include the sokol header file --> windowing and events
#include "dependencies/sokol/sokol_app.h"
// include the sokol header file --> simple GPU API wrapper - pixels, rendering
#include "dependencies/sokol/sokol_gfx.h"
// include the sokol header file --> helper functions for 'sokol_gfx.h' file
#include "dependencies/sokol/sokol_glue.h"
// include 'clgm' higly optimsed math library for 2D and 3D stuff
#include "cglm/cglm.h"
// include our newly created 'model.h' file
#include "model.h"
// include our mesh structure for dynamic loading
#include "mesh.h"
// include our object loader
#include "object_loader.h"
// include our shader for our model
#include "model_shader.h"

// path to the model file, set from command-line args before sapp_run
static const char *obj_path;

// state stucture for rendering
static struct {
  // action performed during a render pass
  sg_pass_action pass_action;
  // GPU bindings for drawing --> hold data for buffers, textures and more
  sg_bindings bindings;
  // shader, vertex layout / positioning and render settings
  sg_pipeline pipeline;
  // define our model to be rendered --> triangle transformation for every frame
  model triangle;
  // define our model's mesh --> actual loaded geometry from the `.obj` file
  mesh_t mesh;
} state;

// function related to `sapp_run` and `sapp_desc`
void init(void) {
  // function to handle initialisation for window system and GPU rendering

  // initialise graphics for rendering ==> preparing memory stuff and pipelines
  sg_setup(&(sg_desc){
      // setup the environment ==> see line 5006 in 'sokol_gfx.h' for `struct`
      .environment = sglue_environment()});

  // setup the triangle's model default positioning, rotation and scaling
  state.triangle = model_defaults();

  // load our `.obj` file from the hard drive and turn it into a mesh for the
  // GPU INFO: default is `test.obj` ( Suzanne from Blender )
  if (!obj_load(obj_path, &state.mesh)) {
    // if loading failed, return early as there is nothing to render
    return;
  }

  // bind the GPU buffers from the mesh to the rendering state
  state.bindings.vertex_buffers[0] = state.mesh.vertex_buffer;
  state.bindings.index_buffer = state.mesh.index_buffer;

  // create the pipeline for applying the shaders
  state.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
      // pass in our shader
      .shader = sg_make_shader(model_shader_desc(sg_query_backend())),
      // make the GPU understand our vertex data
      .layout = {.attrs =
                     {
                         // get the actual coordinate position
                         [ATTR_model_position].format = SG_VERTEXFORMAT_FLOAT3,
                         // get the normal direction for lighting
                         [ATTR_model_normal].format = SG_VERTEXFORMAT_FLOAT3,
                     }},
      // tell sokol that we are using 32-bit unsigned integers for our indices
      .index_type = SG_INDEXTYPE_UINT32,
      // enable depth testing so front faces occlude back faces
      .depth =
          {
              // fragments pass if their depth is <= the existing depth value
              .compare = SG_COMPAREFUNC_LESS_EQUAL,
              // write fragment depth to the depth buffer for future comparisons
              .write_enabled = true,
          },
  });

  // update the state
  // INFO: again my formatter is really weird WTF is this?
  state.pass_action =
      (sg_pass_action){              // render pass to have colours
                       .colors[0] = {// clean the screen from whatever we have
                                     .load_action = SG_LOADACTION_CLEAR,
                                     // change the colour
                                     .clear_value = {
                                         // red colour
                                         0.15f,
                                         // green colour
                                         0.15f,
                                         // blue colour
                                         0.15f,
                                         // opacity
                                         1.0f,
                                     }}};
};

void frame(void) {
  // function to display at each render state ==> called once every frame

  // move the triangle model back along the z-axis for the duration of the frame
  // INFO: this makes the model goes back
  // state.triangle.position[2] -= sapp_frame_duration();

  // rotate the triangle model along the y-axis for the duration of the frame
  // INFO: this makes the model rotate
  state.triangle.rotation[1] -= sapp_frame_duration();

  // scale the triangle model along the x-axis for the duration of the frame
  // INFO: this makes the model bigger
  state.triangle.scale[0] += 0.01f * sapp_frame_duration();

  // define our 4x4 matrices for 3D "rendering"
  mat4 model_mat, view_mat, proj_mat;

  // place the object in the middle of our screen
  glm_mat4_identity(model_mat);

  // apply our transformation, rotation and scaling to our model ==> animation
  model_matrix(&state.triangle, model_mat);

  // similarly, we need to do the same thing for our view matrix ==> the camera
  glm_mat4_identity(view_mat);

  /*
   * define the camera position ==> at the origin in the middle of our screen
   * additionally, move it back along the z-axis to be able to see the model
   *
   */
  // INFO: see OpenGL's coordinate system to learn more
  vec3 eye = {0.0f, 10.0f, 40.0f};

  // define the place where the camera is going to be looking at
  /*
   * INFO: need to look at 'z' due to the right-handed nature of OpenGL
   *
   * Picture showing Right-Handed ( and Left Handed ):
   * https://perry.cz/articles/ProjectionMatrix.xhtml
   */
  vec3 center = {0.0f, 0.0f, -1.0f};

  // where does our y-axis is located
  // INFO: in this case its basically like in the image above ( see link )
  // additionally, Minecraft also places its y-axis like a normal human-being!
  vec3 up = {0.0f, 1.0f, 0.0f};

  // place the "camera" at set location on the screen by updating `viewr_matrix`
  glm_lookat(eye, center, up, view_mat);

  // field of view for our "eye"
  float fov = glm_rad(100.0f);

  // get the aspect ratio of the "dynamic" / resize-able window
  float window_width = sapp_widthf();
  float window_height = sapp_heightf();

  float aspect_ratio = window_width / window_height;

  // how "near" a vertex can be before its not seen
  float near_plane = 0.1f;

  // how "far" a vertex can be before its not seen
  float far_plane = 100.0f;

  // convert the 3D world and project it on a 2D screen
  glm_perspective(fov, aspect_ratio, near_plane, far_plane, proj_mat);

  // start the pass to display at each state
  sg_begin_pass(
      &(sg_pass){.action = state.pass_action, .swapchain = sglue_swapchain()});

  // apply the pipeline that we created ==> so as to be able to render shader
  sg_apply_pipeline(state.pipeline);

  // bind the GPU buffer to handle these vertex data
  sg_apply_bindings(&state.bindings);

  // sokol-shdc generated model params through 'model.glsl' uniform
  model_params_t params = {0};

  // combine all the "populated" matrix into one final matrix to pass to shader
  // basically matrix multiplication is going to happen here
  // INFO: multiplies n number of matrices, given array of matrices of length n
  glm_mat4_mulN((mat4 *[]){&proj_mat, &view_mat, &model_mat}, 3, params.mvp);

  // copy the model matrix for normal transformation in the vertex shader
  // INFO: normals must use the model matrix only, not the full MVP
  glm_mat4_copy(model_mat, params.model);

  // apply and use the uniforms so as to pass the data to the GPU
  sg_apply_uniforms(UB_model_params, &SG_RANGE(params));

  // actually render the thing on our screen
  sg_draw(

      // base element
      0,
      // number of indices to draw ( each triangle has 3 indices )
      state.mesh.index_count,
      // number of intances
      1);

  // finish recording commands for this pass
  sg_end_pass();

  // submit / "write" all command to the GPU
  sg_commit();
};

void cleanup(void) {
  // function to cleanup resources at the end of our program

  // free the mesh's CPU arrays and GPU buffers
  mesh_destroy(&state.mesh);

  // shutdown / kill the instance of our sokol graphics
  sg_shutdown();
};

void event(const sapp_event *event) {
  // function to handle event handling like mouse and keyboard

  // INFO: adding this to suppress `clangd` / `make` warnings
  (void)event;
};

// our main function
int main(int argc, char *argv[]) {
  // initialise our application ==> windowing, GPU setup...
  // INFO: my formatter is formatting weirdly x_x

  // use first command-line arg as model path, or default if none given
  obj_path = (argc > 1) ? argv[1] : "assets/models/test.obj";

  sapp_run(&(sapp_desc){// setup main point
                        .init_cb = init,
                        .frame_cb = frame,
                        .cleanup_cb = cleanup,
                        .event_cb = event});

  return 0;
}
