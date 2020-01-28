#include "physics/Celestial.h"
#include "setups.h"

double scroll_value = 0;
AssetSet* assets;
Settings* settings;

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
	cfg::DataStructure* main_structure = setup_data(argv);
	GLFWwindow* window_ptx = setup_window();

	assets = new AssetSet(main_structure->get_child("assets"));
	Camera cam = Camera(window_ptx, &scroll_value);
	glfwSetScrollCallback(window_ptx, scroll_callback);
	UICanvas* canvas = setup_canvas(window_ptx, &cam);

	glClearColor(settings->background.r, settings->background.g, settings->background.b, 0.0f);
	glLineWidth(settings->line_thickness);

	cfg::DataStructure* sun_data = main_structure->get_child("celestial information")->get_child("Sun");

	Celestial sun = Celestial(sun_data);

	double prev_time = 0.0,
	       delta_t = 0.0;

	// Main loop
	glfwSetInputMode(window_ptx, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Drawing stuff
		delta_t = glfwGetTime() - prev_time;
		prev_time = glfwGetTime();

		cam.update(delta_t);
		sun.update(delta_t);

		sun.draw(&cam);
		canvas->draw(delta_t);

		// Swap buffers
		glfwSwapBuffers(window_ptx);
		glfwPollEvents();

		glfwSetWindowSize(window_ptx, settings->width, settings->height);
		//cam.screensize = { settings->width, settings->height };
	} while (glfwGetKey(window_ptx, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_ptx) == 0);
}