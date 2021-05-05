#version 330
in vec4 vert_Normal;
in vec2 vert_Uv;
out vec4 out_Color;
uniform mat4 mv_Mat;
uniform vec4 color;
uniform sampler2D diffuseTexture;
void main() {
    out_Color = texture(diffuseTexture, vert_Uv) * vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );
}