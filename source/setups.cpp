#include "setups.h"

UICanvas* setup_canvas(GLFWwindow* window, Camera* camera) {
	UICanvas* canvas = new UICanvas(window, camera);
	Console* console = new Console(assets->default_font, window);
	TextInput* inp = new TextInput("adawdw", assets->default_font, window);
	Graph* graph = new Graph();

	std::vector<float> data = std::vector<float>();
	for (float i = 0; i < 100.0f; i++) {
		data.push_back(sin(i / 20.0f));
	}

	graph->set_data(data, -5, 5);
	graph->set_transform(200, 200, 300, 320);
	inp->set_transform(700, 200, 300, 320);
	console->set_transform(100, -100, 300, 300);
	canvas->add(console);
	canvas->add(inp);
	canvas->add(graph);
	canvas->current_focus_input = console->input;

	return canvas;
}

void setup_opengl() {
	glfwWindowHint(GLFW_SAMPLES, 0); // antialaising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
}

GLFWwindow* setup_window() {
	GLFWwindow* window_ptx;
	window_ptx = glfwCreateWindow(settings->width, settings->height, "RocketEngine", NULL, NULL);
	if (window_ptx == NULL) {
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		throw - 1;
	}

	glfwMakeContextCurrent(window_ptx);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		throw - 1;
	}

	return window_ptx;
}

cfg::DataStructure* setup_data(char** args) {
	if (!cfg::initialize(args)) {
		std::cerr << "Couldn't initialize CFG reader" << std::endl;
	}

	cfg::DataStructure* main_structure = cfg::read_file(init_path, cfg::base_dir, "Main", NULL, false);
	settings = new Settings(main_structure->get_child("settings"));

	return main_structure;
}