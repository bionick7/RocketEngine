#pragma once
#include "agent.h"
#include "shape.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "asset_manager.h"
#include "settings.h"

class CameraContainer : Agent
{
public:
	CameraContainer(GLFWwindow*, double*);
	~CameraContainer();

	Camera small_scale;
	Camera large_scale;

	void logic_step(double) override;

	void update_rotation(double, GLFWwindow*);
	void interpolation_step(double);

	void interpolate_focus(Agent*);
	void focus(Agent*);
	void defocus();

	void recalculate_orientation();

	Agent* current_focus = nullptr;
	glm::mat4 focus_transform;
	LongVector focus_offset = LongVector(0.0, 0.0, 0.0);

	LongVector get_focal_point();

private:
	double mouse_speed = 0.5;
	double initial_fov = pi_quart;
	double max_vertical_angle = pi_halfs - .01;
	double min_vertical_angle = -pi_halfs + .01;
	double scroll_sensitivity = 0.087;

	double prev_mouse_x = 0, prev_mouse_y = 0;

	double horizontal_angle, vertical_angle;
	bool is_rotating = true;

	Agent* interpolation_focus = nullptr;
	double interpolation_value;
	bool is_interpolating, interpolation_switch;

	GLFWwindow* window_ptx;
	double* scroll_ptx;
};
