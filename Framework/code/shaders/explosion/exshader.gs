#version 330
	layout (triangles) in;
	layout (triangle_strip, max_vertices = 3) out;

	uniform float time;
    in vec2 texCoords[];
	out vec2 TexCoords; 

	vec3 GetNormal()
	{
		vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
		vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
		return normalize(cross(a, b));
	}

	vec4 explode(vec4 position, vec3 normal)
	{
		float magnitude = 10.0;
		vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
		return position + vec4(direction, 0.0);
	}

	void main() {	

	vec3 normal = GetNormal();

	for (int i = 0; i < 3; i++)
	{
		gl_Position = explode(gl_in[i].gl_Position, normal);
		TexCoords = texCoords[i];
		EmitVertex();
	}
    EndPrimitive();
}