#pragma once
#include "graphic_manager.h"

struct Line3D {
	glm::vec3 start, end;
};

typedef std::vector<Line3D> linearray_t;

class LineSet: public Shape
{
public:
	LineSet();
	~LineSet();

	void update(linearray_t value);

	void draw(const Camera*, glm::mat4) override;

	uint16_t line_count;
	std::vector<GLuint> vertexbuffers;

private:
	GLuint array_id;
	GLuint active_ID;
};
