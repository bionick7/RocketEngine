#include "ellipsis.h"


Ellipsis::Ellipsis() {
	shader = (assets->get<Shader>(io::ResourceType::SHADER, "Ellipsis"))->ID;

	vp_matrix_ID = glGetUniformLocation(shader, "_VPM");
	active_ID = glGetUniformLocation(shader, "_Active");
	transform_ID = glGetUniformLocation(shader, "_OrbitTransform");
	ell_anomaly_ID = glGetUniformLocation(shader, "_EccAnomaly");
	ell_a_ID = glGetUniformLocation(shader, "_EllA");
	ell_b_ID = glGetUniformLocation(shader, "_EllB");
	camerabounding_ID = glGetUniformLocation(shader, "_CameraBounding");

	glGenBuffers(1, &vertex_buffer);
}

Ellipsis::~Ellipsis() {
	glDeleteBuffers(1, &vertex_buffer);
}

void Ellipsis::fill_vertex_buffer(int count, float gap) {
	segemnt_count = count;
	float* vertex_buffer_data = new float[count];
	float x = gap * 0.5f * pi_*2;
	float inc = pi_*2 / (count - 1.0) * (1.0 - gap);
	for (int i = 0; i < count; i++) {
		vertex_buffer_data[i] = x;
		x += inc;
		//std::cout << x << "; ";
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data[0]) * count, vertex_buffer_data, GL_STREAM_DRAW);
	delete[] vertex_buffer_data;
}

void Ellipsis::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible) {
		return;
	}
	glm::mat4 vpm = camera->vp_matrix * transform;

	glUseProgram(shader);
	glUniformMatrix4fv(vp_matrix_ID, 1, GL_FALSE, &vpm[0][0]);
	glUniformMatrix4fv(transform_ID, 1, GL_FALSE, &orbit_transform[0][0]);
	glUniform1f(ell_anomaly_ID, ell_anomaly);
	glUniform1f(ell_a_ID, a);
	glUniform1f(ell_b_ID, b);
	glUniform1f(active_ID, 1.0);
	glUniform2f(camerabounding_ID, camera->near_clip, camera->far_clip);
	//std::cout << camera->far_clip << std::endl;

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINE_STRIP, 0, segemnt_count);
	glDisableVertexAttribArray(0);
}