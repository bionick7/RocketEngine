#ifndef POLYGONE_H
#define POLYGONE_H

#include "shape.h"

typedef std::vector<glm::vec3> point_array_t;

class Polygone : Shape
{
public:
	Polygone();
	~Polygone();

	void generate_point_array(point_array_t value);
	void set_point_array(point_array_t value);

	void draw(Camera* camera) override;

	unsigned vertex_buffer_size;
	std::vector<GLuint> vertexbuffers;

private:
};

#endif // ! POLYNOME_H