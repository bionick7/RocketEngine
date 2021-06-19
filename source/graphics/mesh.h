#pragma once
#include "agent.h"
#include "data_input/mesh_resource.h"
#include "asset_manager.h"

class Mesh : public Shape {
public:
	Mesh();
	Mesh(MeshResourcePtr);
	~Mesh();

	void draw(const Camera*, glm::mat4) override;
	signed char draw_order() override;
	
	MeshResourcePtr source;

	void load();
	void unload();

	bool is_loaded = false;
	uint16_t segment_count;

private:
	GLuint active_ID;
};
