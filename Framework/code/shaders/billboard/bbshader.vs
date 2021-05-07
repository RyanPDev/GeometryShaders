#version 330
	in vec3 in_Position;
	uniform mvpMat;
	void main(){
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);
}