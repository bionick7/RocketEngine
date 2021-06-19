#include "setups.h"
#include "graph.h"

void setup_canvas() {
	Console* console = new Console(assets->get_default<Font>(io::ResourceType::FONT), ui_manager->get_window_ptx());
	Graph* graph = new Graph();


	std::vector<float> data = std::vector<float>();
	for (float i = 0; i < 100.0f; i++) {
		data.push_back(sin(i / 20.0f));
	}

	UICanvas* canvas = ui_manager->get_canvas();

	// TODO Anchor system
	console->set_transform_anchor_margin(0, 300, 0, 0, 0, 0, 1, 0);
	console->movable = false;
	canvas->add(console);

	graph->set_data(data);
	graph->set_transform(0, 300, 600, 300);
	//canvas->add(graph);
	//canvas->current_focus_input = console->input;

	// TODO Anchor system
	Textmesh* text = new Textmesh(
		"the quick brown fox jumps over a lazy dog \n THE QUICK BROWN FOX JUMPS OVER A LAZY DOG \n 0123456789 +-*/=\n@!\"§$%&/()=?\\",
		assets->get_default<Font>(io::ResourceType::FONT)
	);
	text->set_transform_anchor_margin(300, 600, 0, -300, 0, 0, 1, 1);
	text->movable = true;
	canvas->add(text);
}

void setup_opengl() {
	glfwWindowHint(GLFW_SAMPLES, 0); // antialaising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	//glDepthMask(GL_TRUE);
	//glDepthFunc(GL_LESS);
}

GLFWwindow* setup_window(int width, int height) {
	GLFWwindow* window_ptx = glfwCreateWindow(width, height, "RocketEngine", nullptr, nullptr);
	if (window_ptx == nullptr) {
		errlog("Failed to open GLFW window");
		glfwTerminate();
	}

	glfwMakeContextCurrent(window_ptx);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		errlog("Failed to initialize GLEW");
	}

	return window_ptx;
}

io::DataStructurePtr setup_data(int arg_count, char** args) {
	if (!io::initialize(arg_count, args)) {
		errlog("Couldn't initialize CFG reader");
	}
	return io::read_file(init_path, io::base_dir, "Main", nullptr, false);
}

void interprete_cmds(int arg_count, char** args) {
	std::cout << "CM Arguments" << std::endl;
	for (int i = 0; i < arg_count; i++) {
		if (std::string(args[i]) == std::string("-startcommands")) {
			std::string filepath = args[++i];
			bool success;
			std::vector<std::string> lines = io::read_lines_from_file(filepath, io::base_dir, &success, false);
			if (success) {
				// Run boot commands
				for (std::string line : lines) {
					if (line[0] != '/' || line[1] != '/') {
						if (scripting_manager != nullptr) {
							scripting_manager->console_interpreteation(line);
						}
					}
				}
			}
			else {
				errlog("Couldn't open " + filepath);
			}
			std::cout << "\t- " << "command entry point: " << filepath << std::endl;
		}
	}
	std::cout << " ---- " << std::endl;
}