#include "Shader.h"

Shader::Shader() : ObjShader{ 0, 0, 0 }, programID(0) {}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// 1. Llegim els shaders desde un arxiu extern
	std::string vshader = ReadShaderFromFile(vertexPath);
	std::string fshader = ReadShaderFromFile(fragmentPath);
	const char* vShaderCode = vshader.c_str();
	const char* fShaderCode = fshader.c_str();

	// 2. Compilem els shaders
	ObjShader[0] = CompileShaderFromFile(vShaderCode, GL_VERTEX_SHADER, "ObjVert"); // Vertex Shader
	ObjShader[1] = CompileShaderFromFile(fShaderCode, GL_FRAGMENT_SHADER, "ObjFrag"); // Fragment Shader

	// 3. Creem el programa dels shaders
	programID = glCreateProgram();
	glAttachShader(programID, ObjShader[0]);
	glAttachShader(programID, ObjShader[1]);

	if (geometryPath != nullptr) // Geometry Shader
	{
		std::string gshader = ReadShaderFromFile(geometryPath);
		const char* gShaderCode = gshader.c_str();
		ObjShader[2] = CompileShaderFromFile(gShaderCode, GL_GEOMETRY_SHADER, "ObjGeom");
		glAttachShader(programID, ObjShader[2]);
	}

	LinkProgram(programID);
}

//Codi de lectura de shaders externs a partir del codi proposat per https://learnopengl.com/Getting-started/Shaders //
std::string Shader::ReadShaderFromFile(const char* shaderPath)
{	
	std::string shaderCode;
	std::ifstream shaderFile;

	// Ens asegurem de que el ifstream pugui llençar les excepcions
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// obrir arxius
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;

		// llegir el contingut del buffer i enviar ho al stream
		shaderStream << shaderFile.rdbuf();

		// Tanquem el handler del arxiu
		shaderFile.close();

		// convertim el stream en una string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	return shaderCode;
}

GLuint Shader::CompileShaderFromFile(const char* shaderStr, GLenum shaderType, const char* name)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void Shader::LinkProgram(GLuint program)
{
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

void Shader::Use()
{
	glUseProgram(programID);
}

void Shader::CleanUpShader()
{
	glDeleteProgram(programID);
	glDeleteShader(ObjShader[0]);
	glDeleteShader(ObjShader[1]);
}

#pragma region Uniform Setters

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::SetFloat3(const std::string& name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z);
}

void Shader::SetMat4(const std::string& name, int size, bool transpose, float* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), size, transpose, value);
}

#pragma endregion