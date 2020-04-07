#version 400

in vec2 TexCoord;
out vec4 frag_colour;

uniform sampler2D ourTexture ;

void main () 
{
  frag_colour = texture (ourTexture, TexCoord );
}
