#include "setups.h"
#include "console_logic.h"

UICanvas* setup_canvas(GLFWwindow* window) {
	UICanvas* canvas = new UICanvas(window);
	Console* console = new Console((Font*)assets->get_default(io::ResourceType::FONT), window);
	Graph* graph = new Graph();

	std::vector<float> data = std::vector<float>();
	for (float i = 0; i < 100.0f; i++) {
		data.push_back(sin(i / 20.0f));
	}

	graph->set_data(data, -5, 5);
	graph->set_transform(200, 200, 300, 320);
	console->set_transform(100, -100, 300, 300);
	canvas->add(console);
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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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

io::DataStructure* setup_data(int arg_count, char** args) {
	if (!io::initialize(arg_count, args)) {
		std::cerr << "Couldn't initialize CFG reader" << std::endl;
	}

	io::DataStructure* main_structure = io::read_file(init_path, io::base_dir, "Main", NULL, false);
	settings = new Settings(main_structure->get_child("settings"));
	global_blackboard = new Blackboard();
	graphics_server = new GraphicsServer();

	std::cout << "CM Arguments" << std::endl;
	for (int i = 0; i < arg_count; i++) {
		if (std::string(args[i]) == std::string("-startcommands")) {
			std::string filepath = args[++i];
			bool success;
			std::vector<std::string> lines = io::read_lines_from_file(filepath, io::base_dir, &success, false);
			if (success) {
				for (std::string line : lines) {
					console_interpreteation(line);
				}
			}
			else {
				std::cerr << "Couldn't open " << filepath << std::endl;
			}
			std::cout << "\t- " << "command entry point: " << filepath << std::endl;
		}
	}
	std::cout << " ---- " << std::endl;

	return main_structure;
}