#version 400

layout(location=0) in vec4 VertexPos;
layout(location=1) in vec4 VertexNormal;
layout(location=2) in vec2 VertexTexcoord;

uniform mat4 ModelMat;
uniform mat4 ModelViewProjMat;

uniform float Amplitude;
uniform float Frequency;
uniform float Time;

out vec3 Position;
out vec3 Normal;
out vec2 Texcoord;

void main()
{
    float y = sin(Frequency * (VertexPos.x + Time)) * Amplitude;
    vec4 newVertex = vec4(VertexPos.x, y, VertexPos.z, 1.0);
    Position = (ModelMat * VertexPos).xyz;
    Normal =  (ModelMat * VertexNormal).xyz;
    Texcoord = VertexTexcoord;
    gl_Position = ModelViewProjMat * VertexPos;
}