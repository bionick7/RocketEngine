#include "polygone.h"

glm::vec3 get_pos(glm::mat4 vp_matrix, glm::vec3 world_position, float aspect_ratio);

Polygone::Polygone() {
	shader = assets->get_shader(L"Simple");

	matrix_ID = glGetUniformLocation(shader, "VP");
	color_ID = glGetUniformLocation(shader, "Color");
}

Polygone::~Polygone() {

}

void Polygone::generate_point_array(point_array_t value) {
	vertex_buffer_size = value.size();
	vertexbuffers.clear();

	GLuint* vertexbuffers_ptxs = new GLuint[value.size()];

	glGenBuffers(vertex_buffer_size, vertexbuffers_ptxs);

	GLuint* vertex_arrays = new GLuint[vertex_buffer_size];
	glGenVertexArrays(vertex_buffer_size, vertex_arrays);

	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		glBindVertexArray(*(vertex_arrays + i));
		GLfloat vertex_buffer_data[6];

		unsigned other_index = (i == vertex_buffer_size - 1) ? 0 : i + 1;
		*(vertex_buffer_data + 0) = value[i].x;
		*(vertex_buffer_data + 1) = value[i].y;
		*(vertex_buffer_data + 2) = value[i].z;
		*(vertex_buffer_data + 3) = value[other_index].x;
		*(vertex_buffer_data + 4) = value[other_index].y;
		*(vertex_buffer_data + 5) = value[other_index].z;		

		glBindBuffer(GL_ARRAY_BUFFER, *(vertexbuffers_ptxs + i));
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STREAM_DRAW);

		vertexbuffers.push_back(*(vertexbuffers_ptxs + i));
	}
}

void Polygone::set_point_array(point_array_t value) {
	vertex_buffer_size = value.size();

	for (unsigned i = 0; i < vertex_buffer_size; i++) {
		GLfloat vertex_buffer_data[6];

		unsigned other_index = (i == vertex_buffer_size - 1) ? 0 : i + 1;
		*(vertex_buffer_data + 0) = value[i].x;
		*(vertex_buffer_data + 1) = value[i].y;
		*(vertex_buffer_data + 2) = value[i].z;
		*(vertex_buffer_data + 3) = value[other_index].x;
		*(vertex_buffer_data + 4) = value[other_index].y;
		*(vertex_buffer_data + 5) = value[other_index].z;

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[i]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer_data), vertex_buffer_data);
	}
}

void Polygone::draw(Camera* camera) {
	glm::mat4 transform_matrix = camera->vp_matrix;

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


glm::vec3 get_pos(glm::mat4 vp_matrix, glm::vec3 world_position, float aspect_ratio) {
	glm::vec4 world_pos = vp_matrix * glm::vec4(world_position, 1);
	return glm::vec3(world_pos.x * .1f, world_pos.y * .1f, -1);
}