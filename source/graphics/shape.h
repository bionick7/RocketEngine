#pragma once
#include "Camera.h"
#include "asset_manager.h"

#define DEBUG

class Agent;

class Shape
{
public:
	Shape();
	~Shape();

	/// <summary> ID of the shader, the shape uses </summary>
	GLuint shader;
	/// <summary> ID of the vertex buffer </summary>
	GLuint vertex_buffer;

	glm::mat4 shape_transform = glm::mat4(1);

	/// <summary> method needed to draw anything on screen </summary>
	void virtual draw(const Camera*, glm::mat4);
	signed char virtual draw_order();

	Agent* parent = nullptr;
	bool visible = true;
	bool independent = false;

	int32_t render_layers = 1;

protected:
	GLuint vp_matrix_ID;
	GLuint transform_ID;
	GLuint color_ID;
};

glm::mat4 get_shape_transform(Shape*);

class GraphicsServer {
public:
	GraphicsServer();

	std::vector<Camera*> all_cameras = std::vector<Camera*>();
	std::vector<Shape*>* shape_vector_array;
	void draw();
};

extern GraphicsServer* graphics_server;
