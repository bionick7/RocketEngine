#include "Camera.h"

Camera::Camera() {
	screensize = Screenpos(0, 0);
	aspect_ratio = 1.0;
	
	focus_point = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	distance = 1;
}

Camera::Camera(int width, int height) {
	screensize = Screenpos(width, height);
	aspect_ratio = width / (float)height;
	
	focus_point = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	distance = 1;
}

void Camera::recalculate_vp() {
	glm::vec3 d_pos = direction;
	d_pos *= distance;
	projection_matrix = glm::perspective(float(fov), aspect_ratio, float(near_clip), float(far_clip));
	view_matrix = glm::lookAt(focus_point - d_pos, focus_point, up);
	vp_matrix = projection_matrix * view_matrix;
}