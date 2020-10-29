#include "lineset.h"

LineSet::LineSet() {
	shader = ((Shader*)assets->get(io::ResourceType::SHADER, "Simple"))->ID;

	vp_matrix_ID = glGetUniformLocation(shader, "VP");
	color_ID = glGetUniformLocation(shader, "Color");

	glGenBuffers(1, &vertex_buffer);
	glGenVertexArrays(1, &array_id);
}

LineSet::~LineSet() {
	glDeleteBuffers(1, &vertex_buffer);
}

void LineSet::update(linearray_t value) {
	line_count = value.size();
	GLfloat* vertex_buffer_data = new GLfloat[6 * line_count];
	for (unsigned i = 0; i < line_count; i++) {
		*(vertex_buffer_data + 0 + i * 6) = value[i].start.x;
		*(vertex_buffer_data + 1 + i * 6) = value[i].start.y;
		*(vertex_buffer_data + 2 + i * 6) = value[i].start.z;
		*(vertex_buffer_data + 3 + i * 6) = value[i].end.x;
		*(vertex_buffer_data + 4 + i * 6) = value[i].end.y;
		*(vertex_buffer_data + 5 + i * 6) = value[i].end.z;
	}

	glBindVertexArray(array_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data[0]) * 6 * line_count, vertex_buffer_data, GL_STREAM_DRAW);
	delete[] vertex_buffer_data;
}

void LineSet::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible) {
		return;
	}
	glm::mat4 transform_matrix = camera->vp_matrix * transform;

	glUseProgram(shader);
	glUniformMatrix4fv(vp_matrix_ID, 1, GL_FALSE, &transform_matrix[0][0]);
	glUniform3f(color_ID, settings->draw.r, settings->draw.g, settings->draw.b);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 2 * line_count);
	glDisableVertexAttribArray(0);
}