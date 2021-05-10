#version 330
in vec4 vert_Normal;
in vec2 vert_Uv;
flat in int vert_text;
out vec4 out_Color;
uniform mat4 mv_Mat;
uniform sampler2D text0;
uniform sampler2D text1;
uniform sampler2D text2;
uniform sampler2D text3;
uniform sampler2D text4;
uniform sampler2D text5;
void main() {
    switch(vert_text)
    {
        case 1:
            out_Color = texture(text0, vert_Uv);
            break;
        case 2:
            out_Color = texture(text1, vert_Uv);
            break;
        case 3:
            out_Color = texture(text2, vert_Uv);
            break;
        case 4:
            out_Color = texture(text3, vert_Uv);
            break;
        case 5:
            out_Color = texture(text4, vert_Uv);
            break;
        case 6:
            out_Color = texture(text5, vert_Uv);
            break;
        default:
            break;
    }
}