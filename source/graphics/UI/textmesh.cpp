#include "textmesh.h"

bool is_initialized = false;

float test_character_vectors[]{ 0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.25f, 0.5f, 0.75, 0.5f };
Drawing test = Drawing(test_character_vectors, 12);

Textmesh::Textmesh(std::wstring _content, Font _font) {
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

std::wstring Textmesh::get_content() {
	return content;
}

void Textmesh::set_content(std::wstring _content) {
	content = _content;
	recalculate();
}

void Textmesh::set_transform(float _x, float _y, float _sizex, float _sizey) {
	if (_sizex < 0) {
		_sizex = INFINITY;
	}
	UIElement::set_transform(_x, _y, _sizex, _sizey);
}

void Textmesh::recalculate() {
	UIElement::recalculate();

	unsigned int length = content.length();
	if (length == 0)
		return;

	float size = std::min(widt / length, height);

	if (vector) {
		GLuint shader = assets->get_shader(L"Simple");
		UVBufferID_MatrixID = glGetUniformLocation(shader, "VP");
		ColorID = glGetUniformLocation(shader, "Color");

		std::vector<Line> lines = std::vector<Line>();
		// Fill buffers
		for (unsigned int i = 0; i < length; i++) {
			Drawing d = test;

			for (Line l : test.lines) lines.push_back(l);
		}

		unsigned int vertex_buffer_size = lines.size();
		std::vector<GLuint> vertexbuffers = std::vector<GLuint>();

		GLuint* vertexbuffers_ptxs = new GLuint[vertex_buffer_size];

		glGenBuffers(vertex_buffer_size, vertexbuffers_ptxs);

		GLuint* vertex_arrays = new GLuint[vertex_buffer_size];
		glGenVertexArrays(vertex_buffer_size, vertex_arrays);

		for (unsigned i = 0; i < vertex_buffer_size; i++) {
			glBindVertexArray(*(vertex_arrays + i));
			GLfloat vertex_buffer_data[6];

			Line line = lines[i];

			*(vertex_buffer_data + 0) = line.x_start;
			*(vertex_buffer_data + 1) = line.y_start;
			*(vertex_buffer_data + 2) = 0;
			*(vertex_buffer_data + 3) = line.x_end;
			*(vertex_buffer_data + 4) = line.y_end;
			*(vertex_buffer_data + 5) = 0;

			glBindBuffer(GL_ARRAY_BUFFER, *(vertexbuffers_ptxs + i));
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STREAM_DRAW);

			vertexbuffers.push_back(*(vertexbuffers_ptxs + i));
		}
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

			wchar_t character = content[i];
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

		glUseProgram(assets->get_shader(L"Simple"));
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