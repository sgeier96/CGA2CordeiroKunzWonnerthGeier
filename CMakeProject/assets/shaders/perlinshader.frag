#version 330 core

//Richtung der direktionalen Lichtquelle
uniform vec3 lightDir;
//Lichtfarbe
uniform vec3 light_color;
//Textur
uniform sampler2D tex;

//Eingabe vom VertexShader
in struct VertexData
{
	vec2 uv;
	vec3 normal;
} vertexdat;

//Output color
out vec4 color;

void main()
{
	//Textur samplen.
	vec4 texcol = texture(tex, vertexdat.uv);
	//Diffusen term berechnen
	float diffterm = max(dot(-lightDir, vertexdat.normal), 0.0f);

	//Outpur Farbe berechnen.
	color = ((diffterm + 0.4) * vec4(light_color, 1.0f)) * texcol;
}