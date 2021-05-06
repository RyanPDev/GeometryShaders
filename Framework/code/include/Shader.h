#pragma once
#include <iostream>
#include <fstream>
#include <GL\glew.h>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>

class Shader
{
private:
	GLuint ObjShader[3];

	GLuint CompileShaderFromFile(const char*, GLenum, const char* = "");
	void LinkProgram(GLuint);
	std::string ReadShaderFromFile(const char*);
public:
	GLuint programID;

	Shader();
	Shader(const char*, const char*, const char* = nullptr);

	void Use();
	void CleanUpShader();
	void SetBool(const std::string&, bool) const;
	void SetInt(const std::string&, int) const;
	void SetFloat(const std::string&, float) const;
	void SetFloat3(const std::string&, glm::vec3) const;
	void SetMat4(const std::string& name, int size, bool transpose, float* value) const;
};