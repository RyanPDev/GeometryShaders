#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include <vector>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>

#include "GL_framework.h"
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//Declaració de la funció del load_obj.cpp que serveix per cargar els vertexs, uvs i normals dels models que importem
extern bool loadOBJ(const char* path, std::vector < glm::vec3 >& out_vertices, std::vector < glm::vec2 >& out_uvs, std::vector < glm::vec3 >& out_normals);

//Struct de la llum, guarda tots els atributs que tenen els diferents tipus de llums.
struct Light {

	enum class EType
	{
		DIRECTIONAL = 1, POINTLIGHT, SPOTLIGHT, COUNT
	} type;

	glm::vec3 position = { 0.f, 1.f, 0.f };
	glm::vec3 color = { 1.f, 1.f, 1.f };
	glm::vec3 ambientColor = { 1.f, 1.f, 1.f };
	glm::vec3 specularColor = { 1.f, 1.f, 1.f };
	glm::vec3 spotLightDirection = { 0.f,1.f,0.f };
	float intensity = 1.f;
	float ambientIntensity = 0.1f;
	float diffuseIntensity = 1.f;
	float specularIntensity = 1.f;
	float shininessValue = 32.f;
	float constant = 1.f;
	float linear = 0.07f;
	float quadratic = 0.017f;
	float spotLightAngle = 20.f;
	float cutOff = glm::cos(glm::radians(20.f));
	int attenuationActivated = 1;
} light;

//Namespace dels atributs necessaris per a fer l'animació del Dolly Effect
namespace DollyEffect {
	enum class State { START, BACKING, FORWADING, END, COUNT }eDollyState;

	bool animationStarted = false;
	int objectSelected = 0;
	float width = 0;
	float speed = 50;
	float zoom = 0;
	float ZDistance = 7.f;
	bool dollyScene = false;
	float initialDistance = 0, animationDistance = 40.f;
}
namespace Dolly = DollyEffect;

///////// fw decl
namespace ImGui {
	void Render();
}
//namespace Axis {
//	void setupAxis();
//	void cleanupAxis();
//	void drawAxis();
//}
////////////////

namespace RenderVars {
	float FOV = glm::radians(90.f);
	const float zNear = 1.f;
	const float zFar = 5000.f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, -5.f, -15.f };
	float rota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

void GLResize(int width, int height) {
	glViewport(0, 0, width, height);
	if (height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);
}

