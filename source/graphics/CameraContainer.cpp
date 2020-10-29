#include "CameraContainer.h"

CameraContainer::CameraContainer(GLFWwindow* _window_ptx, double* _scroll_ptx) : Agent() {
	scroll_ptx = _scroll_ptx;
	window_ptx = _window_ptx;

	int width, height;
	glfwGetWindowSize(window_ptx, &width, &height);

	small_scale = Camera(width, height);
	large_scale = Camera(width, height);
	graphics_server->all_cameras.push_back(&small_scale);
	graphics_server->all_cameras.push_back(&large_scale);

	small_scale.render_layers = 2;

	horizontal_angle = 0;
	vertical_angle = 0;

	mouse_speed = settings->mouse_sensitivity;
	initial_fov = settings->initial_fov;
	min_vertical_angle = settings->min_vertical_angle;
	max_vertical_angle = settings->max_vertical_angle;
	scroll_sensitivity = settings->scroll_sensitivity;

	small_scale.fov = large_scale.fov = initial_fov;
	small_scale.recalculate_vp();
	large_scale.recalculate_vp();

	small_scale.distance = 1/position_scale;
}

CameraContainer::~CameraContainer() {

}

void CameraContainer::logic_step(double dt) {
	if (is_interpolating) {
		interpolation_step(dt);
	}

	int width, height;
	glfwGetWindowSize(window_ptx, &width, &height);
	small_scale.screensize = large_scale.screensize = Screenpos(width, height);

	if (glfwGetMouseButton(window_ptx, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (!is_rotating) {
		//	glfwSetCursorPos(camera->window_ptx, camera->screensize.x / 2, camera->screensize.y / 2);
			glfwGetCursorPos(window_ptx, &prev_mouse_x, &prev_mouse_y);
		}

		is_rotating = true;
	}
	else if (glfwGetMouseButton(window_ptx, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		is_rotating = false;
	}

	if (is_rotating) {
		update_rotation(dt, window_ptx);
	}

	large_scale.distance = exp(scroll_sensitivity * -*scroll_ptx);
	large_scale.near_clip = std::max(1e-6, large_scale.distance / 100);
	large_scale.far_clip = std::max(1.0, large_scale.distance * 1000);

	small_scale.distance = exp(scroll_sensitivity * -*scroll_ptx) / position_scale;
	small_scale.near_clip = small_scale.distance / 100;
	small_scale.far_clip = small_scale.distance * 1000;

	glm::vec3 move_dir = glm::normalize(glm::vec3(large_scale.direction.x, 0, large_scale.direction.z));
	glm::vec3 move_right = glm::normalize(glm::cross(move_dir, large_scale.up));

	small_scale.recalculate_vp();
	large_scale.recalculate_vp();
}

void CameraContainer::update_rotation(double dt, GLFWwindow* window_ptx) {

	double mouse_x, mouse_y;
	glfwGetCursorPos(window_ptx, &mouse_x, &mouse_y);
	//glfwSetCursorPos(window_ptx, camera->screensize.x / 2, camera->screensize.y / 2);

	horizontal_angle += mouse_speed * dt * (prev_mouse_x - mouse_x);
	vertical_angle += mouse_speed * dt * (prev_mouse_y - mouse_y);

	if (vertical_angle >= max_vertical_angle) {
		vertical_angle = max_vertical_angle;
	}
	else if (vertical_angle <= min_vertical_angle) {
		vertical_angle = min_vertical_angle;
	}

	recalculate_orientation();

	prev_mouse_x = mouse_x;
	prev_mouse_y = mouse_y;
}

void CameraContainer::recalculate_orientation() {
	small_scale.direction = large_scale.direction = glm::vec3(focus_transform * glm::vec4(
		cos(vertical_angle) * sin(horizontal_angle),
		sin(vertical_angle),
		cos(vertical_angle) * cos(horizontal_angle),
		1.0
	));

	glm::vec3 right = glm::vec3(focus_transform * glm::vec4(
		sin(horizontal_angle - pi_halfs),
		0.0,
		cos(horizontal_angle - pi_halfs),
		1.0
	));

	small_scale.up = large_scale.up = glm::cross(right, large_scale.direction);
}

void CameraContainer::interpolation_step(double dt) {
	interpolation_value += dt;
	if (interpolation_value >= 1.0) {
		focus_offset = LongVector(0.0, 0.0, 0.0);
		focus_transform = current_focus->get_focus_transformation();
		is_interpolating = false;
		recalculate_orientation();
		return;
	}
	if (interpolation_value < .5) {
		focus_offset = (interpolation_focus->position - current_focus->position) * interpolation_value * interpolation_value * 2;
		focus_transform = interpolate_matrix(
			current_focus->get_focus_transformation(),
			interpolation_focus->get_focus_transformation(), interpolation_value);
	} else {
		if (!interpolation_switch) {
			auto t = current_focus;
			focus(interpolation_focus);
			interpolation_focus = t;
			interpolation_switch = true;
		}
		focus_offset = (interpolation_focus->position - current_focus->position) * (1 - interpolation_value) * (1 - interpolation_value) * 2;
		focus_transform = interpolate_matrix(
			interpolation_focus->get_focus_transformation(), 
			current_focus->get_focus_transformation(), interpolation_value);
	}
	recalculate_orientation();
	//camera->distance = 1 * sqrt(1 - interpolation_value * interpolation_value);
}

void CameraContainer::focus(Agent* o) {
	if (current_focus != nullptr) {
		current_focus->is_camera_focus = false;
	}
	o->is_camera_focus = true;
	current_focus = o;
	focus_transform = o->get_focus_transformation();
	recalculate_orientation();
}

void CameraContainer::interpolate_focus(Agent* o) {
	if (is_interpolating) {
		return;
	}
	interpolation_focus = o;
	interpolation_value = 0;
	is_interpolating = true;
	interpolation_switch = false;
}

void CameraContainer::defocus() {
	if (current_focus != nullptr) {
		current_focus->is_camera_focus = false;
	}
	current_focus = nullptr;
}

LongVector CameraContainer::get_focal_point() {
	if (current_focus == nullptr) {
		return focus_offset;
	}
	//print_vector(current_focus->position + focus_offset);
	return current_focus->position + focus_offset;
}
