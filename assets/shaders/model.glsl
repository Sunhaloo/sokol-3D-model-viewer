// tell sokol-shdc to include cglm's mat4 type in the generated C header
@header #include "cglm/cglm.h"
// map GLSL's mat4 to cglm's mat4 in the generated C struct
@ctype mat4 mat4

/*
   NOTE: some simple notes

   - vertex shader is responsible for placing "data" / coordinates on screen
   - fragment shader is responsible for "colouring" our pixels ( inside the points of data )
   - each shader has its own entry point / `main` function
   - we need to tell the `sokol-shdc` that this is a complete shader program with `@program`
   - unlike `triangle.glsl`, this shader uses normals instead of colour
   - normals are used to calculate lighting in the fragment shader
*/

// beginning of vertex shader
@vs vertex_shader

// vertex shader takes IN a single set of x, y and z coordinates at A time
in vec3 position;
// vertex shader takes IN a single normal direction at A time ==> which way the surface faces
in vec3 normal;

// need to pass the normal to `fragment_shader` ==> `vertex_shader` now has an output
out vec3 out_vertex_shader_normal;

// create uniforms ==> basically global "constants" used by ( vertex ) shader
layout(binding=0) uniform model_params {
  // get our `mvp` matrix found inside our 'main.c' file ( "from the CPU" )
  mat4 mvp;
  // model matrix alone ==> needed to transform normals correctly into world space
  // INFO: we cannot use `mvp` for normals as projection would distort the direction
  mat4 model;
};

// main entry point / main function for vertex shader
void main() {
  // update the position of vertices with our `mvp` matrix
  // NOTE: `gl_Position` is of type `vec4` ==> need to convert `vec3` to `vec4`
  gl_Position = mvp * vec4(position, 1.0f);

  // transform normal into world space using model matrix only
  // NOTE: mat3(model) strips translation ==> normals are directions, not positions
  // therefore translation must NOT affect them
  out_vertex_shader_normal = mat3(model) * normal;
}

// end of vertex shader
@end

// beginning of fragment shader
@fs fragment_shader

// get the normal from our `vertex_shader`
in vec3 out_vertex_shader_normal;

// outputs 'RGBA' value
out vec4 frag_colour;

// main entry point / main function for fragment shader
void main() {
  // define a fixed directional light pointing down and to the right
  // NOTE: normalize() ensures the vector length is exactly 1.0
  vec3 light_dir = normalize(vec3(0.5, 1.0, 0.5));

  // dot product of normal and light direction ==> gives us the diffuse term
  // NOTE: max( 0.0 ) clamps negative values ==> surfaces facing away from light = 0
  float diff = max(dot(normalize(out_vertex_shader_normal), light_dir), 0.0);

  // base colour of the model ==> neutral grey
  vec3 base_colour = vec3(0.8, 0.8, 0.8);

  // ambient ==> minimum light so surfaces facing away are not completely black
  float ambient = 0.2;

  // final colour = base * ( ambient + diffuse contribution )
  vec3 colour = base_colour * (ambient + diff);

  // output as vec4 ==> alpha 1.0 means fully opaque
  frag_colour = vec4(colour, 1.0);
}

// end of fragment shader
@end

// complete shader program with vertex and fragment shader
@program model vertex_shader fragment_shader
