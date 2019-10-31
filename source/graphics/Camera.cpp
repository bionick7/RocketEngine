#include "Camera.h"

Camera::Camera(GLFWwindow* _window_ptx, double* _scroll_ptx) {
	window_ptx = _window_ptx;
	scroll_ptx = _scroll_ptx;

	int width, height;
	glfwGetWindowSize(window_ptx, &width, &height);
	screensize = Screenpos(width, height);

	fov = initial_fov;
	
	horizontal_angle = 0;
	vertical_angle = 0;
	focus_point = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	distance = 1;

	recalculate_vp();

	mouse_speed = settings->mouse_sensitivity;
	initial_fov = settings->initial_fov;
	min_vertical_angle = settings->min_vertical_angle;
	max_vertical_angle = settings->max_vertical_angle;
	scroll_sensitivity = settings->scroll_sensitivity;
}

Camera::~Camera() {

}

void Camera::update(double d_t) {
	int width, height;
	glfwGetWindowSize(window_ptx, &width, &height);
	screensize = Screenpos(width, height);

	if (glfwGetMouseButton(window_ptx, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (!is_rotating) {
			glfwSetCursorPos(window_ptx, screensize.x / 2, screensize.y / 2);
		}
		is_rotating = true;
	} else if (glfwGetMouseButton(window_ptx, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		is_rotating = false;
	}

	if (is_rotating) {
		update_rotation(d_t, window_ptx);
	}

	distance = exp(scroll_sensitivity * -*scroll_ptx);
	near_clip = distance / 100;
	far_clip = distance * 100;

	glm::vec3 move_dir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
	glm::vec3 move_right = glm::normalize(glm::cross(move_dir, up));

	if (glfwGetKey(window_ptx, GLFW_KEY_UP) == GLFW_PRESS) {
		focus_point += move_dir * float(d_t * distance);
	}
	if (glfwGetKey(window_ptx, GLFW_KEY_DOWN) == GLFW_PRESS) {
		focus_point -= move_dir * float(d_t * distance);
	}
	if (glfwGetKey(window_ptx, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		focus_point += move_right * float(d_t * distance);
	}
	if (glfwGetKey(window_ptx, GLFW_KEY_LEFT) == GLFW_PRESS) {
		focus_point -= move_right * float(d_t * distance);
	}

	recalculate_vp();
}

void Camera::update_rotation(double d_t, GLFWwindow* window_ptx) {
	double mouse_x, mouse_y;
	glfwGetCursorPos(window_ptx, &mouse_x, &mouse_y);
	glfwSetCursorPos(window_ptx, screensize.x / 2, screensize.y / 2);

	horizontal_angle += mouse_speed * d_t * (screensize.x / 2 - mouse_x);
	vertical_angle += mouse_speed * d_t * (screensize.y / 2 - mouse_y);

	if (vertical_angle >= max_vertical_angle) {
		vertical_angle = max_vertical_angle;
	}
	else if (vertical_angle <= min_vertical_angle) {
		vertical_angle = min_vertical_angle;
	}

	direction = glm::vec3(
		cos(vertical_angle) * sin(horizontal_angle),
		sin(vertical_angle),
		cos(vertical_angle) * cos(horizontal_angle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontal_angle - pi_halfs),
		0,
		cos(horizontal_angle - pi_halfs)
	);

	up = glm::cross(right, direction);
}

void Camera::recalculate_vp() {
	glm::vec3 d_pos = direction;
	d_pos *= distance;
	projection_matrix = glm::perspective(float(fov), get_aspect_ratio(), float(near_clip), float(far_clip));
	view_matrix = glm::lookAt(focus_point - d_pos, focus_point, up);
	vp_matrix = projection_matrix * view_matrix;
	//print_matrix(projection_matrix);
	//print_matrix(view_matrix);
	//std::cout << std::endl;
}

float Camera::get_aspect_ratio() {
	return (float) screensize.x / (float) screensize.y;
}

void Camera::focus(glm::vec3 f_point) {
	focus_point = f_point;
	recalculate_vp();
}