void GLmousecb(MouseEvent ev) {
	if (RV::prevMouse.waspressed && RV::prevMouse.button == ev.button && !Dolly::animationStarted) {
		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;
		switch (ev.button) {
		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;
		}
	}
	else {
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}
	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

//////////////////////////////////////////////////
//GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
//	GLuint shader = glCreateShader(shaderType);
//	glShaderSource(shader, 1, &shaderStr, NULL);
//	glCompileShader(shader);
//	GLint res;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
//	if (res == GL_FALSE) {
//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
//		char* buff = new char[res];
//		glGetShaderInfoLog(shader, res, &res, buff);
//		fprintf(stderr, "Error Shader %s: %s", name, buff);
//		delete[] buff;
//		glDeleteShader(shader);
//		return 0;
//	}
//	return shader;
//}

//void linkProgram(GLuint program) {
//	glLinkProgram(program);
//	GLint res;
//	glGetProgramiv(program, GL_LINK_STATUS, &res);
//	if (res == GL_FALSE) {
//		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
//		char* buff = new char[res];
//		glGetProgramInfoLog(program, res, &res, buff);
//		fprintf(stderr, "Error Link: %s", buff);
//		delete[] buff;
//	}
//}

////////////////////////////////////////////////// AXIS
//namespace Axis {
//	GLuint AxisVao;
//	GLuint AxisVbo[3];
//	GLuint AxisShader[2];
//	GLuint AxisProgram;
//
//	float AxisVerts[] = {
//		0.0, 0.0, 0.0,
//		1.0, 0.0, 0.0,
//		0.0, 0.0, 0.0,
//		0.0, 1.0, 0.0,
//		0.0, 0.0, 0.0,
//		0.0, 0.0, 1.0
//	};
//	float AxisColors[] = {
//		1.0, 0.0, 0.0, 1.0,
//		1.0, 0.0, 0.0, 1.0,
//		0.0, 1.0, 0.0, 1.0,
//		0.0, 1.0, 0.0, 1.0,
//		0.0, 0.0, 1.0, 1.0,
//		0.0, 0.0, 1.0, 1.0
//	};
//	GLubyte AxisIdx[] = {
//		0, 1,
//		2, 3,
//		4, 5
//	};
//	const char* Axis_vertShader =
//		"#version 330\n\
//in vec3 in_Position;\n\
//in vec4 in_Color;\n\
//out vec4 vert_color;\n\
//uniform mat4 mvpMat;\n\
//void main() {\n\
//	vert_color = in_Color;\n\
//	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
//}";
//	const char* Axis_fragShader =
//		"#version 330\n\
//in vec4 vert_color;\n\
//out vec4 out_Color;\n\
//void main() {\n\
//	out_Color = vert_color;\n\
//}";
//
//	void setupAxis() {
//		glGenVertexArrays(1, &AxisVao);
//		glBindVertexArray(AxisVao);
//		glGenBuffers(3, AxisVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisVerts, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisColors, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AxisVbo[2]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, AxisIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		AxisShader[0] = compileShader(Axis_vertShader, GL_VERTEX_SHADER, "AxisVert");
//		AxisShader[1] = compileShader(Axis_fragShader, GL_FRAGMENT_SHADER, "AxisFrag");
//
//		AxisProgram = glCreateProgram();
//		glAttachShader(AxisProgram, AxisShader[0]);
//		glAttachShader(AxisProgram, AxisShader[1]);
//		glBindAttribLocation(AxisProgram, 0, "in_Position");
//		glBindAttribLocation(AxisProgram, 1, "in_Color");
//		linkProgram(AxisProgram);
//	}
//	void cleanupAxis() {
//		glDeleteBuffers(3, AxisVbo);
//		glDeleteVertexArrays(1, &AxisVao);
//
//		glDeleteProgram(AxisProgram);
//		glDeleteShader(AxisShader[0]);
//		glDeleteShader(AxisShader[1]);
//	}
//	void drawAxis() {
//		glBindVertexArray(AxisVao);
//		glUseProgram(AxisProgram);
//		glUniformMatrix4fv(glGetUniformLocation(AxisProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
//		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//	}
//}
//
//////////////////////////////////////////////////// CUBE
//namespace Cube {
//	GLuint cubeVao;
//	GLuint cubeVbo[3];
//	GLuint cubeShaders[2];
//	GLuint cubeProgram;
//	Shader cubeShader;
//	glm::mat4 objMat = glm::mat4(1.f);
//
//	extern const float halfW = 0.5f;
//	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
//
//						   //   4---------7
//						   //  /|        /|
//						   // / |       / |
//						   //5---------6  |
//						   //|  0------|--3
//						   //| /       | /
//						   //|/        |/
//						   //1---------2
//	glm::vec3 verts[] = {
//		glm::vec3(-halfW, -halfW, -halfW),
//		glm::vec3(-halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW,  halfW),
//		glm::vec3(halfW, -halfW, -halfW),
//		glm::vec3(-halfW,  halfW, -halfW),
//		glm::vec3(-halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW,  halfW),
//		glm::vec3(halfW,  halfW, -halfW)
//	};
//	glm::vec3 norms[] = {
//		glm::vec3(0.f, -1.f,  0.f),
//		glm::vec3(0.f,  1.f,  0.f),
//		glm::vec3(-1.f,  0.f,  0.f),
//		glm::vec3(1.f,  0.f,  0.f),
//		glm::vec3(0.f,  0.f, -1.f),
//		glm::vec3(0.f,  0.f,  1.f)
//	};
//
//	glm::vec3 cubeVerts[] = {
//		verts[1], verts[0], verts[2], verts[3],
//		verts[5], verts[6], verts[4], verts[7],
//		verts[1], verts[5], verts[0], verts[4],
//		verts[2], verts[3], verts[6], verts[7],
//		verts[0], verts[4], verts[3], verts[7],
//		verts[1], verts[2], verts[5], verts[6]
//	};
//	glm::vec3 cubeNorms[] = {
//		norms[0], norms[0], norms[0], norms[0],
//		norms[1], norms[1], norms[1], norms[1],
//		norms[2], norms[2], norms[2], norms[2],
//		norms[3], norms[3], norms[3], norms[3],
//		norms[4], norms[4], norms[4], norms[4],
//		norms[5], norms[5], norms[5], norms[5]
//	};
//	GLubyte cubeIdx[] = {
//		0, 1, 2, 3, UCHAR_MAX,
//		4, 5, 6, 7, UCHAR_MAX,
//		8, 9, 10, 11, UCHAR_MAX,
//		12, 13, 14, 15, UCHAR_MAX,
//		16, 17, 18, 19, UCHAR_MAX,
//		20, 21, 22, 23, UCHAR_MAX
//	};
//
//	const char* cube_vertShader =
//	"#version 330\n\
//	in vec3 in_Position;\n\
//	in vec3 in_Normal;\n\
//	out vec4 vert_Normal;\n\
//	uniform mat4 objMat;\n\
//	uniform mat4 mv_Mat;\n\
//	uniform mat4 mvpMat;\n\
//	void main() {\n\
//		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
//		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
//	}";
//
//	const char* cube_fragShader =
//		"#version 330\n\
//in vec4 vert_Normal;\n\
//out vec4 out_Color;\n\
//uniform mat4 mv_Mat;\n\
//uniform vec4 color;\n\
//void main() {\n\
//	out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
//}";
//	void setupCube() {
//		glGenVertexArrays(1, &cubeVao);
//		glBindVertexArray(cubeVao);
//		glGenBuffers(3, cubeVbo);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		glPrimitiveRestartIndex(UCHAR_MAX);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		cubeShader = Shader();
//		//cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
//		//cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");
//
//		//cubeProgram = glCreateProgram();
//		//glAttachShader(cubeProgram, cubeShaders[0]);
//		//glAttachShader(cubeProgram, cubeShaders[1]);
//		//glBindAttribLocation(cubeProgram, 0, "in_Position");
//		//glBindAttribLocation(cubeProgram, 1, "in_Normal");
//		//linkProgram(cubeProgram);
//	}
//	void cleanupCube() {
//		glDeleteBuffers(3, cubeVbo);
//		glDeleteVertexArrays(1, &cubeVao);
//
//		glDeleteProgram(cubeProgram);
//		glDeleteShader(cubeShaders[0]);
//		glDeleteShader(cubeShaders[1]);
//	}
//	void updateCube(const glm::mat4& transform) {
//		objMat = transform;
//	}
//	/*void drawCube() {
//		glEnable(GL_PRIMITIVE_RESTART);
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.1f, 1.f, 1.f, 0.f);
//		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//		glDisable(GL_PRIMITIVE_RESTART);
//	}*/
//
//	void drawTwoCubes() {
//		float currentTime = ImGui::GetTime();
//
//		glEnable(GL_PRIMITIVE_RESTART);
//		glBindVertexArray(cubeVao);
//		glUseProgram(cubeProgram);
//
//		//t = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.0f, 3.0f)); //Normal state
//		glm::mat4 t = glm::translate(glm::mat4(), glm::vec3(0.f, -20.f, 0.f));;
//		if (light.type != Light::EType::DIRECTIONAL)
//			t = glm::translate(glm::mat4(), light.position);
//		glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(0.5f, 0.5f, 0.5f));
//		objMat = t * s;
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
//		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
//		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 1.f, 1.f, 1.f, 0.f);
//		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);
//
//		float red = 0.5f + 0.5f * sin(3.f * currentTime);
//		float size = 1.0f; // 1.5f + 0.5f*sin(3.f*currentTime);
//
//		//glm::mat4 t2 = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.5f + 2.f*sin(3.f*currentTime), 3.0f));
//		//glm::mat4 t2 = glm::translate(glm::mat4(), glm::vec3(1.0f, 0.f, 1.0f));
//		//glm::mat4 s = glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f));
//		//glm::mat4 r = glm::rotate(glm::mat4(), 8.f * (float)sin(3.f * currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
//		//objMat = t * r * (t2)*s;
//
//		//glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
//		//glUniform4f(glGetUniformLocation(cubeProgram, "color"), red, 0.f, 0.f, 0.f); //Sets color for 2nd cube
//		//glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);
//
//		glUseProgram(0);
//		glBindVertexArray(0);
//
//		glDisable(GL_PRIMITIVE_RESTART);
//	}
//}

