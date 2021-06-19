#pragma once
#include "Camera.h"
#include "asset_manager.h"
#include "manager.h"

#define GRAPHIC_MANAGER_DEBUG

#define MAX_SHAPE_LAYERS 256

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
};

glm::mat4 get_shape_transform(Shape*);

struct GraphicsManagerDataLookup {
	GLuint frame_buffer;
	GLuint quad_vertex_buffer;
	GLuint post_process_shader;
	GLuint render_texture;
	//GLuint depth_texture;
	GLuint shader_texture;
	GLuint shader_time;
	GLuint shader_background;
	GLuint shader_foreground;
	GLuint depthrenderbuffer;
};

class GraphicsManager :
	Manager{
public:
	GraphicsManager();
	~GraphicsManager();

	std::vector<Camera*> all_cameras = std::vector<Camera*>();
	std::vector<Shape*> shape_vector_array[MAX_SHAPE_LAYERS];
	bool setup();
	void step(double) override;
	void post_processing();

	//void on_window_resized();

	bool post_processing_enabled = false;

	void interprete_signal(RadioSignal) override;

private:
	GraphicsManagerDataLookup gl_ids;
	
	void draw_geometry(Camera*);
	void render_loop(Camera*);
};

extern GraphicsManager* graphics_manager;
