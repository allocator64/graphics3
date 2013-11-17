#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;
uniform vec4 eyePos;
uniform vec4 lightPos;


attribute vec4 a_position;
attribute vec2 a_texcoord;
attribute vec4 a_normcoord;

varying vec2 v_texcoord;

varying	vec3 l;
varying	vec3 v;
varying	vec3 n;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = projection_matrix * (model_view_matrix * a_position - eyePos);

    vec3 p = vec3      ( model_view_matrix * a_position );      // transformed point to world space

    l = normalize ( vec3 ( lightPos ) - p );                    // vector to light source
    v = normalize ( vec3 ( eyePos )   - p );                    // vector to the eye
    n = normalize ( normal_matrix * a_normcoord );              // transformed n

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
}
//! [0]