//Classe "Objecte": Agrupa els atributs necessaris dels objectes que carguem i instanciem a l'escena.
//També conté els seus shaders
class LoadObject {
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;

	Shader shader;

	GLuint ObjVao;
	GLuint textureID;
	GLuint ObjVbo[3];

	glm::mat4 objMat;

#pragma region Shaders
	/*const char* Obj_vertShader =
		"#version 330 core\n\
		layout(location = 0) in vec3 aPos;\n\
		layout(location = 1) in vec2 aUvs; \n\
		layout(location = 2) in vec3 aNormal;\n\
		out vec3 FragPos; \n\
		out vec3 Normal; \n\
		out vec2 Uvs; \n\
		uniform mat4 model; \n\
		uniform mat4 view; \n\
		uniform mat4 projection; \n\
		mat4 normalMat; \n\
		void main(){\n\
			FragPos = vec3(model * vec4(aPos, 1.0)); \n\
			Normal = mat3(transpose(inverse(model))) * aNormal; \n\
			gl_Position = projection * view * vec4(FragPos, 1.0); \n\
			Uvs = aUvs;\n\
	}";*/

	//const char* Obj_fragShader =
	//	"#version 330 core\n\
	//	out vec4 FragColor; \n\
	//	in vec3 Normal; \n\
	//	in vec3 FragPos; \n\
	//	in vec2 Uvs; \n\
	//	uniform sampler2D diffuseTexture; \n\
	//	uniform mat4 view;\n\
	//	uniform vec3 lightPos; \n\
	//	uniform vec3 spotLightDir; \n\
	//	uniform vec3 lightColor; \n\
	//	uniform vec3 objectColor; \n\
	//	uniform vec3 ambientColor; \n\
	//	uniform vec3 specularColor; \n\
	//	uniform float lightIntensity;\n\
	//	\n\
	//	uniform float ambientStrength;\n\
	//	uniform float diffuseStrength;\n\
	//	\n\
	//	uniform float specularStrength;\n\
	//	uniform float shininessValue;\n\
	//	uniform int lightType;\n\
	//	uniform float constant;\n\
	//	uniform float linear;\n\
	//	uniform float quadratic;\n\
	//	uniform int attenuationActive;\n\
	//	uniform float cutOff;\n\
	//	vec3 norm;\n\
	//	float diff;\n\
	//	vec3 ambient;\n\
	//	vec3 diffuse;\n\
	//	vec3 viewPos;\n\
	//	vec3 viewDir;\n\
	//	vec3 lightDir;\n\
	//	vec3 reflectDir;\n\
	//	float spec;\n\
	//	float distance;\n\
	//	vec3 specular;\n\
	//	vec3 result;\n\
	//	float attenuation;\n\
	//	void main(){\n\
	//		/*if(mod(gl_FragCoord.x, 2) > 0.5){\n\
	//			discard;\n\
	//		}*/\n\
	//		// AMBIENT // \n\
	//		ambient = ambientStrength * lightColor * ambientColor;\n\
	//		\n\
	//		switch (lightType) {\n\
	//		case 1:\n\
	//			// DIFFUSE // \n\
	//			\n\
	//			lightDir = normalize(-lightPos);\n\
	//			norm = normalize(Normal);\n\
	//			\n\
	//			diff = max(dot(norm, -lightDir), 0.0);\n\
	//			diffuse = diff * diffuseStrength * lightColor;\n\
	//			// SPECULAR // \n\
	//			\n\
	//			viewPos = vec3(inverse(view)[3]); \n\
	//			viewDir = normalize(viewPos - FragPos); \n\
	//			reflectDir = reflect(lightDir, norm); \n\
	//			spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue); \n\
	//			specular = specularStrength * spec * lightColor;\n\
	//			result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity; \n\
	//			break;\n\
	//		case 2:\n\
	//			// DIFFUSE // \n\
	//			\n\
	//			distance = length(lightPos - FragPos);\n\
	//			attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));\n\
	//			norm = normalize(Normal);\n\
	//			lightDir = normalize(lightPos - FragPos);\n\
	//			\n\
	//			diff = max(dot(norm, lightDir), 0.0);\n\
	//			diffuse = diff * diffuseStrength * lightColor;\n\
	//			// SPECULAR // \n\
	//			\n\
	//			viewPos = vec3(inverse(view)[3]); \n\
	//			viewDir = normalize(viewPos - FragPos); \n\
	//			reflectDir = reflect(-lightDir, norm); \n\
	//			spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue); \n\
	//			specular = specularStrength * spec * lightColor;\n\
	//			ambient = ambient * attenuation;\n\
	//			diffuse = diffuse * attenuation;\n\
	//			specular = specular * attenuation;\n\
	//			result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity; \n\
	//			break;\n\
	//		case 3:\n\
	//			lightDir = normalize(lightPos - FragPos);\n\
	//			viewPos = vec3(inverse(view)[3]); \n\
	//			viewDir = normalize(viewPos - FragPos); \n\
	//			float theta = dot(lightDir, normalize(-spotLightDir));\n\
	//			if (theta > cutOff){\n\
	//				\n\
	//				// DIFFUSE // \n\
	//				norm = normalize(Normal);\n\
	//				diff = max(dot(norm, lightDir), 0.0);\n\
	//				diffuse = diff * diffuseStrength * lightColor;\n\
	//				\n\
	//				// SPECULAR // \n\
	//				reflectDir = reflect(-lightDir, norm); \n\
	//				spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue); \n\
	//				specular = specularStrength * spec * lightColor;\n\
	//				\n\
	//				distance = length(lightPos - FragPos);\n\
	//				if(attenuationActive == 1)\n\
	//				{\n\
	//				attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));\n\
	//				diffuse = diffuse * attenuation; \n\
	//				specular = specular * attenuation; \n\
	//				}\n\
	//				\n\
	//				\n\
	//				result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity; \n\
	//			}\n\
	//			else {\n\
	//				result = ambient;\n\
	//			}\n\
	//			break;\n\
	//		}\n\
	//		FragColor = texture(diffuseTexture, Uvs) * vec4(result, 1.0); \n\
	//	}";


#pragma endregion

public:
	std::string name;
	glm::vec3 initPos, initRot, initScale;
	glm::vec3 dollyPos, dollyRot, dollyScale;
	glm::vec3 objectColor;
	glm::vec3 position, rotation, scale;

