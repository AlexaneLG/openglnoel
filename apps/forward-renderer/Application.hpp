#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/scene_loading.hpp>

class Application
{
public:
	Application(int argc, char** argv);

	int run();
private:
	const size_t m_nWindowWidth = 1280;
	const size_t m_nWindowHeight = 720;
	glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

	const glmlv::fs::path m_AppPath;
	const std::string m_AppName;
	const std::string m_ImGuiIniFilename;
	const glmlv::fs::path m_ShadersRootPath;
	const glmlv::fs::path m_AssetsRootPath;

	glmlv::SimpleGeometry m_cubeGeometry;
	glmlv::SimpleGeometry m_sphereGeometry;

	GLuint m_cubeVBO = 0;
	GLuint m_cubeIBO = 0;
	GLuint m_cubeVAO = 0;
	GLuint m_cubeTextureKd = 0;

	GLuint m_sphereVBO = 0;
	GLuint m_sphereIBO = 0;
	GLuint m_sphereVAO = 0;
	GLuint m_sphereTextureKd = 0;

	GLuint m_textureSampler;

	glmlv::GLProgram m_program;

	glmlv::ViewController m_viewController{ m_GLFWHandle.window(), 1.f };
	GLint m_uModelViewProjMatrixLocation;
	GLint m_uModelViewMatrixLocation;
	GLint m_uNormalMatrixLocation;

	GLint m_uKd;
	GLint m_uDirectionalLightDir;
	GLint m_uDirectionalLightIntensity;
	GLint m_uPointLightPosition;
	GLint m_uPointLightIntensity;
	GLint m_uKdSampler;

	glm::vec3 m_DirectionalLightDir = glm::vec3(1, 1, 1); // normal
	glm::vec3 m_DirectionalLightColor = glm::vec3(1, 1, 1);
	float m_DirectionalLightIntensity = 1.f;

	glm::vec3 m_PointLightPosition = glm::vec3(0, 1, 0);
	glm::vec3 m_PointLightColor = glm::vec3(1, 1, 1);
	float m_PointLightIntensity = 5.f;

	glm::vec3 m_CubeKd = glm::vec3(1, 1, 1);
	glm::vec3 m_SphereKd = glm::vec3(1, 1, 1);

	/*
	float m_DirLightPhiAngleDegrees = 90.f;
    float m_DirLightThetaAngleDegrees = 45.f;
    glm::vec3 m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
	*/

	// Required data about the scene in CPU in order to send draw calls
	struct ShapeInfo
	{
		uint32_t indexCount; // Number of indices
		uint32_t indexOffset; // Offset in GPU index buffer
		int materialID = -1;
		glm::mat4 localToWorldMatrix;
	};

	std::vector<ShapeInfo> m_shapes; // For each shape of the scene, its number of indices
	float m_SceneSize = 0.f; // Used for camera speed and projection matrix parameters
};
