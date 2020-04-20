#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalVec;
layout (location = 2) in vec2 textCo;


out vec3 normal;
out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 viewMatrix;
uniform mat4 proMatrix;

void main(){
    gl_Position = proMatrix * viewMatrix * model * vec4(pos.x, pos.y, pos.z, 1);
    normal = mat3(model) * normalVec ;
    FragPos = vec3(model * vec4(pos, 1.0f));
    TexCoords = textCo;
}
