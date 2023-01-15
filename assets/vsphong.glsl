#version 400
layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;
// P6 Aufgabe 3 Schritt 2
layout(location=3) in vec3 VertexTangente;
layout(location=4) in vec3 VertextBitangente;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;
// P6 Aufgabe 3 Schritt 2
out vec3 Tangente;
out vec3 Bitangente;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;

void main() {
    Position = (ModelMat * VertexPos).xyz;
    Normal = (ModelMat * vec4(VertexNormal.xyz, 0)).xyz;
    // P6 Aufgabe 3 Schritt 2
    Tangente = (ModelMat * vec4(VertexTangente.xyz, 0)).xyz;
    Bitangente = (ModelMat * vec4(VertextBitangente.xyz, 0)).xyz;

    Texcoord = VertexTexcoord;
    gl_Position = ModelViewProjMat * VertexPos;
}

