#include "Application.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
	// Put here code to run before rendering loop

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
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

	glGenBuffers(1, &m_cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
		glmlv::SimpleGeometry cube =  glmlv::makeCube();
		//glBufferData(GL_ARRAY_BUFFER, 4 * 6 * sizeof(glmlv::Vertex3f3f2f), &cube.vertexBuffer, GL_STATIC_DRAW);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(cube.vertexBuffer), &cube.vertexBuffer, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &m_cubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(uint32_t), &cube.indexBuffer, GL_STATIC_DRAW);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube.vertexBuffer), &cube.vertexBuffer, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &m_cubeVAO);
	glBindVertexArray(m_cubeVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
		//m_program = glmlv::compileProgram({ m_ShadersRootPath / "glmlv" / "position2_color3.vs.glsl", m_ShadersRootPath / "glmlv" / "color3.fs.glsl" });
		//m_program = glmlv::compileProgram({ "forward.vs.glsl", "forward.fs.glsl" });
		const GLuint VERTEX_ATTR_POSITION = 0;
		const GLuint VERTEX_ATTR_COLOR = 1;
		glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
		glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, m_cubeIBO);
			glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
			glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//m_program.use();
	glEnable(GL_DEPTH_TEST);
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