#pragma once
#include "settings.h"
#include "debug.h"

class Camera {
public:
	Camera();
	Camera(int, int);

	Screenpos screensize;

	double fov;
	double distance;

	glm::vec3 focus_point, direction, up;
	glm::mat4 vp_matrix;
	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	float aspect_ratio;

	void recalculate_vp();

	double near_clip = .1;
	double far_clip = 1000;

	bool is_active = true;
	int32_t render_layers = 1;
};