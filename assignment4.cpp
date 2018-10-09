#include "assignment4.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/ShaderProgramManager.hpp"
#include "core/node.hpp"
#include "interpolation.hpp"
#include "parametric_shapes.hpp"
#include <glm/glm.hpp>


#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>
#include <tinyfiledialogs.h>

#include <stdexcept>



#include "assignment3.hpp"
#include "interpolation.hpp"
#include "parametric_shapes.hpp"


#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tinyfiledialogs.h>

#include <cstdlib>
#include <stdexcept>

edaf80::Assignment4::Assignment4() :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(), window(nullptr)
{
	Log::View::Init();

	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateWindow("EDAF80: Assignment 4", window_datum, config::msaa_rate);
	if (window == nullptr) {
		Log::View::Destroy();
		throw std::runtime_error("Failed to get a window: aborting!");
	}
}

edaf80::Assignment4::~Assignment4()
{
	Log::View::Destroy();
}

void
edaf80::Assignment4::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mMouseSensitivity = 0.003f;
	mCamera.mMovementSpeed = 0.025;

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/fallback.vert" },
	                                           { ShaderType::fragment, "EDAF80/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//
	GLuint diffuse_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/diffuse.vert" },
	{ ShaderType::fragment, "EDAF80/diffuse.frag" } },
		diffuse_shader);
	if (diffuse_shader == 0u)
		LogError("Failed to load diffuse shader");
	GLuint default_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/default.vert" },
	{ ShaderType::fragment, "EDAF80/default.frag" } },
		default_shader);
	if (default_shader == 0u)
		LogError("Failed to load default shader");
	/*GLuint water_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/water.vert" },
	{ ShaderType::fragment, "EDAF80/water.frag" } },
		water_shader);
	if (water_shader == 0u)
		LogError("Failed to load water shader");*/

	//
	// Todo: Load your geometry
	//

	auto const quad_shape = parametric_shapes::createQuad(20u, 20u);
	if (quad_shape.vao == 0u) {
		LogError("Failed to retrieve the quad mesh");
		return;
	}
	auto const sphere_shape = parametric_shapes::createSphere(20u, 20u,20.0f);
	if (sphere_shape.vao == 0u) {
		LogError("Failed to retrieve the sphere mesh");
		return;
	}

	auto quad = Node();
	quad.set_geometry(quad_shape);

	glEnable(GL_DEPTH_TEST);

	auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f)*10.0f;
	auto const set_uniforms = [&light_position](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
	};

	float ttime = 0.0f;
	auto const water_set_uniforms = [&light_position,&ttime](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform1f(glGetUniformLocation(program, "ttime"), ttime);
	};

	quad.set_program(&fallback_shader, set_uniforms);

	GLuint const earth_texture = bonobo::loadTexture2D("earth_diffuse.png");
	quad.add_texture("my_diffuse", earth_texture, GL_TEXTURE_2D);

	// Enable face culling to improve performance:
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);


	f64 ddeltatime;
	size_t fpsSamples = 0;
	double nowTime, lastTime = GetTimeMilliseconds();
	double fpsNextTick = lastTime + 1000.0;

	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;

	while (!glfwWindowShouldClose(window)) {
		nowTime = GetTimeMilliseconds();
		ddeltatime = nowTime - lastTime;
		if (nowTime > fpsNextTick) {
			fpsNextTick += 1000.0;
			fpsSamples = 0;
		}
		fpsSamples++;
		ttime += ddeltatime / 1000;
		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(ddeltatime, inputHandler);

		if (inputHandler.GetKeycodeState(GLFW_KEY_1) & JUST_PRESSED) {
			quad.set_program(&fallback_shader, set_uniforms);
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_2) & JUST_PRESSED) {
			quad.set_program(&default_shader, set_uniforms);
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_3) & JUST_PRESSED) {
			quad.set_program(&diffuse_shader, set_uniforms);
		}
		/*if (inputHandler.GetKeycodeState(GLFW_KEY_5) & JUST_PRESSED) {
			quad.set_program(&water_shader, water_set_uniforms);
		}*/
		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
				                   "An error occurred while reloading shader programs; see the logs for details.\n"
				                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
				                   "error");
		}

		ImGui_ImplGlfwGL3_NewFrame();

		//
		// Todo: If you need to handle inputs, you can do it here
		//


		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);
		glClearDepthf(1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (!shader_reload_failed) {
			quad.render(mCamera.GetWorldToClipMatrix(), quad.get_transform());
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//

		if (show_logs)
			Log::View::Render();
		if (show_gui)
			ImGui::Render();

		glfwSwapBuffers(window);
		lastTime = nowTime;
	}
}

int main()
{
	Bonobo::Init();
	try {
		edaf80::Assignment4 assignment4;
		assignment4.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
	Bonobo::Destroy();
}
