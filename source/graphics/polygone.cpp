#include "polygone.h"


Polygone::Polygone(LineType plinetype) {
	shader = (assets->get<Shader>(io::ResourceType::SHADER, "Simple"))->ID;
	line_type = plinetype;

	vp_matrix_ID = glGetUniformLocation(shader, "VP");
	active_ID = glGetUniformLocation(shader, "_Active");

	glGenBuffers(1, &vertex_buffer);
}

Polygone::~Polygone() {
	glDeleteBuffers(1, &vertex_buffer);
}

void Polygone::set_point_array(point_array_t value) {
	point_count = value.size();
	GLfloat* vertex_buffer_data = new GLfloat[3 * point_count];
	for (unsigned i = 0; i < point_count; i++) {
		*(vertex_buffer_data + 0 + 3 * i) = value[i].x;
		*(vertex_buffer_data + 1 + 3 * i) = value[i].y;
		*(vertex_buffer_data + 2 + 3 * i) = value[i].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data[0]) * 3 * point_count, vertex_buffer_data, GL_STREAM_DRAW);
	delete[] vertex_buffer_data;
}

void Polygone::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible) {
		return;
	}
	glm::mat4 transform_matrix = camera->vp_matrix * transform;

	glUseProgram(shader);
	glUniformMatrix4fv(vp_matrix_ID, 1, GL_FALSE, &transform_matrix[0][0]);
	glUniform1f(active_ID, 1.0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	switch (line_type)
	{
	case CONTINUOUS: glDrawArrays(GL_LINE_STRIP, 0, point_count); break;
	case CLOSED:	 glDrawArrays(GL_LINE_LOOP,	 0, point_count); break;
	case DASHED:	 glDrawArrays(GL_LINES,		 0, point_count); break;
	default: break;
	}
	glDisableVertexAttribArray(0);
}