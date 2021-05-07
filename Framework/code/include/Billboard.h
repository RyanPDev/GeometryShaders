#pragma once
#include "Shader.h"

class Billboard
{
private:
	GLuint BillboardVao;
	GLuint textureID;
	GLuint BillboardVbo[1];

	//int texWidth, texHeight, nrChannels;
	//unsigned char* data;

	Shader shader;

public:
	Billboard(glm::vec3, unsigned char*, int, int);

	glm::vec3 vertexPos;

	void Draw();
	void CleanUp();
};