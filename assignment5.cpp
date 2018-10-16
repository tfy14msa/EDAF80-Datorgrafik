#include "assignment5.hpp"

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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include <cstdlib>

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>
#include <tinyfiledialogs.h>






#include <stdexcept>

edaf80::Assignment5::Assignment5() :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(), window(nullptr)
{
	Log::View::Init();

	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		Log::View::Destroy();
		throw std::runtime_error("Failed to get a window: aborting!");
	}
}

bool testCollison(glm::vec3 p1, float r1, glm::vec3 p2, float r2)
{
	auto p = p1 - p2;
	//float psum = 0;
	//for (int i = 0; i < p1.length();i++) {
	//	psum += p[i] * p[i];
	//}
	//psum = sqrt(psum);
	float psum = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
	return psum < r1 + r2;
}

Node createAsteroid(Node area, unsigned int area_radius, unsigned int max_radius) {

	auto asteroid = Node();
	
	int x = rand(), y = rand(), z = rand();
	x = x % (2 * area_radius) - area_radius,
		y = y % (2 * area_radius) - area_radius,
		z = z % (2 * area_radius) - area_radius;
	/*float x = -area_radius + rand() % (2*area_radius);
	float y = -area_radius + rand() % (2*area_radius);
	float z = -area_radius + rand() % (2*area_radius );*/

	auto init_pos = glm::vec3(x, y, z);
	asteroid.set_translation(init_pos);

	int radius = rand() % max_radius;
	auto const asteroid_shape = parametric_shapes::createSphere(300.0f, 300.0f, radius);
	if (asteroid_shape.vao == 0u) {
		LogError("Failed to retrieve the sphere mesh");
	}
	asteroid.set_geometry(asteroid_shape);

	GLuint const asteroid_texture = bonobo::loadTexture2D("venusmap.png");
	asteroid.add_texture("diffuse", asteroid_texture, GL_PROXY_TEXTURE_2D);
	return asteroid;
}

edaf80::Assignment5::~Assignment5()
{
	Log::View::Destroy();
}

