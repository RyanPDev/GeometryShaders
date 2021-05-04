#pragma once
#include <iostream>
#include <fstream>
#include <GL\glew.h>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>

class Shader
{
public:
	GLuint programID;
	GLuint ObjShader[2];

	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	//Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	/*~Shader();*/

	GLuint CompileShader(const char*, GLenum, const char*);
	void LinkProgram(GLuint);
	void Use();
	void CleanUpShader();
	void SetBool(const std::string&, bool) const;
	void SetInt(const std::string&, int) const;
	void SetFloat(const std::string&, float) const;
	void SetFloat3(const std::string&, glm::vec3) const;
	void SetMat4(const std::string& name, int size, bool transpose, float* value) const;
};