	int texWidth, texHeight, nrChannels;

	LoadObject(std::string _path, glm::vec3 _startPos, glm::vec3 _startRot, glm::vec3 _startScale, glm::vec3 _startColor = { 1.f, 0.5f, 0.31f }) :
		name(_path), position(_startPos), rotation(_startRot), scale(_startScale), objectColor(_startColor), initPos(_startPos), initRot(_startRot), initScale(_startScale)
	{
		bool res = loadOBJ(_path.c_str(), vertices, uvs, normals);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load("cat_texture.jpg", &texWidth, &texHeight, &nrChannels, 0);

		name.erase(name.size() - 4, name.size());

		glGenVertexArrays(1, &ObjVao);
		glGenTextures(1, &textureID); //TEXTURES
		glBindVertexArray(ObjVao);
		glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //TEXTURES
			//glGenerateMipmap(GL_TEXTURE_2D);
		}
		else std::cout << "Failed to load texture" << std::endl;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		stbi_image_free(data);		

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

		shader = Shader("shader.vs", "shader.fs");

		glBindAttribLocation(shader.programID, 0, "aPos");
		glBindAttribLocation(shader.programID, 1, "aUvs");
		glBindAttribLocation(shader.programID, 2, "aNormal");

		//ObjShader[0] = shader.CompileShader(Obj_vertShader, GL_VERTEX_SHADER, "ObjVert");
		//ObjShader[1] = shader.CompileShader(Obj_fragShader, GL_FRAGMENT_SHADER, "ObjFrag");
		//shader.LinkProgram(ObjProgram);

