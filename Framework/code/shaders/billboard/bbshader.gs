#version 330
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	float width = 5, height = 6;
	out vec2 texCoord;
	void main() {
	gl_Position = gl_in[0].gl_Position + vec4{width, -height, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{width, height, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{-width, -height, 0, 0};
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4{-width, height, 0, 0};
	EmitVertex();
	EndPrimitive();
	}
}