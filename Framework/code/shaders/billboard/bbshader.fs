#version 330
	out vec4 FragColor;

	in vec2 texCoord;

	uniform sampler2D diffuseTexture;

	void main(){
		FragColor = texture(diffuseTexture, texCoord) * vec4(1.0,1.0,1.0,1.0);
		if (FragColor.rgb == 1 || FragColor.rgb == 0) {
			discard;
    }
}