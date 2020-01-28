#include "ui_element.h"

GLPos pixel2glpos(Screenpos pixel) {
	return GLPos(
		(float) pixel.x / settings->width * 2.0f - 1.0f,
		(float) pixel.y / settings->height * 2.0f - 1.0f
	);
}

Screenpos glpos2pixel(GLPos pos) {
	return Screenpos(
		(int) ((pos.x + 1.0f) * 0.5f * settings->width),
		(int) ((pos.y + 1.0f) * 0.5f * settings->height)
	);
}

UIElement::UIElement() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;

	glGenBuffers(1, vertexbuffers);
	glGenBuffers(1, &background_quad);
	glGenBuffers(1, &VertexBufferID);
	glGenBuffers(1, &UVBufferID_MatrixID);

	SamplerUniformID = glGetUniformLocation(assets->text_shader, "TextureSampler");
	ScreenSizeID = glGetUniformLocation(assets->text_shader, "ScreenSizeHlf");
	ColorID = glGetUniformLocation(assets->text_shader, "MainColor");

	draw_border = true;
}

UIElement::~UIElement() {
	glDeleteBuffers(vertex_buffer_size, vertexbuffers);
	glDeleteBuffers(1, &background_quad);
	glDeleteBuffers(1, &VertexBufferID);
	glDeleteBuffers(1, &UVBufferID_MatrixID);
}

void UIElement::set_transform(int _x, int _y, int _width, int _height, bool wrap_around) {
	width = _width;
	height = _height;
	if (_x >= 0 || !wrap_around) x = _x;
	else x = settings->width  - width + _x;
	if (_y >= 0 || !wrap_around) y = _y;
	else y = settings->height - height + _y;
	recalculate();
}

void UIElement::update(double, GLFWwindow*, Camera*) {
	if (recalc_flag) {
		recalculate();
		recalc_flag = false;
	}

	// Draw
	glm::mat4 transform_matrix = glm::mat4(1.0f);

	glUseProgram(assets->get_shader("Simple"));
	glUniformMatrix4fv(UVBufferID_MatrixID, 1, GL_FALSE, &transform_matrix[0][0]);
	// clear area
	glUniform3f(ColorID, settings->background.r, settings->background.g, settings->background.b);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, background_quad);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);

	// draw lines
	glUniform3f(ColorID, settings->draw.r, settings->draw.g, settings->draw.b);

	glEnableVertexAttribArray(0);
	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
}


std::vector<float> UIElement::get_lines() {
	std::vector<float> res = std::vector<float>();
	if (has_outline) {
		res.push_back(0);
		res.push_back(-3);
		res.push_back(width);
		res.push_back(-3);
	}
	for (int i = 0; i < 16; i++) {
		if (frame[i] == 1) res.push_back(width);
		else if (frame[i] == 2) res.push_back(height);
		else res.push_back(0);
	}
	return res;
}

void UIElement::setup_lines(std::vector<float> lines) {
	glDeleteBuffers(vertex_buffer_size, vertexbuffers);

	vertex_buffer_size = lines.size() / 4;

	GLuint shader = assets->get_shader("Simple");
	UVBufferID_MatrixID = glGetUniformLocation(shader, "VP");
	ColorID = glGetUniformLocation(shader, "Color");

	// Recalculate coordinates
	GLPos start_pos = pixel2glpos(Screenpos(x, y));
	float fl_width = 1.0f / settings->width * 2.0f;
	float fl_height = 1.0f / settings->height * 2.0f;

	vertexbuffers = new GLuint[vertex_buffer_size];

	glGenBuffers(vertex_buffer_size, vertexbuffers);

	GLuint* vertex_arrays = new GLuint[vertex_buffer_size];
	glGenVertexArrays(vertex_buffer_size, vertex_arrays);

	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		glBindVertexArray(*(vertex_arrays + i));
		GLfloat vertex_buffer_data[6];

		*(vertex_buffer_data + 0) = start_pos.x + lines[4 * i] * fl_width;
		*(vertex_buffer_data + 1) = start_pos.y + lines[4 * i + 1] * fl_height;
		*(vertex_buffer_data + 2) = 0;
		*(vertex_buffer_data + 3) = start_pos.x + lines[4 * i + 2] * fl_width;
		*(vertex_buffer_data + 4) = start_pos.y + lines[4 * i + 3] * fl_height;
		*(vertex_buffer_data + 5) = 0;

		glBindBuffer(GL_ARRAY_BUFFER, *(vertexbuffers + i));
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STREAM_DRAW);
	}

	// bg_quads
	GLPos
		start = pixel2glpos(Screenpos(x - 2, y - 2)),
		end = pixel2glpos(Screenpos(x + width + 2, y + height + 2));

	GLfloat g_vertex_buffer_data[] = {
			start.x, start.y, 0.0f,
			end.x,   start.y, 0.0f,
			start.x, end.y,   0.0f,
			start.x, end.y,   0.0f,
			end.x,   start.y, 0.0f,
			end.x,   end.y,   0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, background_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}


void UIElement::request_recalculate() {
	recalc_flag = true;
}

bool UIElement::contains_pixel(Screenpos coords) {
	return x < coords.x && coords.x < x + width && y < coords.y && coords.y < y + height;
}

void UIElement::recalculate() {
	setup_lines(get_lines());
}

void UIElement::set_outline(bool value) {
	has_outline = value;
	recalculate();
}