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

	int numVertices;

	Shader shader;

public:
	Object(const char*, glm::vec3, glm::vec3, glm::vec3, glm::vec3, const char*, const char*, const char* = nullptr, const char* = nullptr);

	glm::vec3 objectColor;
	glm::vec3 position, rotation, scale;

	void Update();
	void Draw(Light);
	void Draw(float,float ,float,bool,bool);
	void CleanUp();

	std::string GetName() { return name; } //--> Retorna el nom de l'objecte corresponent per poder "printar-lo" per pantalla
};