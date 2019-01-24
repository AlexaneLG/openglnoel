#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
	float clearColor[3] = { 0.5, 0.5, 0.5 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

	// Loop until the user closes the window
	for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
	{
		const auto seconds = glfwGetTime();

		// Put here rendering code
		const auto viewportSize = m_GLFWHandle.framebufferSize();
		glViewport(0, 0, viewportSize.x, viewportSize.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
		//const auto viewMatrix = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		const auto viewMatrix = m_viewController.getViewMatrix();

		// Lights
		// GLint location, GLsizei count, GLfloat *value
		// tous les vecteurs doivent être normalisés
		glUniform3fv(m_uDirectionalLightDir, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirectionalLightDir), 0)))); // m_DirectionalLightDir non défini
		glUniform3fv(m_uDirectionalLightIntensity, 1, glm::value_ptr(m_DirectionalLightIntensity * m_DirectionalLightColor));
		glUniform3fv(m_uPointLightPosition, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
		glUniform3fv(m_uPointLightIntensity, 1, glm::value_ptr(m_PointLightIntensity * m_PointLightColor));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_uKdSampler, 0);
		glBindSampler(0, m_textureSampler);

		{
			const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));
			const auto mvMatrix = viewMatrix * modelMatrix;
			const auto mvpMatrix = projMatrix * mvMatrix;
			const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			// GLint location, GLsizei count, GLboolean transpose, const GLfloat *value
			glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			// GLint location, GLsizei count, GLfloat *value
			glUniform3fv(m_uKd, 1, glm::value_ptr(m_CubeKd));

			glBindTexture(GL_TEXTURE_2D, m_cubeTextureKd);

			glBindVertexArray(m_cubeVAO);
			glDrawElements(GL_TRIANGLES, m_cubeGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
		}

		{
			const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));
			const auto mvMatrix = viewMatrix * modelMatrix;
			const auto mvpMatrix = projMatrix * mvMatrix;
			const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			// GLint location, GLsizei count, GLboolean transpose, const GLfloat *value
			glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			// GLint location, GLsizei count, GLfloat *value
			glUniform3fv(m_uKd, 1, glm::value_ptr(m_SphereKd));

			glBindTexture(GL_TEXTURE_2D, m_sphereTextureKd);

			glBindVertexArray(m_sphereVAO);
			glDrawElements(GL_TRIANGLES, m_sphereGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
		}

		glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
		glBindSampler(0, 0); // unbind sampler

		// GUI code:
		glmlv::imguiNewFrame();

		{
			ImGui::Begin("GUI");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			if (ImGui::ColorEdit3("clearColor", clearColor)) {
				glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
			}
			// Color cubeKd and sphereKd
			if (ImGui::CollapsingHeader("Materials")) {
				ImGui::ColorEdit3("CubeKd", glm::value_ptr(m_CubeKd));
				ImGui::ColorEdit3("SphereKd", glm::value_ptr(m_SphereKd));
			}
			if (ImGui::CollapsingHeader("Point Light"))
			{
				ImGui::ColorEdit3("PointLightColor", glm::value_ptr(m_PointLightColor));
				ImGui::DragFloat("PointLightIntensity", &m_PointLightIntensity, 0.1f, 0.f, 16000.f);
				ImGui::InputFloat3("Position", glm::value_ptr(m_PointLightPosition));
			}
			ImGui::End();
		}

		glmlv::imguiRenderFrame();

		/* Poll for and process events */
		glfwPollEvents();

		/* Swap front and back buffers*/
		m_GLFWHandle.swapBuffers();

		auto ellapsedTime = glfwGetTime() - seconds;
		auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
		if (!guiHasFocus) {
			// Nothing for now
			// update ViewController
			m_viewController.update(float(ellapsedTime));
		}
	}

	return 0;
}

Application::Application(int argc, char** argv) :
	m_AppPath{ glmlv::fs::path{ argv[0] } },
	m_AppName{ m_AppPath.stem().string() },
	m_ImGuiIniFilename{ m_AppName + ".imgui.ini" },
	m_ShadersRootPath{ m_AppPath.parent_path() / "shaders" },
	m_AssetsRootPath{ m_AppPath.parent_path() / "assets" }