		//ObjProgram = glCreateProgram();
		//glAttachShader(ObjProgram, ObjShader[0]);
		//glAttachShader(ObjProgram, ObjShader[1]);
		updateObj();
	}

	void cleanupObj()
	{
		glDeleteBuffers(3, ObjVbo);
		glDeleteVertexArrays(1, &ObjVao);

		/*glDeleteProgram(shader.programID);
		glDeleteShader(ObjShader[0]);
		glDeleteShader(ObjShader[1]);*/
		shader.CleanUpShader();

		glDeleteTextures(1, &textureID);
	}

	//Actualitza objMat amb les matrius de transformació (translació, rotació i escala)
	void updateObj()
	{
		glm::mat4 t = glm::translate(glm::mat4(), position);
		glm::mat4 r1 = glm::rotate(glm::mat4(), rotation.x, glm::vec3(1, 0, 0));
		glm::mat4 r2 = glm::rotate(glm::mat4(), rotation.y, glm::vec3(0, 1, 0));
		glm::mat4 r3 = glm::rotate(glm::mat4(), rotation.z, glm::vec3(0, 0, 1));
		glm::mat4 s = glm::scale(glm::mat4(), scale);
		objMat = t * r1 * r2 * r3 * s;
	}

	void drawObj()
	{
		shader.Use();
		//glUseProgram(ObjProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(ObjVao);
		//glUniformMatrix4fv(glGetUniformLocation(ObjProgram, "model"), 1, GL_FALSE, glm::value_ptr(objMat));
		//glUniformMatrix4fv(glGetUniformLocation(ObjProgram, "view"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		//glUniformMatrix4fv(glGetUniformLocation(ObjProgram, "projection"), 1, GL_FALSE, glm::value_ptr(RenderVars::_projection));
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
		
		//glUniform3f(glGetUniformLocation(ObjProgram, "objectColor"), objectColor.x, objectColor.y, objectColor.z);
		//glUniform3f(glGetUniformLocation(ObjProgram, "lightColor"), light.color.x, light.color.y, light.color.z);
		//glUniform3f(glGetUniformLocation(ObjProgram, "lightPos"), light.position.x, light.position.y, light.position.z);
		//glUniform3f(glGetUniformLocation(ObjProgram, "spotLightDir"), light.spotLightDirection.x, light.spotLightDirection.y, light.spotLightDirection.z);
		//glUniform1f(glGetUniformLocation(ObjProgram, "lightIntensity"), light.intensity);
		//glUniform1i(glGetUniformLocation(ObjProgram, "attenuationActive"), light.attenuationActivated);
		//glUniform1i(glGetUniformLocation(ObjProgram, "lightType"), (int)light.type);
		//glUniform1f(glGetUniformLocation(ObjProgram, "constant"), light.constant);
		//glUniform1f(glGetUniformLocation(ObjProgram, "linear"), light.linear);
		//glUniform1f(glGetUniformLocation(ObjProgram, "quadratic"), light.quadratic);
		//glUniform1f(glGetUniformLocation(ObjProgram, "cutOff"), light.cutOff);
		//glUniform1f(glGetUniformLocation(ObjProgram, "ambientStrength"), light.ambientIntensity);
		//glUniform3f(glGetUniformLocation(ObjProgram, "ambientColor"), light.ambientColor.x, light.ambientColor.y, light.ambientColor.z);
		//glUniform1f(glGetUniformLocation(ObjProgram, "diffuseStrength"), light.diffuseIntensity);
		//glUniform1f(glGetUniformLocation(ObjProgram, "specularStrength"), light.specularIntensity);
		//glUniform3f(glGetUniformLocation(ObjProgram, "specularColor"), light.specularColor.x, light.specularColor.y, light.specularColor.z);
		//glUniform1f(glGetUniformLocation(ObjProgram, "shininessValue"), light.shininessValue);
		//glUniform1i(glGetUniformLocation(ObjProgram, "diffuseTexture"), 0);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glUseProgram(0);
		glBindVertexArray(0);
	}

