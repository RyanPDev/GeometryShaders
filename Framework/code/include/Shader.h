#pragma once
#include "Utils.h"

class Shader
{
private:
	GLuint ObjShader[3];
	GLuint programID;

	GLuint CompileShaderFromFile(const char*, GLenum, const char* = "");
	void LinkProgram(GLuint);
	std::string ReadShaderFromFile(const char*);

public:
	Shader();

	// Fem que el shader del geometry sigui igual a nullptr per donar-li 
	// un valor per defecte i no haver de fer un overwrite
	Shader(const char*, const char*, const char* = nullptr); 

	void Use();
	void CleanUpShader();
	void SetBool(const std::string&, bool) const;
	void SetInt(const std::string&, int) const;
	void SetFloat(const std::string&, float) const;
	void SetFloat3(const std::string&, glm::vec3) const;
	void SetMat4(const std::string& name, int size, bool transpose, float* value) const;

	GLuint GetID() { return programID; }
};