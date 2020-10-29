#include "unscaling_billboard.h"

GLuint get_buffer(const float);

Billboard::Billboard(float rad)
{
	// Create a vertex array
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Get the vertex buffer from function below
	vertex_buffer = get_buffer(rad);

	// initialize shader
	shader = ((Shader*) assets->get(io::ResourceType::SHADER, "Simple"))->ID;
	transform_ID = glGetUniformLocation(shader, "VP");
}

Billboard::~Billboard() {
	// Nothing to see here, yet
}

void Billboard::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible) {
		return;
	}

	glm::vec4 screen_position = camera->vp_matrix * transform * glm::vec4(0, 0, 0, 1);
	if (screen_position.z < camera->near_clip || screen_position.z > camera->far_clip) {
		return;
	}
	screen_position /= (screen_position.w);
	glm::mat4 matrix = glm::translate(glm::mat4(1), glm::vec3(screen_position))* glm::scale(glm::mat4(1), glm::vec3(1, camera->aspect_ratio, 1));

	glUseProgram(shader);
	glUniformMatrix4fv(transform_ID, 1, GL_FALSE, &matrix[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_LINES, 0, 4);
	glDisableVertexAttribArray(0);
}

signed char Billboard::draw_order() {
	return 2;
}

/*
 *	Single functions
 */

GLuint get_buffer(const float size) {
	static const GLfloat g_vertex_buffer_data[] = {
		0, 0, 0,
		size, size, 0,
		0 ,0, 0,
		-size, size, 0
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexbuffer;
}