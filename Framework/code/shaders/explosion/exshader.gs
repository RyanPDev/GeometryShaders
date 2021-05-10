#version 330
	layout (triangles) in;
	layout (triangle_strip, max_vertices = 12) out;

	uniform float time;
	uniform float magnitude;
	uniform bool shouldSubdivide;

	in vec2 Uvs[];
	out vec2 TexCoords; 
	vec3 normal;
	vec4 vertices[3];	// 0 = 0-1
				// 1 = 1-2
				// 2 = 2-0

	vec3 GetNormal(vec3 v1, vec3 v2, vec3 v3)
	{
		return normalize(cross(v1 - v2, v3 - v2));
	}

	vec4 explode(vec4 position, vec3 normal)
	{
		vec3 direction = normal * time * magnitude; 

		return position + vec4(direction, 0.0);
	}
	void subdivideTriangle(int vertIdx, int subVertIdx1, int subVertIdx2)
	{
		normal = GetNormal(vec3(gl_in[vertIdx].gl_Position), vec3(vertices[subVertIdx1]), vec3(vertices[subVertIdx2]));

		gl_Position = explode(gl_in[vertIdx].gl_Position, normal);
		TexCoords = Uvs[vertIdx];
		EmitVertex();

		gl_Position = explode(vertices[subVertIdx1], normal);
		TexCoords = (Uvs[vertIdx] + Uvs[int(mod(vertIdx + 1, 3))]) / 2;
		EmitVertex();

		gl_Position = explode(vertices[subVertIdx2], normal);
		TexCoords = (Uvs[vertIdx] + Uvs[int(mod(vertIdx + 2, 3))]) / 2;
		EmitVertex();
		EndPrimitive();
	}

	void explosionAnimation(bool subdivide)
	{
		normal = GetNormal(vec3(gl_in[0].gl_Position), vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position));
	
		if(subdivide)
		{
			// Inner Triangle
			for (int i = 0; i < 3; i++)
			{
				vertices[i] = ((gl_in[i].gl_Position + gl_in[int(mod(i + 1, 3))].gl_Position) / 2) + vec4(normal, 0.0) * 0.005f; // Baricenter vertices
				gl_Position = explode(vertices[i] , normal);
				TexCoords = (Uvs[i] + Uvs[int(mod(i + 1, 3))]) / 2;
				EmitVertex();
			}
			EndPrimitive();

			subdivideTriangle(0,0,2);
			subdivideTriangle(1,1,0);
			subdivideTriangle(2,2,1);
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				gl_Position = explode(gl_in[i].gl_Position, normal);
				TexCoords = Uvs[i];
				EmitVertex();
			}
			EndPrimitive();
		}
	}

	void main() {	
		explosionAnimation(shouldSubdivide);
	}