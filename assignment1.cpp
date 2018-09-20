#include "config.hpp"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/WindowManager.hpp"

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>

#include <stack>

#include <cstdlib>


int main()
{
	//
	// Set up the logging system
	//
	Log::Init();
	Log::View::Init();

	//
	// Set up the camera
	//
	InputHandler input_handler;
	FPSCameraf camera(0.5f * glm::half_pi<float>(),
	                  static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	                  0.01f, 1000.0f);
	camera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	camera.mMouseSensitivity = 0.003f;
	camera.mMovementSpeed = 0.25f * 12.0f;

	//
	// Set up the windowing system and create the window
	//
	WindowManager window_manager;
	WindowManager::WindowDatum window_datum{ input_handler, camera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};
	GLFWwindow* window = window_manager.CreateWindow("EDAF80: Assignment 1", window_datum, config::msaa_rate);
	if (window == nullptr) {
		LogError("Failed to get a window: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}

	//
	// Load the sphere geometry
	//
	std::vector<bonobo::mesh_data> const objects = bonobo::loadObjects("sphere.obj");
	if (objects.empty()) {
		LogError("Failed to load the sphere geometry: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}
	bonobo::mesh_data const& sphere = objects.front();


	//
	// Create the shader program
	//
	ShaderProgramManager program_manager;
	GLuint shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/default.vert" },
	                                           { ShaderType::fragment, "EDAF80/default.frag" } },
	                                         shader);
	if (shader == 0u) {
		LogError("Failed to generate the shader program: exiting.");

		Log::View::Destroy();
		Log::Destroy();

		return EXIT_FAILURE;
	}


	//
	// Set up the sun node and other related attributes
	//
	Node sun_node;
	sun_node.set_geometry(sphere);
	GLuint const sun_texture = bonobo::loadTexture2D("sunmap.png");
	sun_node.add_texture("diffuse", sun_texture, GL_TEXTURE_2D);
	float const sun_spin_speed = glm::two_pi<float>() / 000020.0f; // Full rotation in 20 seconds


	Node solar_system_node;
	solar_system_node.add_child(&sun_node);
	float const solar_system_spin_speed = glm::two_pi<float>() / 50.0f; // Full rotation in 50 seconds

	Node earth_pivot;
	solar_system_node.add_child(&earth_pivot);

	
	
	//solar_system_node.set_geometry(sphere);
	//solar_system_node.set_translation(glm::vec3(1.0, 0.0, 2.0));

	Node earth_translation;
	glm::vec3 earth_translate_vec = glm::vec3(3.0, 1.0, 1.0);
	earth_translation.translate(earth_translate_vec);
	earth_pivot.add_child(&earth_translation);

	Node earth_spin;
	earth_spin.set_geometry(sphere);
	GLuint const earth_texture = bonobo::loadTexture2D("earthmap1k.png");
	earth_spin.add_texture("diffuse_texture", earth_texture, GL_TEXTURE_2D);
	float const earth_spin_speed = glm::two_pi<float>() / 6.0f; // Full rotation in six seconds

	glm::vec3 earth_scale_vec = glm::vec3(0.3, 0.3, 0.3);
	earth_spin.scale(earth_scale_vec);
	earth_translation.add_child(&earth_spin);

	Node moon_pivot;
	earth_translation.add_child(&moon_pivot);

	Node moon_node;
	moon_node.set_geometry(sphere);
	GLuint const moon_texture = bonobo::loadTexture2D("moonmap4k.png");
	moon_node.add_texture("diffuse_texture", moon_texture, GL_TEXTURE_2D);
	float const moon_spin_speed = earth_spin_speed;
	//moon_node.set_translation(earth_node.get_translation + glm::vec3(0.1, 0.1, 0.1));
	//moon_node.translate(glm::vec3(0.3, 0.2, 0.2));
	moon_node.translate(glm::vec3(0.3, 0.2, 0.2));
	moon_node.scale(earth_scale_vec * glm::float32(0.27));
	moon_pivot.add_child(&moon_node);
	

	
	Node venus_node;
	venus_node.set_geometry(sphere);
	GLuint const venus_texture = bonobo::loadTexture2D("venusmap.png");
	venus_node.add_texture("diffuse_texture", venus_texture, GL_TEXTURE_2D);
	float const venus_spin_speed = -earth_spin_speed/243; // https://www.space.com/18530-how-big-is-venus.html
	//glm::vec3 earth_translate_vec = glm::vec3(3.0, 1.0, 1.0);
	venus_node.translate(glm::vec3(2.0, -1.5, 1.0));

	Node venus_pivot;
	solar_system_node.add_child(&venus_pivot);

	venus_node.scale(glm::float32(0.95)*earth_scale_vec);
	venus_pivot.add_child(&venus_node);
	
	
	
	//
	// TODO: Create nodes for the remaining of the solar system
	//


	glViewport(0, 0, config::resolution_x, config::resolution_y);
	glClearDepthf(1.0f);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	size_t fpsSamples = 0;
	double lastTime = GetTimeSeconds();
	double fpsNextTick = lastTime + 1.0;


	bool show_logs = true;
	bool show_gui = true;

	while (!glfwWindowShouldClose(window)) {
		//
		// Compute timings information
		//
		double const nowTime = GetTimeSeconds();
		double const delta_time = nowTime - lastTime;
		lastTime = nowTime;
		if (nowTime > fpsNextTick) {
			fpsNextTick += 1.0;
			fpsSamples = 0;
		}
		++fpsSamples;
		


		//
		// Process inputs
		//
		glfwPollEvents();

		ImGuiIO const& io = ImGui::GetIO();
		input_handler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);
		input_handler.Advance();
		camera.Update(delta_time, input_handler);

		if (input_handler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (input_handler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;


		//
		// Start a new frame for Dear ImGui
		//
		ImGui_ImplGlfwGL3_NewFrame();


		//
		// Clear the screen
		//
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		//
		// Update the transforms
		//
		sun_node.rotate_y(sun_spin_speed * delta_time);
		earth_spin.rotate_y(earth_spin_speed * delta_time);
		//earth_translate_vec = glm::vec3(3.0 * std::sin(nowTime/2.0), std::sin(nowTime/2.0), 3.0 * std::cos(nowTime/2.0));
		//earth_node.set_translation(earth_translate_vec);
		//moon_node.set_translation(glm::vec3( 0.4 *std::sin(4.0*nowTime), 0.2*std::sin(4.0*nowTime), 0.4*std::cos(4.0*nowTime)));
		moon_node.rotate_y(moon_spin_speed*delta_time);
		moon_pivot.rotate_y(glm::two_pi<float>() / 50.0f*delta_time);
		//venus_node.set_translation(glm::vec3((107.0 / 149.6)*3.0*std::sin(nowTime*0.615/2.0), -0.3*std::sin(nowTime*0.615), (107.0 / 149.6)*3.0*std::cos(nowTime*0.615/2.0)));
		//https://space-facts.com/venus/
		venus_node.rotate_y(venus_spin_speed*delta_time);
		solar_system_node.rotate_y(solar_system_spin_speed * delta_time);

		//
		// Traverse the scene graph and render all nodes
		//
		std::stack<Node const*> node_stack({ &solar_system_node });
		std::stack<glm::mat4> matrix_stack({ glm::mat4(1.0f) });
		// TODO: Replace this explicit rendering of the Sun with a
		// traversal of the scene graph and rendering of all its nodes.
		
		Node const* current_node = &solar_system_node;
		//int nbrOfChildren = solar_system_node.get_children_nb();
		while (!node_stack.empty()) {
			current_node = node_stack.top();
			glm::mat4 matrix_stack_transform = matrix_stack.top()*current_node->get_transform();
			
			current_node->render(camera.GetWorldToClipMatrix(), matrix_stack_transform, shader, [](GLuint /*program*/) {});
			node_stack.pop();
			matrix_stack.pop();
			
			for (int i = 0; i < current_node->get_children_nb(); i++) {
				Node const* child = current_node->get_child(i);
				matrix_stack.push(matrix_stack_transform);
				node_stack.push(child);
			}
		
		}
			//kolla om child finns
			// gå till child 
			// kolla om child finns
			//ingen finns, pusha objektet, gå upp ett steg

			
			
		// 
		
		
		//sun_node.render(camera.GetWorldToClipMatrix(), sun_node.get_transform(), shader, [](GLuint /*program*/){});
		//earth_node.render(camera.GetWorldToClipMatrix(), earth_node.get_transform(), shader, [](GLuint /*program*/) {});
		//moon_node.render(camera.GetWorldToClipMatrix(), moon_node.get_transform(), shader, [](GLuint /*program*/) {});
		//venus_node.render(camera.GetWorldToClipMatrix(), venus_node.get_transform(), shader, [](GLuint /*program*/) {});
		//solar_system_node.render(camera.GetWorldToClipMatrix(), solar_system_node.get_transform(), shader, [](GLuint /*program*/) {});

		//
		// Display Dear ImGui windows
		//
		if (show_logs)
			Log::View::Render();
		if (show_gui)
			ImGui::Render();


		//
		// Queue the computed frame for display on screen
		//
		glfwSwapBuffers(window);
	}

	glDeleteTextures(1, &sun_texture);


	Log::View::Destroy();
	Log::Destroy();

	return EXIT_SUCCESS;
}
