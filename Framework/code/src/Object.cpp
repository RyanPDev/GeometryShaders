#include "Object.h"

// Declaració de la funció del load_obj.cpp que serveix per cargar els vertexs, uvs i normals dels models que importem
extern bool loadOBJ(const char* path, std::vector < glm::vec3 >& out_vertices, std::vector < glm::vec2 >& out_uvs, std::vector < glm::vec3 >& out_normals);

Object::Object(const char* _objPath, glm::vec3 _startPos, glm::vec3 _startRot, glm::vec3 _startScale, glm::vec3 _startColor,
	const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* texturePath) :
	name(_objPath), position(_startPos), rotation(_startRot), scale(_startScale), objectColor(_startColor), initPos(_startPos), initRot(_startRot), initScale(_startScale)
{
	bool res = loadOBJ(_objPath, vertices, uvs, normals);
	data = stbi_load(texturePath, &texWidth, &texHeight, &nrChannels, 0); //--> Carreguem textura

	numVertices = vertices.size();

	// Cambiem l'string del path que rebem de l'objecte per deixar només visible el seu nom per després fer-ho servir al ImGui
	name.erase(name.size() - 4, name.size());
	name.erase(name.begin(), name.begin() + 4);

	glGenVertexArrays(1, &ObjVao);
	glBindVertexArray(ObjVao);
	glGenTextures(1, &textureID); //TEXTURES
	glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //TEXTURES
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "Failed to load texture" << std::endl;

	stbi_image_free(data); //--> Alliberem memòria de les textures

	glGenBuffers(3, ObjVbo);

	glBindBuffer(GL_ARRAY_BUFFER, ObjVbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ObjVbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ObjVbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Es crida un constructor de shader o un altre depenent de si s'ha passat el path d'un geometry shader al constructor de l'objecte
	geometryPath == nullptr ? shader = Shader(vertexPath, fragmentPath) : shader = Shader(vertexPath, fragmentPath, geometryPath);

	glBindAttribLocation(shader.GetID(), 0, "aPos");
	glBindAttribLocation(shader.GetID(), 1, "aUvs");
	glBindAttribLocation(shader.GetID(), 2, "aNormal");

	// Allibera memoria de la cpu
	vertices.clear();
	normals.clear();
	uvs.clear();
}


void Object::Update()
{
	glm::mat4 t = glm::translate(glm::mat4(), position);
	glm::mat4 r1 = glm::rotate(glm::mat4(), rotation.x, glm::vec3(1, 0, 0));
	glm::mat4 r2 = glm::rotate(glm::mat4(), rotation.y, glm::vec3(0, 1, 0));
	glm::mat4 r3 = glm::rotate(glm::mat4(), rotation.z, glm::vec3(0, 0, 1));
	glm::mat4 s = glm::scale(glm::mat4(), scale);
	objMat = t * r1 * r2 * r3 * s;
}

void Object::Draw(Light light)
{
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(ObjVao);
	shader.SetMat4("model", 1, GL_FALSE, glm::value_ptr(objMat));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	shader.SetMat4("projection", 1, GL_FALSE, glm::value_ptr(RenderVars::_projection));
	shader.SetFloat3("objectColor", objectColor);
	shader.SetFloat3("lightColor", light.color);
	shader.SetFloat3("lightPos", light.position);
	shader.SetFloat3("spotLightDir", light.spotLightDirection);
	shader.SetFloat("lightIntensity", light.intensity);
	shader.SetInt("attenuationActive", light.attenuationActivated);
	shader.SetInt("lightType", (int)light.type);
	shader.SetFloat("constant", light.constant);
	shader.SetFloat("linear", light.linear);
	shader.SetFloat("quadratic", light.quadratic);
	shader.SetFloat("cutOff", light.cutOff);
	shader.SetFloat("ambientStrength", light.ambientIntensity);
	shader.SetFloat3("ambientColor", light.ambientColor);
	shader.SetFloat("diffuseStrength", light.diffuseIntensity);
	shader.SetFloat("specularStrength", light.specularIntensity);
	shader.SetFloat3("specularColor", light.specularColor);
	shader.SetFloat("shininessValue", light.shininessValue);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glUseProgram(0);
	glBindVertexArray(0);
}

void Object::Draw(float currentTime, float auxTime, float magnitude, bool startAnimation, bool shouldSubdivide)
{
	if (startAnimation) currentTime = (sin(ImGui::GetTime() - auxTime) + 1.0) / 2.0;

	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(ObjVao);
	shader.SetMat4("model", 1, GL_FALSE, glm::value_ptr(objMat));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	shader.SetMat4("projection", 1, GL_FALSE, glm::value_ptr(RenderVars::_projection));

	shader.SetFloat("time", currentTime);
	shader.SetFloat("magnitude", magnitude);
	shader.SetBool("shouldSubdivide", shouldSubdivide);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glUseProgram(0);
	glBindVertexArray(0);
}

void Object::CleanUp()
{
	glDeleteBuffers(3, ObjVbo);
	glDeleteVertexArrays(1, &ObjVao);
	shader.CleanUpShader();

	glDeleteTextures(1, &textureID);
}