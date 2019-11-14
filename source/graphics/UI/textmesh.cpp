#include "textmesh.h"

bool is_initialized = false;

Textmesh::Textmesh(std::string _content, Font _font) {
	if (!is_initialized) {
		is_initialized = true;
	}

	content = _content;
	font = _font;

	glGenBuffers(1, &VertexBufferID);
	glGenBuffers(1, &UVBufferID_MatrixID);

	SamplerUniformID = glGetUniformLocation(assets->text_shader, "TextureSampler");
	ScreenSizeID = glGetUniformLocation(assets->text_shader, "ScreenSizeHlf");
	ColorID = glGetUniformLocation(assets->text_shader, "MainColor");
}

Textmesh::~Textmesh() {
	glDeleteBuffers(1, &VertexBufferID);
	glDeleteBuffers(1, &UVBufferID_MatrixID);
}

std::string Textmesh::get_content() {
	return content;
}

void Textmesh::set_content(std::string _content) {
	content = _content;
	recalculate();
}

void Textmesh::set_transform(int _x, int _y, int _sizex, int _sizey) {
	if (_sizex < 0) {
		_sizex = INFINITY;
	}
	UIElement::set_transform(_x, _y, _sizex, _sizey);
	recalculate();
}


std::vector<float> Textmesh::get_lines() {
	unsigned int length = content.length();
	if (length == 0)
		return std::vector<float>();

	float size = std::min(width / length, height);

	GLuint shader = assets->get_shader("Simple");
	UVBufferID_MatrixID = glGetUniformLocation(shader, "VP");
	ColorID = glGetUniformLocation(shader, "Color");

	std::vector<float> lines = std::vector<float>();

	Textpos tp = { 0, 0 };
	// Fill buffers
	for (unsigned int i = 0; i < length; i++) {
		if (content[i] >= 32) {
			unsigned char* data = assets->default_vector_font.data[content[i]];

			for (int j = 0; *(data + j) != 0; j += 4) {
				lines.push_back((*(data + j + 0) - 1) / 254.0f + tp.x * (1 + spacing_x));
				lines.push_back((*(data + j + 1) - 1) / 254.0f - tp.y * (1 + spacing_y));
				lines.push_back((*(data + j + 2) - 1) / 254.0f + tp.x * (1 + spacing_x));
				lines.push_back((*(data + j + 3) - 1) / 254.0f - tp.y * (1 + spacing_y));
			}
		}
		else {
			// Control characters
			switch (content[i])
			{
			case '\a':
				// Ding!
				break;
			case '\f':
				// cls
				break;
			default:
				break;
			}
		}
		increment_tp(&tp, content[i]);
	}

	return lines;
}

void Textmesh::setup_lines(std::vector<float> lines) {
	vertex_buffer_size = lines.size() / 4;
	vertexbuffers = std::vector<GLuint>();

	// Recalculate coordinates
	GLPos start_pos = pixel2glpos(Screenpos(x, y));
	float fl_width = (float)height / (settings->width * ratio) * 2.0f;
	float fl_height = (float)height / settings->height * 2.0f;

	GLuint* vertexbuffers_ptxs = new GLuint[vertex_buffer_size];

	glGenBuffers(vertex_buffer_size, vertexbuffers_ptxs);

	GLuint* vertex_arrays = new GLuint[vertex_buffer_size];
	glGenVertexArrays(vertex_buffer_size, vertex_arrays);

	for (unsigned i = 0; i < vertex_buffer_size; i ++) {
		glBindVertexArray(*(vertex_arrays + i));
		GLfloat vertex_buffer_data[6];

		*(vertex_buffer_data + 0) = start_pos.x + lines[4*i] * fl_width;
		*(vertex_buffer_data + 1) = start_pos.y + lines[4*i + 1] * fl_height;
		*(vertex_buffer_data + 2) = 0;
		*(vertex_buffer_data + 3) = start_pos.x + lines[4 * i + 2] * fl_width;
		*(vertex_buffer_data + 4) = start_pos.y + lines[4 * i + 3] * fl_height;
		*(vertex_buffer_data + 5) = 0;

		glBindBuffer(GL_ARRAY_BUFFER, *(vertexbuffers_ptxs + i));
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STREAM_DRAW);

		vertexbuffers.push_back(*(vertexbuffers_ptxs + i));
	}

	delete[] vertexbuffers_ptxs;
}

void Textmesh::recalculate() {
	UIElement::recalculate();

	unsigned int length = content.length();
	if (length == 0)
		return;

	float size = std::min(width / length, height);

	if (vector){
		setup_lines(get_lines());
	}
	else {

		// Fill buffers
		std::vector<glm::vec2> vertices;
		std::vector<glm::vec2> UVs;
		for (unsigned int i = 0; i < length; i++) {

			glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
			glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
			glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
			glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

			vertices.push_back(vertex_up_left);
			vertices.push_back(vertex_down_left);
			vertices.push_back(vertex_up_right);

			vertices.push_back(vertex_down_right);
			vertices.push_back(vertex_up_right);
			vertices.push_back(vertex_down_left);

			char character = content[i];
			float uv_x = (character % 16) / 16.0f;
			float uv_y = (character / 16) / 16.0f;

			glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
			glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
			glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
			glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
			UVs.push_back(uv_up_left);
			UVs.push_back(uv_down_left);
			UVs.push_back(uv_up_right);

			UVs.push_back(uv_down_right);
			UVs.push_back(uv_up_right);
			UVs.push_back(uv_down_left);
		}
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, UVBufferID_MatrixID);
		glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
	}
}

void Textmesh::draw(Camera* cam) {
	if (vector) {
		glm::mat4 transform_matrix = glm::mat4(1.0f);

		glUseProgram(assets->get_shader("Simple"));
		glUniformMatrix4fv(UVBufferID_MatrixID, 1, GL_FALSE, &transform_matrix[0][0]);
		glUniform3f(ColorID, settings->draw.r, settings->draw.g, settings->draw.b);

		glEnableVertexAttribArray(0);
		for (unsigned i = 0; i < vertex_buffer_size; i++) {
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glDisableVertexAttribArray(0);
	}
	else {

		// Bind shader
		glUseProgram(assets->text_shader);

		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, font.Texture_ID);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(SamplerUniformID, 0);

		glUniform2ui(ScreenSizeID, cam->screensize.x / 2, cam->screensize.y / 2);
		glUniform4f(ColorID, settings->draw.r, settings->draw.g, settings->draw.b, 1);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, UVBufferID_MatrixID);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw call
		glDrawArrays(GL_TRIANGLES, 0, 6 * content.length());

		glDisable(GL_BLEND);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

Textmesh::Textpos Textmesh::get_text_pos(int index) {
	Textpos res = { 0, 0 };
	
	int len = index < content.length() ? index : content.length();
	for (int i = 0; i < len; i++) {
		increment_tp(&res, content[i]);
	}

	return res;
}

void Textmesh::increment_tp(Textmesh::Textpos* tp, char c) {
	if (c == '\n') {
		tp->y++;
		tp->x = 0;
	}
	else if (c == '\t') {
		tp->x = (tp->x / 4 + 1) * 4;
	}
	else if (c == '\v') {
		tp->y = (tp->y / 4 + 1) * 4;
	}
	else if (c == '\r') {
		tp->x = 0;
	}
	else if (c < 32);
	else {
		tp->x++;
	}
}