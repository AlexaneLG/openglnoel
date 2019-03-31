#include "Application.hpp"

#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

int Application::run()
{
	// Put here code to run before rendering loop
	float clearColor[3] = { 0.5, 0.8, 0.2 };
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code
		const auto fbSize = m_GLFWHandle.framebufferSize();
		glViewport(0, 0, fbSize.x, fbSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*______________________________________________________________________*/
		/*glActiveTexture(GL_TEXTURE0); // select active texture unit
		glmlv::Image2DRGBA imageMirai = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "mirai.jpg"); // load image*/
		
		/*glGenTextures(1, &m_texid); // generate texture name

		tinygltf::Texture &tex = m_model.textures[0];
		tinygltf::Image &image = m_model.images[tex.source];

		glBindTexture(GL_TEXTURE_2D, m_texid); // bind a named texture to a texturing target
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width, image.height); // simultaneously specify storage for all levels of a two-dimensional or one-dimensinal array texture
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, &image.image.at(0)); // specify a two-dimensional texture subimage
*/

		const auto projMatrix = glm::perspective(70.f, float(fbSize.x) / fbSize.y, 0.01f, 100.f);
		const auto viewMatrix = m_viewController.getViewMatrix();

		glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
		glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

		glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
		glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_uKdSamplerLocation, 0); // Set the uniform to 0 because we use texture unit 0
		glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit

		for (int i = 0; i < m_vaos.size(); i++) {
			const auto modelMatrix = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0)), 0.2f * float(seconds), glm::vec3(0, 1, 0));

			const auto mvMatrix = viewMatrix * modelMatrix;
			const auto mvpMatrix = projMatrix * mvMatrix;
			const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			glUniform3fv(m_uKdLocation, 1, glm::value_ptr(m_modelKd));

			glBindTexture(GL_TEXTURE_2D, m_texid);

			glBindVertexArray(m_vaos[i]);

			const tinygltf::Scene &scene = m_model.scenes[m_model.defaultScene];

			tinygltf::Accessor indexAccessor = m_model.accessors[m_primitives[i].indices];
			glDrawElements(m_primitives[i].mode, indexAccessor.count, indexAccessor.componentType, BUFFER_OFFSET(indexAccessor.byteOffset));
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindSampler(0, 0); // Unbind the sampler
		/*______________________________________________________________________*/

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
	std::cout << "Constructor : Application" << std::endl;
	m_objPath = argv[1];
	std::cout << "\tObject path : " << m_objPath << std::endl;

	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&m_model, &err, &warn, m_objPath);

	if (!warn.empty()) {
		std::cout << "Warn: " << warn.c_str() << std::endl;
	}

	if (!err.empty()) {
		std::cout << "Err: " << err.c_str() << std::endl;
	}

	if (!ret) {
		std::cout << "Failed to parse glTF" << std::endl;
	}
	else {
		std::cout << "Succed to parse glFT" << std::endl;
	}

	// Texture
	glGenTextures(1, &m_texid); // generate texture name
	tinygltf::Texture &tex = m_model.textures[0];
	tinygltf::Image &image = m_model.images[tex.source];
	glBindTexture(GL_TEXTURE_2D, m_texid); // bind a named texture to a texturing target
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width, image.height); // simultaneously specify storage for all levels of a two-dimensional or one-dimensinal array texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, &image.image.at(0)); // specify a two-dimensional texture subimage


	// Init
	std::vector<GLuint> buffers(m_model.buffers.size());

	glGenBuffers(buffers.size(), buffers.data());
	for (int i = 0; i < buffers.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferStorage(GL_ARRAY_BUFFER, m_model.buffers[i].data.size(), m_model.buffers[i].data.data(), 0);
	}

	for (int i = 0; i < m_model.meshes.size(); i++) {
		for (int j = 0; j < m_model.meshes[i].primitives.size(); j++) {
			GLuint vao_id;
			glGenVertexArrays(1, &vao_id);
			glBindVertexArray(vao_id);
			tinygltf::Accessor index_accessor = m_model.accessors[m_model.meshes[i].primitives[j].indices];
			tinygltf::BufferView buffer_view = m_model.bufferViews[index_accessor.bufferView];
			int buffer_index = buffer_view.buffer;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[buffer_index]);
			for (auto attribute : m_model.meshes[i].primitives[j].attributes) {
				tinygltf::Accessor accessor = m_model.accessors[attribute.second];
				buffer_view = m_model.bufferViews[accessor.bufferView];
				buffer_index = buffer_view.buffer;
				glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer_index]);
				
				int size = 1;
				if (accessor.type != TINYGLTF_TYPE_SCALAR) {
					size = accessor.type;
				}
				int vaa = -1;
				if (attribute.first.compare("POSITION") == 0) vaa = 0;
				if (attribute.first.compare("NORMAL") == 0) vaa = 1;
				if (attribute.first.compare("TEXCOORD_0") == 0) vaa = 2;
				if (vaa > -1) {
					glEnableVertexAttribArray(vaa);
					glVertexAttribPointer(vaa, size, accessor.componentType,
						accessor.normalized ? GL_TRUE : GL_FALSE,
						buffer_view.byteStride, (const void*)(buffer_view.byteOffset + accessor.byteOffset));
				}
				else
					std::cout << "vaa missing: " << attribute.first << std::endl;
			}
			m_vaos.push_back(vao_id);
			m_primitives.push_back(m_model.meshes[i].primitives[j]);
		}
	}

	m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
	m_program.use();

	// Get Uniform Locations
	m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
	m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
	m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
	m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	m_uKdLocation = glGetUniformLocation(m_program.glId(), "uKd");

	m_uDirectionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
	m_uDirectionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

	m_uPointLightPosition = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
	m_uPointLightIntensity = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

	m_uKdSamplerLocation = glGetUniformLocation(m_program.glId(), "uKdSampler");

	
}