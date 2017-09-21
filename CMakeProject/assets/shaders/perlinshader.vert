#version 330 core
//Vertex Attribute
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;	//Wird erst beim Normal Mapping spaeter wichtig. Der OBJLoader liefert das einfach mit.

//world, view und projection matrix
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

//Richtung unserer direktionalen Lichtquelle in Weltkoordinaten
uniform vec3 light_dir;

uniform sampler2D perlinTex;
uniform float u_time;

//Daten fuer den Fragment Shader
out struct VertexData
{
	vec2 uv;
	vec3 normal;
} vertexdat;

void main()
{
	vec3 pos = position;
	vec2 offset1 = vec2(1.0, 0.5)  *  u_time * 0.07;
	vec2 offset2 = vec2(0.5, 1.0)  * u_time * 0.07;
	float height1 = texture(perlinTex, uv + offset1).r * 0.3;
	float height2 = texture(perlinTex, uv + offset2).r * 0.3;
	pos.y += height1 + height2;
	vec4 mvPosition = model_mat * vec4(pos, 1.0);
	gl_Position = proj_mat * view_mat * mvPosition;

	//uv Koordinate setzen
	vertexdat.uv = uv;
	//Normale in Viewspace transformieren
	vertexdat.normal = normalize((transpose(inverse(view_mat * model_mat)) * vec4(normal, 0.0)).xyz);
}