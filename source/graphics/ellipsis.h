#pragma once
#include "graphic_manager.h"

class Ellipsis : public Shape
{
public:
	Ellipsis();
	~Ellipsis();

	void draw(const Camera*, glm::mat4) override;

	float a;
	float b;
	float ell_anomaly;
	glm::mat4 orbit_transform;

	void fill_vertex_buffer(int, float);

private:
	GLuint active_ID;
	GLuint ell_anomaly_ID;
	GLuint ell_a_ID;
	GLuint ell_b_ID;
	GLuint camerabounding_ID;

	int segemnt_count = 0;
};