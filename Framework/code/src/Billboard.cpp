#include "Billboard.h"

Billboard::Billboard(glm::vec3 _vertexPos) : vertexPos(_vertexPos)
{
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("cat_texture.jpg", &texWidth, &texHeight, &nrChannels, 0);

	glGenVertexArrays(1, &BillboardVao);
	glBindVertexArray(BillboardVao);
	glGenTextures(1, &textureID); //TEXTURES
	glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //TEXTURES
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "Failed to load texture" << std::endl;

	stbi_image_free(data);

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
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);*/
	glBindVertexArray(BillboardVao);

	shader.SetMat4("mvpMat", 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));

	glDrawArrays(GL_POINTS, 0, 4);
	glUseProgram(0);
	glBindVertexArray(0);
}