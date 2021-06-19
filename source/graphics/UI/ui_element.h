#pragma once
#include "asset_manager.h"
#include "Camera.h"
#include "radio.h"

GLPos pixel2glpos(Screenpos);
Screenpos glpos2pixel(GLPos);

#define UI_UPDATE_FLAGS_GEOMETRY	0x001
#define UI_UPDATE_FLAGS_TRANSFORM	0x002

const char frame[] = {
	0, 0, 1, 0,
	0, 0, 0, 2,
	1, 0, 1, 2,
	0, 2, 1, 2,
};

class UIElement abstract
{
public:
	UIElement();
	~UIElement();
	void virtual set_transform(int, int, int, int, bool=false);
	void virtual set_transform_anchor_margin(int, int, int, int, float, float, float, float);
	void virtual step();
	void virtual request_update(uint16_t);
	bool contains_pixel(Screenpos);

	uint16_t update_flags = 0;

	bool draw_border = false, movable = true;
	
	float anchor_left, anchor_right, anchor_up, anchor_down;
	pixel margin_left, margin_right, margin_up, margin_down;

	pixel x, y, width, height;

	void set_outline(bool);
	//void recieve_signal(int);

	void virtual character_callback(GLFWwindow*, unsigned int);
	void virtual key_callback(GLFWwindow*, int, int, int, int);

	glm::mat4 share_object2clip_transform();

	UIElement* parent;

	std::string _debug_label = "UNDEFINED";

	DecoratedRadio* radio;

protected:
	void virtual update_lines();
	void virtual update_transform();
	std::vector<float> virtual get_lines();
	void virtual setup_lines(std::vector<float>);

	void virtual interprete_signal(RadioSignal);
	static void interprete_signal_callback(void*, RadioSignal);

	GLuint background_quad;

	UIElement** dependencies;
	int dependency_count;

private:
	bool has_outline = false;

	size_t vertexbuffer_size = 0;
	GLuint vertexbuffer;
	GLuint vertex_array;

	bool vb_is_initialized = false;

	glm::mat4 object2clip;
	glm::mat4 screen2clip;
	glm::mat4 root_transform;

	glm::mat4 get_base_transform();

	GLuint VertexBufferID;
	GLuint TransformMatrixID;
	GLuint ActiveID;
	GLuint ShaderID;
};
