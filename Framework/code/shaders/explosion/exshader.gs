#version 330
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	float width = 5, height = 6;
	out vec2 texCoord;
	uniform mat4 mvp;
	uniform mat4 view;
	vec3 viewPos;
	vec3 viewDir;
	uniform float time;

	vec3 GetNormal()
	{
		vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
		vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
		return normalize(cross(a, b));
	}

	vec4 explode(vec4 position, vec3 normal)
	{
		float magnitude = 2.0;
		vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
		return position + vec4(direction, 0.0);
	}

	void main() {	

	vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}