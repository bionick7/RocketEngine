#pragma once
#include "resource.h"
#include "data_input/reader.h"

struct Triangle { uint16_t a, b, c; };
struct Segment { uint16_t a, b; };

class MeshResource: public io::Resource
{
friend class Mesh;
public:
	MeshResource();
	MeshResource(io::DataStructure*);

	bool deserialize(std::istream*, int) override;
	bool serialize(std::ostream*, int*) override;
	void kill() override;
	io::ResourceType get_type() override;

	uint16_t const get_vertex_count();
	uint16_t const get_triangle_count();
	uint16_t const get_segment_count();
	void generate_vertecies(uint16_t);
	void generate_segments(uint16_t);
	void generate_triangles(uint16_t);

	glm::vec3 get_vertex(uint16_t);
	Triangle get_triangle(uint16_t);
	Segment get_segment(uint16_t);
	void set_vertex(uint16_t, glm::vec3);
	void set_triangle(uint16_t, uint16_t, uint16_t, uint16_t);
	void set_segment(uint16_t, uint16_t, uint16_t);

private:
	glm::vec3* vertecies = nullptr;
	uint16_t* indecies = nullptr;
	uint16_t vertex_count = 0;
	uint16_t index_count = 0;

	double extra_size;
};