#pragma region Inverse Dolly Effect
	//Calcula l'amplitud de l'escena a partir de la fórmula mencionada en el pdf adjuntat
	float CalculateWidth()
	{
		return CameraObjectDistance() * 2 * glm::tan(RV::FOV / 2);
	}

	//Càlcul de la distància entre la posició de la "càmera" i l'objecte a enquadrar.
	float CameraObjectDistance()
	{
		glm::vec3 viewPos = glm::vec3(inverse(RenderVars::_modelView)[3]);
		return glm::distance(viewPos, position);
	}

	//Càlcul del FOV variable aïllant-lo de la mateixa fórmula mencionada anteriorment.
	float CalculateNewFov(float _dollyEffectWidth)
	{
		float newFov = 2 * (glm::atan(_dollyEffectWidth / (2 * CameraObjectDistance())));
		return newFov;
	}
#pragma endregion
};

std::vector<LoadObject> objectVectors; //--> Vector que emmagatzema els objectes que s'instancien a l'escena.

void GLinit(int width, int height) {

	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	// Setup shaders & geometry
	light.type = Light::EType::DIRECTIONAL;
	/*Axis::setupAxis();
	Cube::setupCube();*/

	//Crida al constructor de la classe amb els diferents objectes
	LoadObject Neko("cat.obj", glm::vec3(-3.11f, 1.6f, 2.71f), glm::vec3(0, 4.71f, 0), glm::vec3(1, 1, 1));

	//Emmagatzema els objectes creats al vector
	objectVectors.push_back(Neko);
}

