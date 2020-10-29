#pragma once
#include "shape.h"

class Circle :
	public Shape
{
public:
	Circle(double);
	~Circle();

	float radius;

	void draw(const Camera*, glm::mat4 base) override;
	signed char draw_order() override;

	glm::vec3 world_position;

private:
	GLuint viewmatrix_ID;
	GLuint projectionmatrix_ID;
	GLuint backgroundcolor_ID;
	GLuint edge_ID;
	GLuint thickness_ID;
	GLuint screenwith_ID;
};

