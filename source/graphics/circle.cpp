#include "circle.h"

GLuint get_quad_buffer(const float);

glm::vec3 get_screen_coordinates(Screenpos screen_size, glm::vec3 gl_coordinates) {
	return glm::vec3(
		screen_size.x * (gl_coordinates.x + 1) * .5f,
		screen_size.y * (gl_coordinates.y + 1) * .5f,
		gl_coordinates.z
	);
}

Circle::Circle(double rad)
{
	// Create a vertex array

	// Get the vertex buffer from function below
	vertex_buffer = get_quad_buffer(.5f);

	// initialize shader
	shader = ((Shader*)assets->get(io::ResourceType::SHADER, "Circle"))->ID;

	viewmatrix_ID = glGetUniformLocation(shader, "ViewMatrix");
	projectionmatrix_ID = glGetUniformLocation(shader, "ProjectionMatrix");
	transform_ID = glGetUniformLocation(shader, "Transform");
	color_ID = glGetUniformLocation(shader, "Color");
	backgroundcolor_ID = glGetUniformLocation(shader, "BGColor");
	thickness_ID = glGetUniformLocation(shader, "Edge");
	edge_ID = glGetUniformLocation(shader, "Space");
	screenwith_ID = glGetUniformLocation(shader, "ScreenWidth");

	radius = rad;
}

Circle::~Circle() {
	glDeleteBuffers(1, &vertex_buffer);
}

glm::vec3 translation(glm::mat4 m) {
	return glm::vec3(m[3][0], m[3][1], m[3][2]);
}

void Circle::draw(const Camera* camera, glm::mat4 transform) {
	if (!visible) {
		return;
	}
	// Get circle matrix form below
	// set shader

	transform = glm::scale(transform, glm::vec3(radius));

	/*
	glm::vec4 mid_point = camera->view_matrix * glm::vec4(glm::vec3(transform[3]), 1);
	glm::mat4 transform_base = transform;
	transform_base[3].x = 0;
	transform_base[3].y = 0;
	transform_base[3].z = 0;
	glm::mat4 billboard_transform = glm::translate(glm::mat4(1), glm::vec3(mid_point));
	glm::mat4 final_matrix = camera->projection_matrix * billboard_transform * transform_base;
	*/

	glUseProgram(shader);
	glUniform3f(color_ID, settings->draw.r, settings->draw.g, settings->draw.b);
	glUniform3f(backgroundcolor_ID, settings->background.r, settings->background.g, settings->background.b);
	glUniform1f(edge_ID, settings->line_thickness);
	glUniform1f(thickness_ID, settings->line_thickness);
	glUniform1f(screenwith_ID, settings->width);
	glUniformMatrix4fv(projectionmatrix_ID, 1, GL_FALSE, &camera->projection_matrix[0][0]);
	glUniformMatrix4fv(viewmatrix_ID, 1, GL_FALSE, &camera->view_matrix[0][0]);
	glUniformMatrix4fv(transform_ID, 1, GL_FALSE, &transform[0][0]);


	// Draw the shape
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);
}

signed char Circle::draw_order() {
	return 1;
}

/*
 *	Single functions
 */

GLuint get_quad_buffer(const float size) {
	static const GLfloat g_vertex_buffer_data[] = {
			-size, -size, 0.0f,
			 size, -size, 0.0f,
			-size,  size, 0.0f,
			 size, -size, 0.0f,
			-size,  size, 0.0f,
			 size,  size, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexbuffer;
}