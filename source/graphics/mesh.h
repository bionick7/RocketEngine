#pragma once
#include "agent.h"
#include "data_input/mesh_resource.h"

class Mesh : public Shape {
public:
	Mesh();
	Mesh(MeshResource*);
	~Mesh();

	void draw(const Camera*, glm::mat4) override;
	signed char draw_order() override;
	
	MeshResource* source;

	void load();
	void unload();

private:
	bool initialized = false;
	uint16_t segment_count;
};