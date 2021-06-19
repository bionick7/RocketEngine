#include "ui_element.h"
#include "ui_canvas.h"
#include "ui_manager.h"

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

GLuint get_quad_buffer() {
	static const GLfloat g_vertex_buffer_data[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexbuffer;
}

UIElement::UIElement() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;

	anchor_left = 0.0f;
	anchor_right = 0.0f;
	anchor_up = 0.0f;
	anchor_down = 0.0f;

	margin_up = 0;
	margin_down = 0;
	margin_left = 0;
	margin_right = 0;

	ShaderID = (assets->get<Shader>(io::ResourceType::SHADER, "UI"))->ID;

	TransformMatrixID = glGetUniformLocation(ShaderID, "_ScreenTransform");
	ActiveID = glGetUniformLocation(ShaderID, "_Active");

	draw_border = true;
	object2clip = glm::mat4(1);
	screen2clip = glm::mat4(1);

	background_quad = get_quad_buffer();
	glGenBuffers(1, &vertexbuffer);

	// radio initialisation
	radio = new DecoratedRadio(this, &UIElement::interprete_signal_callback);
	radio->add_signal("transform_changed");
	radio->add_signal("geometry_changed");
	ui_manager->radio->connect("window_resized", radio, RadioSignal("window_resized"));

	update_transform();
}

UIElement::~UIElement() {
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &background_quad);
}

void UIElement::set_transform(int _x, int _y, int _width, int _height, bool wrap_around) {
	// assuming the anchor is unmodified

	width = _width;
	height = _height;
	if (_x >= 0 || !wrap_around) x = _x;
	else x = settings->width  - width + _x;
	if (_y >= 0 || !wrap_around) y = _y;
	else y = settings->height - height + _y;

	margin_left = x - settings->width * anchor_left;
	margin_right = x + width - settings->width * anchor_right;
	margin_down = y - settings->height * anchor_down;
	margin_up = y + height - settings->height * anchor_up;

	request_update(UI_UPDATE_FLAGS_TRANSFORM);
}

void UIElement::set_transform_anchor_margin(int margin_left_, int margin_right_, int margin_up_, int margin_down_, 
	float anchor_left_, float anchor_right_, float anchor_up_, float anchor_down_) {

	width = margin_right_ + anchor_right_ * settings->width - margin_left_ - anchor_left_ * settings->width;
	height = margin_up_ + anchor_up_ * settings->height - margin_down_ - anchor_down_ * settings->height;
	x = margin_left_ + anchor_left_ * settings->width;
	y = margin_down_ + anchor_down_ * settings->height;
	
	margin_left = margin_left_;
	margin_right = margin_right_;
	margin_up = margin_up_;
	margin_down = margin_down_;

	anchor_left = anchor_left_;
	anchor_right = anchor_right_;
	anchor_up = anchor_up_;
	anchor_down = anchor_down_;

	request_update(UI_UPDATE_FLAGS_TRANSFORM);
}

void UIElement::step() {
	if (update_flags & UI_UPDATE_FLAGS_GEOMETRY) {
		radio->emit_signal("geometry_changed");
		update_lines();
	}
	if (update_flags & UI_UPDATE_FLAGS_TRANSFORM) {
		radio->emit_signal("transform_changed");
		update_transform();
	}
	update_flags = 0;

	// draw
	glUseProgram(ShaderID);
	//print_matrix(object2clip);

	// clear area
	glUniform1f(ActiveID, 0.0);
	glUniformMatrix4fv(TransformMatrixID, 1, GL_FALSE, &object2clip[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, background_quad);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);

	// draw lines
	glUniform1f(ActiveID, 1.0);
	glUniformMatrix4fv(TransformMatrixID, 1, GL_FALSE, &screen2clip[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 2 * vertexbuffer_size);
	glDisableVertexAttribArray(0);

	for (int i = 0; i < dependency_count; i++) {
		dependencies[i]->step();
	}
}

// In "PixelSpace"
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
	vertexbuffer_size = lines.size();
	GLfloat* vertex_buffer_data = new GLfloat[vertexbuffer_size];
	for (unsigned i = 0; i < vertexbuffer_size; i++) {
		vertex_buffer_data[i] = lines[i];
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data[0]) * vertexbuffer_size, vertex_buffer_data, GL_STREAM_DRAW);
	delete[] vertex_buffer_data;
}

void UIElement::interprete_signal(RadioSignal signal) {
	// std::cout << "==>" << signal.identifier << std::endl;
	if (signal.identifier == "window_resized") {
		update_transform();
		for (int i = 0; i < dependency_count; i++) {
			dependencies[i]->update_transform();
		}
	}
}

void UIElement::interprete_signal_callback(void* obj, RadioSignal signal) {
	((UIElement*)obj)->interprete_signal(signal);
}

void UIElement::request_update(uint16_t flags) {
	update_flags |= flags;
	
	for (int i = 0; i < dependency_count; i++) {
		dependencies[i]->request_update(flags);
	}
}

bool UIElement::contains_pixel(Screenpos coords) {
	return x < coords.x && coords.x < x + width && y < coords.y && coords.y < y + height;
}

void UIElement::update_lines() {
	setup_lines(get_lines());
}

void UIElement::update_transform() {
	// Recalculate coordinates
	
	// recalculate anchor
	x = anchor_left * settings->width + margin_left;
	width = anchor_right * settings->width + margin_right - x;
	y = anchor_down * settings->height + margin_down;
	height = anchor_up * settings->height + margin_up - y;

	GLPos start_pos = pixel2glpos(Screenpos(x, y));

	glm::mat4 screen_scale = glm::scale(glm::mat4(1), glm::vec3(2.0 / settings->width, 2.0 / settings->height, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3((float)width / settings->width * 2.0, (float)height / settings->height * 2.0, 1));
	glm::mat4 translate = glm::translate(glm::mat4(1), glm::vec3(start_pos.x, start_pos.y, 0));
	//glm::mat4 base = get_base_transform();

	object2clip = translate * scale;
	screen2clip = translate * screen_scale;
	//root_transform = glm::translate(glm::mat4(1), glm::vec3(start_pos.x + 1 , start_pos.y + 1, 0));
}

void UIElement::set_outline(bool value) {
	has_outline = value;
	request_update(UI_UPDATE_FLAGS_GEOMETRY);
}
/*
void UIElement::recieve_signal(int signal_id) {
	// TODO: Integrate into Radio
	switch (signal_id)
	{
	case WINDOW_RESIZED:
		update_transform();
		break;
	default:
		break;
	}
	for (int i = 0; i < dependency_count; i++) {
		dependencies[i]->recieve_signal(signal_id);
	}
}*/

void UIElement::character_callback(GLFWwindow*, unsigned int) {
	// Do nothing
}

void UIElement::key_callback(GLFWwindow*, int, int, int, int) {
	// Do nothing
}

glm::mat4 UIElement::get_base_transform()
{
	if (parent == nullptr) {
		return glm::mat4(1);
	}
	else {
		return parent->share_object2clip_transform();
	}
}

glm::mat4 UIElement::share_object2clip_transform()
{
	return root_transform;
}
