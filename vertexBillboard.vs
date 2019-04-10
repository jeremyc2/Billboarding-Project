uniform mat4 u_Matrix;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float scaleFactor;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main()
{
   mat4 ModelView = u_View * u_Model;
   // Column 0:
   ModelView[0][0] = scaleFactor;
   ModelView[0][1] = 0.0;
   ModelView[0][2] = 0.0;

   // Column 1:
   ModelView[1][0] = 0.0;
   ModelView[1][1] = scaleFactor;
   ModelView[1][2] = 0.0;

   // Column 2:
   ModelView[2][0] = 0.0;
   ModelView[2][1] = 0.0;
   ModelView[2][2] = scaleFactor;

   v_UV = a_UV;
   v_Normal = vec3(ModelView * vec4(a_Normal,1.0));
   v_Position = vec3(ModelView * vec4(a_Position,1.0));

   //gl_Position = ModelView * vec4(a_Position, 1.0);
   //gl_Position /= gl_Position.w;
   //gl_Position.xy += a_Position.xy * vec2(0.2,0.05);
    gl_Position = u_Projection * ModelView * vec4(a_Position, 1.0);
}