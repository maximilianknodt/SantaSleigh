#version 400
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main(){
    // Da Daten der Textur nur in der Red-component gespeichert, dass nur nutzen
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, Textcoords).r);
    // vec4 mit textColor, um unterschiedliche Farbe zu erhalten
    color = vec4(textColor, 1.0) * sampled;
}