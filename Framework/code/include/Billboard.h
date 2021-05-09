#pragma once
#include "Shader.h"

class Billboard
{
private:
	GLuint BillboardVao;
	GLuint textureID;
	GLuint BillboardVbo[1];

	Shader shader;

public:
	Billboard(glm::vec3, unsigned char*, int, int, const char*, const char*, const char* = nullptr);

	glm::vec3 vertexPos;

	void Draw(float, float);
	void CleanUp();
};