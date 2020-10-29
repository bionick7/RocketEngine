#pragma once
#include "shape.h"

typedef std::vector<glm::vec3> point_array_t;

enum LineType {
	CONTINUOUS,
	CLOSED,
	DASHED,
};

class Polygone : public Shape
{
public:
	Polygone(LineType);
	~Polygone();

	void set_point_array(point_array_t value);

	void draw(const Camera*, glm::mat4) override;

	uint16_t point_count;

	LineType line_type;
};