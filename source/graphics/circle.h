#pragma once
#include "graphic_manager.h"

class Circle :
	public Shape
{
public:
	Circle(double);
	~Circle();

	float radius;

	void draw(const Camera* camera, glm::mat4 transform) override;
	signed char draw_order() override;

private:
	GLuint vp_ID;
	GLuint edge_ID;
	GLuint thickness_ID;
	GLuint screenwith_ID;
	GLuint radius_ID;
	GLuint camera_pos_ID;
	GLuint camera_dir_ID;

	glm::mat4 get_adjusted_transform(LongVector camera_position, glm::mat4 transform);
};