{
	ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

	glActiveTexture(GL_TEXTURE0); // select active texture unit
	// create two texture objects
	{
		glmlv::Image2DRGBA imageMirai = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "mirai.jpg"); // load image
		glGenTextures(1, &m_cubeTextureKd); // generate texture name
		glBindTexture(GL_TEXTURE_2D, m_cubeTextureKd); // bind a named texture to a texturing target
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, imageMirai.width(), imageMirai.height()); // simultaneously specify storage for all levels of a two-dimensional or one-dimensinal array texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageMirai.width(), imageMirai.height(), GL_RGBA, GL_UNSIGNED_BYTE, imageMirai.data()); // specify a two-dimensional texture subimage
	}
	{
		glmlv::Image2DRGBA imageMaquia = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "maquia.jpg"); // load image
		glGenTextures(1, &m_sphereTextureKd); // generate texture name
		glBindTexture(GL_TEXTURE_2D, m_sphereTextureKd); // bind a named texture to a texturing target
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, imageMaquia.width(), imageMaquia.height()); // simultaneously specify storage for all levels of a two-dimensional or one-dimensinal array texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageMaquia.width(), imageMaquia.height(), GL_RGBA, GL_UNSIGNED_BYTE, imageMaquia.data()); // specify a two-dimensional texture subimage
	}

	//glmlv::loadObjScene(const glmlv::fs::path & objPath, glmlv::SceneData & data);

	const GLint vboBindingIndex = 0; // Arbitrary choice between 0 and glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS)

	const GLint positionAttrLocation = 0;
	const GLint normalAttrLocation = 1;
	const GLint texCoordsAttrLocation = 2;

	glGenBuffers(1, &m_cubeVBO);
	glGenBuffers(1, &m_cubeIBO);
	glGenBuffers(1, &m_sphereVBO);
	glGenBuffers(1, &m_sphereIBO);

	m_cubeGeometry = glmlv::makeCube();
	m_sphereGeometry = glmlv::makeSphere(32);

	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glBufferStorage(GL_ARRAY_BUFFER, m_cubeGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_cubeGeometry.vertexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
	glBufferStorage(GL_ARRAY_BUFFER, m_sphereGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_sphereGeometry.vertexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
	glBufferStorage(GL_ARRAY_BUFFER, m_cubeGeometry.indexBuffer.size() * sizeof(uint32_t), m_cubeGeometry.indexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_sphereIBO);
	glBufferStorage(GL_ARRAY_BUFFER, m_sphereGeometry.indexBuffer.size() * sizeof(uint32_t), m_sphereGeometry.indexBuffer.data(), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Lets use a lambda to factorize VAO initialization:
	const auto initVAO = [positionAttrLocation, normalAttrLocation, texCoordsAttrLocation](GLuint& vao, GLuint vbo, GLuint ibo)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// We tell OpenGL what vertex attributes our VAO is describing:
		glEnableVertexAttribArray(positionAttrLocation);
		glEnableVertexAttribArray(normalAttrLocation);
		glEnableVertexAttribArray(texCoordsAttrLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

		glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
		glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
		glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

		glBindVertexArray(0);
	};

	initVAO(m_cubeVAO, m_cubeVBO, m_cubeIBO);
	initVAO(m_sphereVAO, m_sphereVBO, m_sphereIBO);

	// sampler = set of GLSL variable types
	// each sampler represents a single texture of a particular texture type
	glGenSamplers(1, &m_textureSampler); // generate sampler object names, no need to bind to access it
	glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_DEPTH_TEST);

	m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
	m_program.use();

	// Get Uniform Locations
	m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
	m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
	m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
	m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	m_uKd = glGetUniformLocation(m_program.glId(), "uKd");

	m_uDirectionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
	m_uDirectionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

	m_uPointLightPosition = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
	m_uPointLightIntensity = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

	m_uKdSampler = glGetUniformLocation(m_program.glId(), "uKdSampler");
}