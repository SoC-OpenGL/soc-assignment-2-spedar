#version 440

layout (location = 0) in vec3 tpos;

out vec3 TexCoord;

uniform mat4 proMatrix;
uniform mat4 viewMatrix;


void main (void) 
{
  vec4 pos = proMatrix * viewMatrix * vec4(tpos, 1.0) ;
  TexCoord = tpos;
  gl_Position = pos.xyww ;
}
