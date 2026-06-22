// own the function implementation found in sokol's header file
#define SOKOL_APP_IMPL
// Linux: using OpenGL's API to communicate with GPU
#define SOKOL_GLCORE
// NOTE: please read line 1154 of "our" `sokol_app.h` header file
#define SOKOL_NO_ENTRY
// include the actual sokol header file
#include "dependencies/sokol/sokol_app.h"

// function related to `sapp_run` and `sapp_desc`
void init(void) {};
void frame(void) {};
void cleanup(void) {};
void event(const sapp_event *event) {};

// our main function
int main() {
  // initialise our application ==> windowing, GPU setup...
  // INFO: my formatter is formatting weirdly x_x
  sapp_run(&(sapp_desc){// setup main point
                        .init_cb = init,
                        .frame_cb = frame,
                        .cleanup_cb = cleanup,
                        .event_cb = event});

  return 0;
}
