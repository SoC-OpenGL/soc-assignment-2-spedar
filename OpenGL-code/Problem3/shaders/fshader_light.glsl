#version 400 core
out vec4 frag_colour;

uniform vec3 light;
void main(){
    frag_colour = vec4(light, 1.0);
}