void GLcleanup() {
	/*Axis::cleanupAxis();
	Cube::cleanupCube();*/

	/////////////////////////////////////////////////////TODO
	// Do your cleanup code here
	//Cleanup per cada objecte dins del vector
	for (int i = 0; i < objectVectors.size(); i++)
	{
		objectVectors[i].cleanupObj();
	}
	objectVectors.clear(); //--> Allibera memòria del vector d'objectes
	/////////////////////////////////////////////////////////
}

void GLrender(float dt) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RV::_modelView = glm::mat4(1.f);

	if (!Dolly::animationStarted) //--> Si l'animació no ha començat permet moure la càmera
	{
		RV::_modelView = glm::translate(RV::_modelView, glm::vec3(RV::panv[0], RV::panv[1], RV::panv[2]));
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));
	}
	else
	{
		//Màquina d'estats per controlar l'animació del Dolly Effect
		switch (Dolly::eDollyState)
		{
		case Dolly::State::START: //-->Comença l'animació, frame 0, s'inicialitza la informació.
			Dolly::zoom = Dolly::ZDistance;
			RV::_modelView = glm::translate(RV::_modelView, glm::vec3(objectVectors[0].position.x * -1, objectVectors[0].position.y * -1, objectVectors[0].position.z - Dolly::zoom));
			RV::_modelView = glm::rotate(RV::_modelView, 0.f, glm::vec3(0.f, 0.f, 1.f));
			RV::_modelView = glm::rotate(RV::_modelView, 0.f, glm::vec3(1.f, 1.f, 0.f));
			Dolly::width = objectVectors[Dolly::objectSelected].CalculateWidth();
			Dolly::initialDistance = objectVectors[Dolly::objectSelected].CameraObjectDistance();
			Dolly::eDollyState = Dolly::State::BACKING;
			break;
		case Dolly::State::BACKING: //-->La càmera s'allunya
			Dolly::zoom += Dolly::speed * 0.005f;
			if (Dolly::zoom >= Dolly::animationDistance) { Dolly::eDollyState = Dolly::State::FORWADING; }
			break;
		case Dolly::State::FORWADING: //-->La càmera s'apropa
			Dolly::zoom -= Dolly::speed * 0.005f;
			if (Dolly::zoom <= Dolly::ZDistance) { Dolly::eDollyState = Dolly::State::BACKING; }
			break;
		case Dolly::State::END: //-->Acaba el recorregut de la iteració, ens assegurem que el FOV acaba igual que comença.
			RV::FOV = glm::radians(90.f);
			Dolly::animationStarted = false;
			break;
		default:
			break;
		}

		//Fixem la càmera en un punt concret i modifiquem només l'eix de la z pel moviment.
		RV::_modelView = glm::translate(RV::_modelView, glm::vec3(objectVectors[0].position.x * -1, objectVectors[0].position.y * -1, objectVectors[0].position.z - Dolly::zoom));
		RV::_modelView = glm::rotate(RV::_modelView, 0.f, glm::vec3(0.f, 0.f, 1.f));
		RV::_modelView = glm::rotate(RV::_modelView, 0.f, glm::vec3(1.f, 1.f, 0.f));

		//Recalculem el FOV i l'apliquem a la perspectiva de la matriu de projecció
		if (Dolly::eDollyState != Dolly::State::START)
		{
			if (Dolly::eDollyState != Dolly::State::END)
				RV::FOV = objectVectors[Dolly::objectSelected].CalculateNewFov(Dolly::width);

			RV::_projection = glm::perspective(RV::FOV, 1800.f / 960.f, RV::zNear, RV::zFar);
		}
	}

	RV::_MVP = RV::_projection * RV::_modelView;
	/*Axis::drawAxis();
	Cube::drawTwoCubes();*/

	//S'actualitza i es dibuixa a cada objecte del vector
	for (int i = 0; i < objectVectors.size(); i++)
	{
		//if (i == 0) // Solo lo va a hacer con el gato
		//{
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//}
		objectVectors[i].updateObj();
		objectVectors[i].drawObj();
		//if (i == 0) // Solo lo va a hacer con el gato
		//{
		//	glDisable(GL_BLEND);
		//}
	}

	ImGui::Render();
}

