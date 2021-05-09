#version 330
	out vec4 FragColor;
	in vec2 TexCoords;
	uniform sampler2D diffuseTexture;

	void main(){		
		FragColor = texture(diffuseTexture, TexCoords);
	}