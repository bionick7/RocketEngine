#include "mesh.h"

Mesh::Mesh(MeshResourcePtr _source) : Shape::Shape()
{
	shader = (assets->get<Shader>(io::ResourceType::SHADER, "Simple"))->ID;

	vp_matrix_ID = glGetUniformLocation(shader, "VP");
	active_ID = glGetUniformLocation(shader, "_Active");

	segment_count = 0;
	source = _source;
}

Mesh::Mesh() : Mesh::Mesh(nullptr) { }

Mesh::~Mesh() {
	unload();
}

void Mesh::load()
{
	if (source != nullptr) {
		segment_count = source->get_segment_count();
	}
	//segment_count = 3;
	if (segment_count < 1) {
		std::cerr << " ! Failed to apply Mesh " << (source == nullptr ? "NULL" : source->name) << " (" << (parent == nullptr ? "NULL" : parent->name) << ")\n";
		return;
	}
	GLfloat* vertex_buffer_data = new GLfloat[6 * segment_count];
	for (unsigned i = 0; i < source->index_count; i ++) {
		vertex_buffer_data[0 + 3 * i] = source->vertecies[source->indecies[i]].x;
		vertex_buffer_data[1 + 3 * i] = source->vertecies[source->indecies[i]].y;
		vertex_buffer_data[2 + 3 * i] = source->vertecies[source->indecies[i]].z;
		//std::cout << "(" << source->vertecies[source->indecies[i]].x << "," << source->vertecies[source->indecies[i]].y << "," << source->vertecies[source->indecies[i]].z << "); ";
	}
	//std::cout << std::endl;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data[0]) * segment_count * 6, vertex_buffer_data, GL_STATIC_DRAW);
	delete[] vertex_buffer_data;
	is_loaded = true;

	segment_count = source->get_segment_count();
}


void Mesh::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible || !is_loaded) {
		return;
	}
	glm::mat4 transform_matrix = camera->vp_matrix* transform;

	glUseProgram(shader);
	glUniformMatrix4fv(vp_matrix_ID, 1, GL_FALSE, &transform_matrix[0][0]);
	glUniform1f(active_ID, 1.0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, segment_count * 6);
	glDisableVertexAttribArray(0);
}

void Mesh::unload() {
	if (is_loaded) {
		glDeleteBuffers(1, &vertex_buffer);
		is_loaded = false;
	}
}

signed char Mesh::draw_order()
{
	return 1;
}