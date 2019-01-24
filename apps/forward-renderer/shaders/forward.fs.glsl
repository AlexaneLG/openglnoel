#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

// Directional Light Parameters
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

// Point Light Parameters
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;

// Diffuse Color
uniform vec3 uKd;

// Texture
uniform sampler2D uKdSampler;

out vec3 fColor;

void main()
{
	float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
	vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
	//fColor = vec3(vTexCoords, 0);
	vec3 kd = uKd * vec3(texture(uKdSampler, vTexCoords));
	fColor = kd * (uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, uDirectionalLightDir)) + uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
	
}