#include "physics/Celestial.h"
#include "setups.h"
#include "CameraContainer.h"
#include "Blackboard.h"

double scroll_value = 0;
AssetSet* assets;
Settings* settings;
Blackboard* global_blackboard;
GraphicsServer* graphics_server;

void scroll_callback(GLFWwindow* window_ptx, double x, double y) {
	scroll_value += y;
}

int main(int argc, char** argv) {
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n" << std::endl;
	}

	setup_opengl();
	io::DataStructure* main_structure = setup_data(argc, argv);
	GLFWwindow* window_ptx = setup_window();

	assets = new AssetSet(main_structure->get_child("assets"));
	CameraContainer cam = CameraContainer(window_ptx, &scroll_value);
	glfwSetScrollCallback(window_ptx, scroll_callback);
	UICanvas* canvas = setup_canvas(window_ptx);

	glClearColor(settings->background.r, settings->background.g, settings->background.b, 0.0f);
	glLineWidth(settings->line_thickness);

	io::DataStructure* sun_data = main_structure->get_child("celestial information")->get_child("Sun");

	global_blackboard->main_camera = &cam;

	Celestial sun = Celestial(sun_data, nullptr);
	cam.focus(&sun); // default
	global_blackboard->hierarchy_entrypoint = &sun;

	double prev_time = 0.0,
	       delta_t = 0.0;

	// Main loop
	glfwSetInputMode(window_ptx, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Drawing stuff
		delta_t = glfwGetTime() - prev_time;
		prev_time = glfwGetTime();
		global_blackboard->time += delta_t * time_scale;

		// Pre-step all
		global_blackboard->hierarchy_entrypoint->pre_step(delta_t);

		if (cam.current_focus != nullptr) {
			cam.current_focus->step(delta_t);
		}
		cam.logic_step(delta_t);
		global_blackboard->hierarchy_entrypoint->step(delta_t);

		// Post-step all
		global_blackboard->hierarchy_entrypoint->post_step(delta_t);

		// Draw all
		graphics_server->draw();

		// Draw UI
		canvas->draw(delta_t);

		console_text += run_buffered_commands();

		// Swap buffers
		glfwSwapBuffers(window_ptx);
		glfwPollEvents();

		glfwSetWindowSize(window_ptx, settings->width, settings->height);


	} while (glfwGetKey(window_ptx, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_ptx) == 0);
}
