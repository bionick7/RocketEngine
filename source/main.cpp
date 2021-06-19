#include "Celestial.h"
#include "setups.h"
#include "CameraContainer.h"
#include "audio_manager.h"

// Put testing code here. Gets run immediatly
void test_code() {
	return;

	char* buffer = new char[4];
	buffer[0] = 0x12;
	buffer[1] = 0x34;
	buffer[2] = 0xab;
	buffer[3] = 0xcd;
	int res = io::parse_binary_to_int(buffer, 4, false);
	std::cout << "Parse check: " << std::hex << res << std::dec << std::endl;
	delete[] buffer;
}

// Game specific
bool game_init(int argc, char** argv) {
	setup_opengl();

	io::DataStructurePtr main = setup_data(argc, argv);
	assets->root_ds = main;
	settings->reload(assets->root_ds->get_child("settings"));

	interprete_cmds(argc, argv);

	GLFWwindow* window_ptx = setup_window(settings->width, settings->height);
	if (window_ptx == nullptr) return false;

	ui_manager->setup(window_ptx);
	assets->reload(assets->root_ds->get_child("assets"));
	scripting_manager->load_modules(assets->root_ds->get_child("include"));
	graphics_manager->setup();
	audio_manager->setup();

	setup_canvas();
	scene_manager->init_scene(assets->root_ds);

	return true;
}

void game_loop(double dt) {
	audio_manager->step(dt);
	scripting_manager->step(dt);
	scene_manager->step(dt);
	graphics_manager->step(dt);
	ui_manager->step(dt);
	graphics_manager->post_processing();
}

bool should_exit() {
	if (glfwWindowShouldClose(ui_manager->get_window_ptx()) != 0) {
		return true;
	}
	return false;
}

AssetSet* assets = nullptr;
Settings* settings = nullptr;

GraphicsManager* graphics_manager = nullptr;
UIManager* ui_manager = nullptr;
SceneManager* scene_manager = nullptr;
ScriptingManager* scripting_manager = nullptr;
AudioManager* audio_manager = nullptr;

void test_lifecycle(int argc, char** argv) {
	setup_opengl();
	setup_window(100, 100);

	for (int i = 0; i < 100; i++) {		
		assets = new AssetSet();
		settings = new Settings();
		graphics_manager = new GraphicsManager();
		//audio_manager = new AudioManager();

		assets->root_ds = setup_data(argc, argv);
		assets->reload(assets->root_ds->get_child("assets"));
		settings->reload(assets->root_ds->get_child("settings"));
		
		graphics_manager->setup();
		//audio_manager->setup();

		delete assets;
		delete settings;
		delete graphics_manager;
		//delete audio_manager;
	}
}

int main(int argc, char** argv) {
	if (!glfwInit()) std::cerr << "Failed to initialize GLFW\n" << std::endl;

	//test_lifecycle(argc, argv);
	//return 0;

	graphics_manager = new GraphicsManager();
	scene_manager = new SceneManager();
	scripting_manager = new ScriptingManager();
	audio_manager = new AudioManager();
	ui_manager = new UIManager();
	assets = new AssetSet();
	settings = new Settings();

	if (!game_init(argc, argv)) return -1;
	
	test_code();
	
	double prev_time = 0.0,
	       delta_t = 0.0;

	do {
		delta_t = glfwGetTime() - prev_time;
		prev_time = glfwGetTime();

		game_loop(delta_t);

		// Swap buffers
		glfwSwapBuffers(ui_manager->get_window_ptx());
		glfwPollEvents();
	} while (!should_exit());
	return 0;
}
