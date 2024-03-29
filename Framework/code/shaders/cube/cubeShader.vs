#version 330
in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_Uv;
in int in_texture;

out vec4 vert_Normal;
out vec2 vert_Uv;
flat out int vert_text;
uniform mat4 objMat;
uniform mat4 mv_Mat;
uniform mat4 mvpMat;
void main() {
    gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);
    vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);
    vert_Uv = in_Uv;
    vert_text = in_texture;
}