std::string s; //-->String declarat global per no redeclarar-lo a cada frame. S'usa pels noms del ImGui.

void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	/////////////////////////////////////////////////////TODO
	// Do your GUI code here....

#pragma region Lights
	//Botons per canviar dels tipus d'il·luminació
	if (ImGui::Button("Directional light")) { light.intensity = 1.f; light.position = glm::vec3{ 0.f, 1.f, 0.f }; light.type = Light::EType::DIRECTIONAL; }
	if (ImGui::Button("Point light")) { light.intensity = 3.f; light.position = glm::vec3{ -4.f, 6.7f, 2.2f }; light.type = Light::EType::POINTLIGHT; }
	if (ImGui::Button("Spot light")) { light.type = Light::EType::SPOTLIGHT; light.intensity = 3.f; light.position = glm::vec3{ 0.f, 2.9f, 13.5f }; light.spotLightDirection = glm::vec3{ 0.f, 0.f, -1.f }; light.spotLightAngle = 21.85f; }

	ImGui::ColorEdit3("Light color", (float*)&light.color); //--> Color de la llum
	ImGui::DragFloat("Light intensity", (float*)&light.intensity, 0.01f, 0.f, 10.f); //--> Intensitat de la llum

	//Switch per canviar la informació de la interfaç segons el tipus d'il·luminació que s'esta fent servir
	switch (light.type)
	{
	case Light::EType::DIRECTIONAL:
		ImGui::DragFloat3("Light Direction", (float*)&light.position, 0.005f, -1.f, 1.f);
		break;
	case Light::EType::POINTLIGHT:
		ImGui::DragFloat3("Pointlight Position", (float*)&light.position, 0.1f, -50.f, 50.f);
		break;
	case Light::EType::SPOTLIGHT:
		ImGui::DragFloat3("Spotlight Position", (float*)&light.position, 0.1f, -50.f, 50.f);
		ImGui::DragFloat3("Spotlight Direction", (float*)&light.spotLightDirection, 0.005f, -1.f, 1.f);
		ImGui::DragFloat("Spotlight angle", &light.spotLightAngle, 0.05f, 10.f, 50.f);
		light.cutOff = glm::cos(glm::radians(light.spotLightAngle));

		s = (light.attenuationActivated == 1) ? "Deactivate attenuation" : "Activate attenuation";

		if (ImGui::Button(s.c_str())) {
			light.attenuationActivated *= -1;
		}
		break;
	}
	ImGui::ColorEdit3("Ambientcolor", (float*)&light.ambientColor); //--> Color de la llum ambient
	ImGui::DragFloat("Ambient strength", &light.ambientIntensity, 0.005f, 0.f, 3.f); //--> Intensitat de la llum ambient
	ImGui::DragFloat("Diffuse strength", &light.diffuseIntensity, 0.01f, 0.f, 10.f); //--> Intensitat de la llum difusa
	ImGui::ColorEdit3("Specular color", (float*)&light.specularColor); //--> Color de la llum especular
	ImGui::DragFloat("Specular strength", &light.specularIntensity, 0.01f, 0.f, 10.f); //--> Intensitat de la llum especular
	ImGui::DragFloat("Shininess value ", &light.shininessValue, 0.5f, 1.f, 256.f); //--> Quantitat de la brillentor de la llum especular
#pragma endregion

#pragma region Objects

	//Informació modificable de cada objecte
	for (int i = 0; i < objectVectors.size(); i++)
	{
		ImGui::PushID(i);
		s = std::to_string(i + 1) + ": " + objectVectors[i].name + " Color";
		ImGui::ColorEdit3(s.c_str(), (float*)&objectVectors[i].objectColor);

		s = std::to_string(i + 1) + ": " + objectVectors[i].name + " Position";
		ImGui::DragFloat3(s.c_str(), (float*)&objectVectors[i].position, 0.01f, -50.f, 50.f);

		s = std::to_string(i + 1) + ": " + objectVectors[i].name + " Rotation";
		ImGui::DragFloat3(s.c_str(), (float*)&objectVectors[i].rotation, 0.01f, 0.f, 360.f);

		s = std::to_string(i + 1) + ": " + objectVectors[i].name + " Scale";
		ImGui::DragFloat3(s.c_str(), (float*)&objectVectors[i].scale, 0.01f, 0.01f, 50.f);

		ImGui::PopID();
	}

#pragma endregion
	/////////////////////////////////////////////////////////
// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}