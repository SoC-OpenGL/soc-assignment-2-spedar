#version 440

layout (location = 0) in vec3 vp;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 proMatrix;
uniform mat4 viewMatrix;


void main (void) 
{
  gl_Position = proMatrix * viewMatrix * vec4(vp, 1.0) ;
  TexCoord = aTexCoord;
}
