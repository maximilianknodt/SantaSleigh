#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
// P6 Aufgabe 3 Schritt 3
in vec3 Tangente;
in vec3 Bitangente;

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
// P6 Aufgabe 3 Schritt 3
uniform sampler2D NormalTexture;

// Aufgabe 2
const int MAX_LIGHTS=14;
struct Light {
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;		// Fuer Aufgabe 4 benoetigt
};

uniform Lights {
	int LightCount;
	Light lights[MAX_LIGHTS];
};

/// Eingrenzung des Parameterwertes in das spezifizierte Intervall von 0.0 bis 1.0
float sat(in float a) {
	float lowerLimit = 0.0;
	float upperLimit = 1.0;
	return clamp(a, lowerLimit, upperLimit);
}

/// Berechnung der Licht-Abschwaechung mit zunehmender Entfernung
float calcAttenuation(Light light) {
	float dist = length(light.Position - Position);
	float x = light.Attenuation.x;
	float y = light.Attenuation.y;
	float z = light.Attenuation.z;

	// Attenuation = 1.0 / (Attenuation.x + Attenuation.y * Dist + Attenuation.z * Dist * Dist)
	float attenuation = 1.0 / (x + y * dist + z * dist * dist);

	return attenuation;
}

/// Berechnung der Lichtintensitaet eines Spot Lights
/// Nach: https://learnopengl.com/Lighting/Multiple-lights
float calcIntensity(Light light){
	float innerRadius = cos(light.SpotRadius.x);
	float outerRadius = cos(light.SpotRadius.y);

	// Winkel Theta Berechnung = Winkel zwischen Fragment und Lichtrichtung
	vec3 fragLightDir = normalize(light.Position - Position); 
	float theta = dot(fragLightDir, normalize(-light.Direction));		// negativ -> Vectoren zeigen zum Licht hin

	// Vergleich, ob Theta kleiner als Phi (SpotRadius) ist und somit im Lichtkonus liegt
	// Da Cosinuswerte genutzt werden, Vergleich "umdrehen" -> 0° = Cos von 1
	// Wenn nicht, dann hier "abbrechen"
	if(!(theta > outerRadius)) return 0;

	// Berechnung der weichen Kanten im Bereich zwischen Inner- und OuterRadius
	float epsilon = innerRadius - outerRadius;
	float intensity = sat((theta - outerRadius) / epsilon);

	return intensity;
}

/// Umrechnung vom RGB in XYZ Werte
/// P6 Aufgabe 3 Schritt 4
float rgbToXYZ(float f){
	return (f * 2.0 - 1.0);
}

/// Umrechnung der Normalen vom RGB in den XYZ Raum
/// P6 Aufgabe 3 Schritt 4
vec3 calcNormal(sampler2D normalMap, vec2 texCoords){
	vec3 normalColor = texture(normalMap, texCoords).rgb;
	float x = rgbToXYZ(normalColor.x);
	float y = rgbToXYZ(normalColor.y);
	float z = rgbToXYZ(normalColor.z);

	return vec3(x, y, z);
}

/// Umrechnung der Normalen in den Welt-Raum
/// P6 Aufgabe 3 Schritt 4
vec3 calcNormalWorld(){
	vec3 xyzNormal = calcNormal(NormalTexture, Texcoord);
	// Vorzeichen der Bitangente kann in der Implementierung abweichen
	mat3 matrix = mat3(normalize(Tangente), normalize(-Bitangente), normalize(Normal));

	return (matrix * xyzNormal);
}

/// Berechnung der Fragmentfarbe fuer Direct Lights
vec3 calcLight(Light light) {
	vec4 DiffTex = texture(DiffuseTexture, Texcoord);
	if(DiffTex.a < 0.3f) discard;

	// Aufgabe 3 Schritt 4
	vec3 normalMapsVector = calcNormalWorld();

	// N ist 'Normal', oder wenn die NormalMapTexture genutzt wird 'normalMapsVector'
	vec3 N = normalize(normalMapsVector);
	vec3 L = normalize(light.Position - Position);
	vec3 E = normalize(EyePos - Position);
	//vec3 R = reflect(-L, N);
	// Phong nach Blinn
	vec3 Halfway = normalize(E + L);

	// Phong Components
	vec3 DiffuseComponent = light.Color * DiffuseColor * DiffTex.rgb * sat(dot(N,L));
	vec3 SpecularComponent = light.Color * SpecularColor * pow(sat(dot(Halfway, N)), SpecularExp);
	vec3 AmbientComponent = AmbientColor * DiffTex.rgb;

	// Wert fuer die Abschwaechung / Intensitaet des Lichts
	float variable = 1.0;

	switch(light.Type){
		case 0: {	// Point Light
			variable = calcAttenuation(light);
			break;
		};
		case 1: {	// Direct Light
			break;
		};
		case 2: {	// Spot Light
			variable = calcIntensity(light);	//  * calcAttenuation(light)
			break;
		};
	}
	
	DiffuseComponent *= variable;
	SpecularComponent *= variable;
	AmbientComponent *= variable;

	return (DiffuseComponent + SpecularComponent + AmbientComponent);
}

/// Farbe des Fragmentes berechnen
/// Aufgabe 2
vec4 getFragColor(){
	vec3 Color = vec3(0, 0, 0);

	for(int i = 0; i < LightCount; i++){
		Color += calcLight(lights[i]);
	}

	return vec4(Color, 1);
}

void main() {
	FragColor = getFragColor() ;
}