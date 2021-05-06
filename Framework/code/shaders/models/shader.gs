#version 330 core
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	inc vec2 texCoord[];
	void main() {
	gl_Position = gl_in[0].gl_Position + vec4{10, -10, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{10, 10, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{-10, -10, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{-10, 10, 0, 0};
	EmitVertex();
	EndPrimitive();
	}
}