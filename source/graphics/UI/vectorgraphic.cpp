#include "vectorgraphic.h"

Line::Line(float _x_start, float _y_start, float _x_end, float _y_end) {
	x_start = _x_start;
	y_start = _y_start;
	x_end = _x_end;
	y_end = _y_end;
}

Drawing::Drawing(float args[], int len) {
	lines = std::vector<Line>();
	for (size_t i = 0; i < len / 4; i++) {
		lines.push_back(Line(
			args[i * 4 + 0],
			args[i * 4 + 1],
			args[i * 4 + 2],
			args[i * 4 + 3]
		));
	}
}


GLuint shader, matrix_ID, color_ID;

void draw_drawing(Drawing drawing, pixel x, pixel y, pixel width, pixel height) {
	shader = assets->get_shader(L"Simple");
	matrix_ID = glGetUniformLocation(shader, "VP");
	color_ID = glGetUniformLocation(shader, "Color");

	unsigned int vertex_buffer_size = drawing.lines.size();
	std::vector<GLuint> vertexbuffers = std::vector<GLuint>();

	GLuint* vertexbuffers_ptxs = new GLuint[vertex_buffer_size];

	glGenBuffers(vertex_buffer_size, vertexbuffers_ptxs);

	GLuint* vertex_arrays = new GLuint[vertex_buffer_size];
	glGenVertexArrays(vertex_buffer_size, vertex_arrays);

	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		glBindVertexArray(*(vertex_arrays + i));
		GLfloat vertex_buffer_data[6];

		Line line = drawing.lines[i];

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

	glm::mat4 transform_matrix = glm::mat4(1.0f);

	glUseProgram(shader);
	glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &transform_matrix[0][0]);
	glUniform3f(color_ID, settings->draw.r, settings->draw.g, settings->draw.b);

	glEnableVertexAttribArray(0);
	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableVertexAttribArray(0);
}