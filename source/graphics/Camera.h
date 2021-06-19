#pragma once
#include "settings.h"
#include "debug.h"

class Camera {
public:
	Camera();

	Screenpos screensize;

	double fov;
	double distance;

	LongVector focus_point, direction, up, world_position;
	glm::mat4 vp_matrix;
	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	float aspect_ratio;

	void on_window_resized();
	void recalculate_vp();

	double near_clip = .1;
	double far_clip = 1000;

	bool is_active = true;
	int32_t render_layers = 1;
};