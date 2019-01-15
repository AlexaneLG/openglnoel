#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
	// Put here code to run before rendering loop
	glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), float(600) / 600, 0.1f, 100.f);
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -5));
	glm::mat4 NormalMatrix = glm::transpose(glm::inverse(ModelViewMatrix));
	glm::mat4 ModelViewProjectionMatrix = ProjMatrix * ModelViewMatrix;

	glClearColor(0, 0, 1, 1);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(uModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewMatrix));
		glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		glBindVertexArray(m_cubeVAO);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		const auto fbSize = m_GLFWHandle.framebufferSize();
		glViewport(0, 0, fbSize.x, fbSize.y);
		glClear(GL_COLOR_BUFFER_BIT);

        // GUI code:
		glmlv::imguiNewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		glmlv::imguiRenderFrame();

        glfwPollEvents(); // Poll for and process events

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            // Put here code to handle user interactions
        }

		m_GLFWHandle.swapBuffers(); // Swap front and back buffers
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    // Put here initialization code
	uModelViewProjectionMatrix = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
	uModelViewMatrix = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
	uNormalMatrix = glGetUniformLocation(m_program.glId(), "uNormalMatrix");

	m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &m_cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
		glmlv::SimpleGeometry cube =  glmlv::makeCube();
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(cube.vertexBuffer), &cube.vertexBuffer, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &m_cubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube.vertexBuffer), &cube.vertexBuffer, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &m_cubeVAO);
	glBindVertexArray(m_cubeVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);

		const GLuint VERTEX_ATTR_POSITION = 0;
		const GLuint VERTEX_ATTR_NORMAL = 1;
		const GLuint VERTEX_ATTR_TEXCOORDS = 2;
		glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
		glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
		glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

		glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
			glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
			glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
			glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_program.use();
}

Application::~Application()
{
	if (m_cubeVBO) {
		glDeleteBuffers(1, &m_cubeVBO);
	}
	if (m_cubeVAO) {
		glDeleteBuffers(1, &m_cubeVAO);
	}
	if (m_cubeIBO) {
		glDeleteBuffers(1, &m_cubeIBO);
	}
	glfwTerminate();
}