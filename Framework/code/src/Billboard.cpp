#include "Billboard.h"

Billboard::Billboard(glm::vec3 _vertexPos, unsigned char* _data, int width, int height) : vertexPos(_vertexPos)
{
	//data = stbi_load("materials/tree_texture.png", &texWidth, &texHeight, &nrChannels, 0);

	glGenVertexArrays(1, &BillboardVao);
	glBindVertexArray(BillboardVao);
	glGenTextures(1, &textureID); //TEXTURES
	glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

	if (_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data); //TEXTURES
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "Failed to load texture" << std::endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenBuffers(1, BillboardVbo);

	glBindBuffer(GL_ARRAY_BUFFER, BillboardVbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &vertexPos, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	shader = Shader("shaders/billboard/bbshader.vs", "shaders/billboard/bbshader.fs", "shaders/billboard/bbshader.gs");
}

void Billboard::Draw()
{
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(BillboardVao);

	shader.SetMat4("mvp", 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	glDrawArrays(GL_POINTS, 0, 1);
	glUseProgram(0);
	glBindVertexArray(0);
}

void Billboard::CleanUp()
{
	glDeleteBuffers(1, BillboardVbo);
	glDeleteVertexArrays(1, &BillboardVao);
	shader.CleanUpShader();

	glDeleteTextures(1, &textureID);
}