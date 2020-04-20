#version 400

in vec3 TexCoord;
out vec4 frag_colour;

uniform samplerCube skybox ;

void main () 
{
  frag_colour = texture (skybox, TexCoord ) ;
}
