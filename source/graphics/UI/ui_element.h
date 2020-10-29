#pragma once
#include "asset_manager.h"
#include "Camera.h"

GLPos pixel2glpos(Screenpos);
Screenpos glpos2pixel(GLPos);

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
	void virtual update(double, GLFWwindow*);
	void virtual request_recalculate();
	bool contains_pixel(Screenpos);

	bool recalc_flag = false;

	bool draw_border = false, window = false;
	pixel x, y, width, height;

	void set_outline(bool);

protected:
	void virtual recalculate();
	std::vector<float> virtual get_lines();
	void virtual setup_lines(std::vector<float>);

	GLuint background_quad;

private:
	bool has_outline = false;

	size_t vertex_buffer_size;
	GLuint* vertexbuffers;

	GLuint VertexBufferID;
	GLuint UVBufferID_MatrixID;
	GLuint ColorID;
};
