#version 440

layout (location = 0) in vec3 pos;

//out vec3 FragPos;

uniform mat4 proMatrix;
uniform mat4 viewMatrix;
uniform mat4 model ;


void main (void) 
{
  gl_Position = proMatrix * viewMatrix * model * vec4(pos, 1.0) ;
  //FragPos = vec3(model * vec4(apos, 1.0f)); ;
}
