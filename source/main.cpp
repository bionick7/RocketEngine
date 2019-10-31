#include "physics/Celestial.h"
#include "graphics/UI/ui_canvas.h"

// Math stuff
using namespace glm;

double scroll_value = 0;
AssetSet* assets;
Settings* settings;

const std::string init_path = "init.cfgt";

void setup() {
	glfwWindowHint(GLFW_SAMPLES, 4); // antialaising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
}

void scroll_callback(GLFWwindow* window_ptx, double x, double y) {
	scroll_value += y;
}

static Orbiter* current_camera_focus = NULL;

void change_focus(Orbiter* new_focus) {
	current_camera_focus = new_focus;
}

int main(int argc, char** argv) {
	// Initialize GLFW
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n" << std::endl;
		return -1;
	}

	setup();

	if (!cfg::initialize(argv)) {
		std::cerr << "Couldn't initialize CFG reader" << std::endl;
		return -1;
	}

	cfg::DataStructure* main_structure = cfg::read_file(init_path, cfg::base_dir, L"Main", NULL, false);
	settings = new Settings(main_structure->get_child(L"settings"));

	// Open a window
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

	assets = new AssetSet(main_structure->get_child(L"assets"));
	Camera cam = Camera(window_ptx, &scroll_value);
	glfwSetScrollCallback(window_ptx, scroll_callback);

	cfg::DataStructure* sun_data = main_structure->get_child(L"celestial information")->get_child(L"Sun");

	Celestial sun = Celestial(sun_data);
	change_focus(&sun);

	glClearColor(settings->background.r, settings->background.g, settings->background.b, 0.0f);
	glLineWidth(settings->line_thickness);

	double prev_time = 0.0,
	       delta_t = 0.0;

	int orbiter_index = 0;
	bool enter_pressed = false;

	Circle c = Circle(1);
	c.world_position = vec3(0, 0, 0);

	Textmesh* tm = new Textmesh(TEXT(L"p enter"), assets->default_font);
	tm->set_transform(5, 5, -1, 20);

	//TextInput ti = TextInput(L"H",assets->default_font, window_ptx);
	//ti.set_transform(5, 500, -1, 20);

	// Main loop
	glfwSetInputMode(window_ptx, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Drawing stuff
		delta_t = glfwGetTime() - prev_time;
		prev_time = glfwGetTime();

		cam.update(delta_t);
		sun.update(delta_t);

		cam.focus(current_camera_focus->meta_position);

		sun.draw(&cam);
		tm->draw(&cam);
		//ti.draw(&cam);

		if (glfwGetKey(window_ptx, GLFW_KEY_ENTER) == GLFW_PRESS) {
			enter_pressed = true;
		}

		if (glfwGetKey(window_ptx, GLFW_KEY_ENTER) == GLFW_RELEASE) {
			if (enter_pressed) {
				orbiter_index = (orbiter_index + 1) % all_orbiters.size();
				change_focus(all_orbiters[orbiter_index]);
				tm->set_content(TEXT(all_orbiters[orbiter_index]->name));
			}
			enter_pressed = false;
		}

		// Swap buffers
		glfwSwapBuffers(window_ptx);
		glfwPollEvents();

	} while (glfwGetKey(window_ptx, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window_ptx) == 0);
}