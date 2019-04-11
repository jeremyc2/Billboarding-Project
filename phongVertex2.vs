uniform mat4 u_Matrix;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float frequency;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main (){

    float x = (sin(a_Position.y * frequency) * .01) + a_Position.x;
    vec3 position = vec3(x,a_Position.y,a_Position.z);

    v_UV = a_UV;
    v_Position = vec3(u_Model * vec4 (position, 1.0));
    v_Normal = vec3 (u_Model * vec4 (a_Normal, 1.0));

    gl_Position = u_Matrix * vec4 (position, 1.0);
}