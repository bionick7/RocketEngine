#include "Camera.h"

Camera::Camera() {
	screensize = Screenpos(settings->width, settings->height);
	aspect_ratio = settings->width / (float)settings->height;
	
	focus_point = LongVector(0.0, 0.0, 0.0);  // Should always be 0; everything else moves
	direction = LongVector(0.0, 0.0, -1.0);
	up = LongVector(0.0, 1.0, 0.0);
	distance = 1;
	world_position = focus_point - direction * distance;
}

void Camera::on_window_resized() {
	screensize = Screenpos(settings->width, settings->height);
	aspect_ratio = settings->width / (float)settings->height;

	recalculate_vp();
}

void Camera::recalculate_vp() {
	world_position = focus_point - direction * distance;
	projection_matrix = glm::perspective(float(fov), aspect_ratio, float(near_clip), float(far_clip));
	view_matrix = glm::lookAt(world_position.to_float_vec(), focus_point.to_float_vec(), up.to_float_vec());
	vp_matrix = projection_matrix * view_matrix;
}