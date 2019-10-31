#pragma once
#include "shape.h"

class Circle :
	public Shape
{
public:
	Circle(double);
	~Circle();

	float radius;

	void draw_c(Camera*);// override;

	glm::vec3 world_position;

private:
	GLuint screensize_ID;
	GLuint radii_ID;
	GLuint midpos_ID;
	GLuint camera_right_ID;
	GLuint camera_up_ID;
	GLuint billboard_pos_ID;
	GLuint billboard_size_ID;
};