void
edaf80::Assignment5::run()
{


	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f));
	mCamera.mMouseSensitivity = 0.003f;
	mCamera.mMovementSpeed = 0.25;

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
	GLuint default_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/default.vert" },
	{ ShaderType::fragment, "EDAF80/default.frag" } },
		default_shader);
	if (default_shader == 0u)
		LogError("Failed to load default shader");
	GLuint diffuse_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/diffuse.vert" },
	{ ShaderType::fragment, "EDAF80/diffuse.frag" } },
		diffuse_shader);
	if (diffuse_shader == 0u)
		LogError("Failed to load diffuse shader");
	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/cubemap.vert" },
	{ ShaderType::fragment, "EDAF80/cubemap.frag" } },
		skybox_shader);
	if (skybox_shader == 0u)
		LogError("Failed to load skybox shader");

	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	auto camera_position = mCamera.mWorld.GetTranslation();
	auto light_position = glm::vec3(-2.0f, 8.0f, 2.0f)*1000.0f;
	auto const set_uniforms = [&light_position, &camera_position](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
	};


	//
	// Todo: Load your geometry
	//
	// Create the triangle
	float tr_sides = 20.0f;
	auto ship = Node();
	auto const triangle_shape = //parametric_shapes::createSphere(100.0f, 100.0f, 5.0f);
		parametric_shapes::createTriangle(tr_sides, tr_sides, 100.0f);
	if (triangle_shape.vao == 0u) {
		LogError("Failed to retrieve the quad mesh");
		return;
	}
	float area_radius = 500.0f;
	auto const sphere_shape = parametric_shapes::createSphere(300.0f, 300.0f, area_radius);
	if (sphere_shape.vao == 0u) {
		LogError("Failed to retrieve the sphere mesh");
		return;
	}


	//
	// Load the sphere geometry
	//
	/*std::vector<bonobo::mesh_data> const objects = bonobo::loadObjects("spaceship.obj");
	if (objects.empty()) {
		LogError("Failed to load the sphere geometry: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}
	bonobo::mesh_data const& spaceship = objects.front();*/



	ship.set_geometry(triangle_shape);
	//ship.set_geometry(spaceship);
	ship.set_program(&fallback_shader, set_uniforms);
	
	ship.rotate_x(glm::two_pi<float>()/4.0f);
	ship.rotate_y(glm::two_pi<float>() / 2.0f);
	auto area = Node();
	area.set_geometry(sphere_shape);


	std::string stringe = "snow";
	auto my_reflection_cube_id = bonobo::loadTextureCubeMap(stringe + "/posx.png", stringe + "/negx.png",
		stringe + "/posy.png", stringe + "/negy.png", stringe + "/posz.png", stringe + "/negz.png", true);
	if (my_reflection_cube_id == 0u) {
		LogError("Failed to load my_cube_map texture");
		return;
	}
	area.add_texture("my_reflection_cube", my_reflection_cube_id, GL_TEXTURE_CUBE_MAP);

	GLuint const ripple_texture = bonobo::loadTexture2D("waves.png");
	area.add_texture("my_ripple", ripple_texture, GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	GLuint const ship_texture = bonobo::loadTexture2D("SWtie.png");
	ship.add_texture("diffuse", ship_texture, GL_TEXTURE_2D);



	float ttime = 0.0f;
	auto const water_set_uniforms = [&light_position, &camera_position, &ttime](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform1f(glGetUniformLocation(program, "ttime"), ttime);
	};

	ship.set_program(&fallback_shader, set_uniforms);
	area.set_program(&skybox_shader, set_uniforms);

	Node asteroid = createAsteroid(area, area_radius, 50u);
	area.add_child(&asteroid);

	glEnable(GL_DEPTH_TEST);

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

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(ddeltatime, inputHandler);

		if (inputHandler.GetKeycodeState(GLFW_KEY_1) & JUST_PRESSED) {
			ship.set_program(&fallback_shader, set_uniforms);
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_2) & JUST_PRESSED) {
			ship.set_program(&default_shader, set_uniforms);
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_3) & JUST_PRESSED) {
			ship.set_program(&diffuse_shader, set_uniforms);
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_4) & JUST_PRESSED) {
			ship.set_program(&skybox_shader, water_set_uniforms);
		}
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



		camera_position = mCamera.mWorld.GetTranslation();





		float anglex = mCamera.mRotation.x + 1.57; // The angle of the rotation is always 90 degrees. Minus/plus from direction. 
		float cosvalx = std::cos(anglex);
		float sinvalx = std::sin(anglex);
		float angley = mCamera.mRotation.y - 1.57;
		float cosvaly = std::cos(angley);
		float sinvaly = std::sin(angley);

		// Why reserved y and z? Because the angle is measured in xz plane.
		// Why minus in formula? It eihter has to be minus in x if
		// +1.57 in anglex, or minus in z and -1.57 in anglex. You choose.

		float x = camera_position.x - 31.0f*sinvaly*cosvalx;
		float z = camera_position.z + 31.0f*sinvaly*sinvalx;
		float y = camera_position.y + 31.0f*cosvaly;
		//float x = camera_position.x - 31.0f*(cosvalx); // Minus due to using z and x axis. 
		//float z = camera_position.z + 31.0f*(sinvalx);

		float pi_half = glm::half_pi<float>();
		float pi = glm::pi<float>();

		//ship.set_translation(glm::vec3(camera_position.x, camera_position.y, camera_position.z - 31.0f));
		auto ship_position = glm::vec3(x, y - 9.0f, z + tr_sides / 16.0f);
		ship.set_translation(ship_position);
		ship.set_rotation_x(-mCamera.mRotation.y + pi_half);
		//mCamera.mWorld.LookAt(glm::vec3(camera_position.x, camera_position.y, camera_position.z - 31.0f));
		ship.set_rotation_y(mCamera.mRotation.x + pi);

		if (!testCollison(ship_position, tr_sides, glm::vec3(0.0f,0.0f,0.0f), area_radius)) {
			printf("\n You suck!! \n Ship crashed");
			//break;
			mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f));
		}


		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			ship.render(mCamera.GetWorldToClipMatrix(), ship.get_transform());
			area.render(mCamera.GetWorldToClipMatrix(), area.get_transform());
			
			// Traverse the scene graph and render all nodes
			//
			std::stack<Node const*> node_stack({ &area });
			std::stack<glm::mat4> matrix_stack({ glm::mat4(1.0f) });

			Node const* current_node = &area;

			while (!node_stack.empty()) {
				current_node = node_stack.top();
				glm::mat4 matrix_stack_transform = matrix_stack.top()*current_node->get_transform();

				if (current_node != &area) {
					current_node->render(mCamera.GetWorldToClipMatrix(), matrix_stack_transform, default_shader, [](GLuint /*program*/) {});
				}
				node_stack.pop();
				matrix_stack.pop();

				for (int i = 0; i < current_node->get_children_nb(); i++) {
					Node const* child = current_node->get_child(i);
					matrix_stack.push(matrix_stack_transform);
					node_stack.push(child);
				}

			}
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
		edaf80::Assignment5 assignment5;
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
	Bonobo::Destroy();
}
