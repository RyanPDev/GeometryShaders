#pragma once
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <stb_image.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
};

//Enum per controlar les diferents escenes de la demostració
enum class Scene { PHONG, TEXTURING, GEOMETRY_SHADERS };

namespace RenderVars {
	extern glm::mat4 _projection;
	extern glm::mat4 _modelView;
	extern glm::mat4 _MVP;
}