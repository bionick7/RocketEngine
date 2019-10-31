#ifndef CAMERA_H
#define CAMERA_H

#include "../core/asset_manager.h"
#include "../core/settings.h"

class Camera {
public:
	Camera(GLFWwindow*, double*);
	~Camera();

	Screenpos screensize;

	glm::mat4 vp_matrix;

	float get_aspect_ratio();
	double fov;
	double distance;

	void update(double);
	void focus(glm::vec3);

	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

private:
	glm::vec3 focus_point, direction, up;

	double horizontal_angle, vertical_angle;
	bool is_rotating = true;

	GLFWwindow* window_ptx;
	double* scroll_ptx;

	void update_rotation(double d_t, GLFWwindow* window_ptx);
	void recalculate_vp();

	double mouse_speed = 0.5;
	double initial_fov = pi_quart;
	double max_vertical_angle = pi_halfs - .01;
	double min_vertical_angle = -pi_halfs + .01;
	double scroll_sensitivity = 0.087;
	
	double near_clip = .1;
	double far_clip = 1000;
};

#endif // !CAMERA_H