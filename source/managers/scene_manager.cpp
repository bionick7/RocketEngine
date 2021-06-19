#include "scene_manager.h"
#include "scripting_manager.h"
#include "Celestial.h"
#include "CameraContainer.h"

SceneManager::SceneManager() {
	orbit_root = nullptr;
	main_camera = nullptr;

	time_scale = 1e4;
	time = 0;
	time_delta = 0;
	real_time = 0;
	real_time_delta = 0;
}

SceneManager::~SceneManager() {

}

void SceneManager::init_scene(io::DataStructurePtr ds) {
	// Setup hierarchy
	CameraContainer* cam = new CameraContainer();
	io::DataStructurePtr sun_data = ds->get_child("celestial information")->get_child(ds->get_string("entry point"));
	scene_manager->main_camera = cam;
	Celestial* sun = new Celestial(sun_data, nullptr);
	cam->focus(sun); // default
	scene_manager->orbit_root = sun;

	scripting_manager->run_on_scene_load();
}

void SceneManager::interprete_signal(RadioSignal signal) {

}

void SceneManager::step(double dt) {
	Manager::step(dt);
	time_delta = dt * time_scale;
	time += time_delta;

	real_time_delta = dt;
	real_time += real_time_delta;

	for (Agent* a : loose_agents) {
		a->step(dt);
	}

	// Pre-step all
	orbit_root->pre_step(dt); // Physics

	if (main_camera->current_focus != nullptr) {
		main_camera->current_focus->step(dt);	// Logic
	}
	main_camera->logic_step(dt);
	orbit_root->step(dt);	// Drawing

	// Post-step all
	orbit_root->post_step(dt);
}