#version 330 core
	out vec4 FragColor;
	in vec3 Normal;
	in vec3 FragPos;
	in vec2 Uvs;
	uniform sampler2D diffuseTexture;
	uniform mat4 view;
	uniform vec3 lightPos;
	uniform vec3 spotLightDir;
	uniform vec3 lightColor;
	uniform vec3 objectColor;
	uniform vec3 ambientColor;
	uniform vec3 specularColor;
	uniform float lightIntensity;
	uniform float ambientStrength;
	uniform float diffuseStrength;
	uniform float specularStrength;
	uniform float shininessValue;
	uniform int lightType;
	uniform float constant;
	uniform float linear;
	uniform float quadratic;
	uniform int attenuationActive;
	uniform float cutOff;
	vec3 norm;
	float diff;
	vec3 ambient;
	vec3 diffuse;
	vec3 viewPos;
	vec3 viewDir;
	vec3 lightDir;
	vec3 reflectDir;
	float spec;
	float distance;
	vec3 specular;
	vec3 result;
	float attenuation;
	void main(){
		// AMBIENT // 
		ambient = ambientStrength * lightColor * ambientColor;
		switch (lightType) {
		case 1:
			// DIFFUSE //
			lightDir = normalize(-lightPos);
			norm = normalize(Normal);
			diff = max(dot(norm, -lightDir), 0.0);
			diffuse = diff * diffuseStrength * lightColor;
			// SPECULAR //
			viewPos = vec3(inverse(view)[3]);
			viewDir = normalize(viewPos - FragPos);
			reflectDir = reflect(lightDir, norm);
			spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
			specular = specularStrength * spec * lightColor;
			result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity;
			break;
		case 2:
			// DIFFUSE //
			distance = length(lightPos - FragPos);
			attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
			norm = normalize(Normal);
			lightDir = normalize(lightPos - FragPos);
			diff = max(dot(norm, lightDir), 0.0);
			diffuse = diff * diffuseStrength * lightColor;
			// SPECULAR //
			viewPos = vec3(inverse(view)[3]);
			viewDir = normalize(viewPos - FragPos);
			reflectDir = reflect(-lightDir, norm);
			spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
			specular = specularStrength * spec * lightColor;
			ambient = ambient * attenuation;
			diffuse = diffuse * attenuation;
			specular = specular * attenuation;
			result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity;
			break;
		case 3:
			lightDir = normalize(lightPos - FragPos);
			viewPos = vec3(inverse(view)[3]);
			viewDir = normalize(viewPos - FragPos);
			float theta = dot(lightDir, normalize(-spotLightDir));
			if (theta > cutOff){
			// DIFFUSE //
			norm = normalize(Normal);
			diff = max(dot(norm, lightDir), 0.0);
			diffuse = diff * diffuseStrength * lightColor;
			// SPECULAR //
			reflectDir = reflect(-lightDir, norm); 
			spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessValue);
			specular = specularStrength * spec * lightColor;
			distance = length(lightPos - FragPos);
			if(attenuationActive == 1)
			{
				attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
				diffuse = diffuse * attenuation;
				specular = specular * attenuation;
					}
				result = (ambient + diffuse + (specular * specularColor)) * objectColor * lightIntensity;
			}
			else {
				result = ambient;
			}
			break;
		}
		FragColor = texture(diffuseTexture, Uvs) * vec4(result, 1.0);
}