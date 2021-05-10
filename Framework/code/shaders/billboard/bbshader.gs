#version 330
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;
	uniform float width;
	uniform float height;
	out vec2 texCoord;
	uniform mat4 mvp;
	uniform mat4 view;
	vec3 viewPos;
	vec3 viewDir;

	void main() {

		vec3 pos = gl_in[0].gl_Position.xyz;
		viewPos = vec3(inverse(view)[3]);
		viewDir = normalize(viewPos - pos);
		vec3 up = vec3(0.0, 1.0, 0.0);
    		vec3 right = normalize(cross(viewDir, up));

		pos -= (right * width/2);
		gl_Position = mvp * vec4(pos, 1.0);
		texCoord = vec2( 0.0, 0.0 );
		EmitVertex();

		pos.y += height;
		gl_Position = mvp * vec4(pos, 1.0);
		texCoord = vec2( 0.0, 1.0 );
		EmitVertex();

		pos += (right * width);
		pos.y -= height;
		gl_Position = mvp * vec4(pos, 1.0);
		texCoord = vec2( 1.0, 0.0 );
		EmitVertex();

		pos.y += height;
		gl_Position = mvp * vec4(pos, 1.0);
		texCoord = vec2( 1.0, 1.0 );
		EmitVertex();

		EndPrimitive();
}