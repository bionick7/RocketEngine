#include "ui_manager.h"
#include "graphic_manager.h"
#include "scene_manager.h"
#include "CameraContainer.h"

void scroll_callback(GLFWwindow* window_ptx, double x, double y) {
	*ui_manager->get_scroll_ptx() += y;
}

UIManager::UIManager() {
	window_ptx = nullptr;
	canvas = nullptr;
	scroll_value_ptx = new double;
	*scroll_value_ptx = 0;

	radio->add_signal("window_resized");
}

UIManager::~UIManager() {
	delete canvas;
	delete scroll_value_ptx;
}

void UIManager::setup(GLFWwindow* p_window_ptx) {
	window_ptx = p_window_ptx;

	glfwSetScrollCallback(window_ptx, scroll_callback);
	canvas = new UICanvas(window_ptx);
}

GLFWwindow* UIManager::get_window_ptx() {
	return window_ptx;
}

double* UIManager::get_scroll_ptx() {
	return scroll_value_ptx;
}

void UIManager::interprete_signal(RadioSignal signal) {

}

void UIManager::step(double dt) {
	Manager::step(dt);
	//scene_manager->main_camera->small_scale->on_window_resized();
	//scene_manager->main_camera->large_scale->on_window_resized();
	int w = 0, h = 0;
	glfwGetWindowSize(window_ptx, &w, &h);
	if (w != settings->width || h != settings->height) {
		std::cout << "Window resized" << std::endl;
		settings->width = w;
		settings->height = h;
		radio->emit_signal("window_resized");
	}
	canvas->draw();
}

UICanvas* UIManager::get_canvas()
{
	return canvas;
}
