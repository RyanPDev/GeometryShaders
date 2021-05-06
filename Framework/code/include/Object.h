#pragma once
#include "Shader.h"

//Classe "Objecte": Agrupa els atributs necessaris dels objectes que carguem i instanciem a l'escena.
//També conté els seus shaders
class Object
{
private:
	GLuint ObjVao;
	GLuint textureID;
	GLuint ObjVbo[3];

	glm::mat4 objMat;

	int texWidth, texHeight, nrChannels;
	unsigned char* data;
	std::string name;
	glm::vec3 initPos, initRot, initScale;

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;

	Shader shader;

public:
	Object(std::string _path, glm::vec3 _startPos, glm::vec3 _startRot, glm::vec3 _startScale, glm::vec3 _startColor = { 1.f, 0.5f, 0.31f });

	glm::vec3 objectColor;
	glm::vec3 position, rotation, scale;

	void Update();
	void Draw(Light);
	void CleanUp();

	std::string GetName() { return name; } //--> Retorna el nom de l'objecte corresponent per poder "printar-lo" per pantalla
};