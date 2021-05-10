#include "Billboard.h"

Billboard::Billboard(glm::vec3 _vertexPos, unsigned char* _data, int width, int height, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
	: vertexPos(_vertexPos)
{
	glGenVertexArrays(1, &BillboardVao);
	glBindVertexArray(BillboardVao);
	glGenTextures(1, &textureID); //TEXTURES
	glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (_data) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data); //TEXTURES
	else std::cout << "Failed to load texture" << std::endl;

	glGenBuffers(1, BillboardVbo);

	glBindBuffer(GL_ARRAY_BUFFER, BillboardVbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &vertexPos, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	shader = Shader(vertexPath, fragmentPath, geometryPath);
}

void Billboard::Draw(float _width, float _height)
{
	shader.Use(); //--> Activem shader

	//Activem i bindejem textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(BillboardVao);

	//Variables que pasem als shaders com a uniforms per ser usats per la gràfica
	shader.SetMat4("mvp", 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	shader.SetFloat("width", _width);
	shader.SetFloat("height", _height);

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