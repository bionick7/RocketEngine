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
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Get the vertex buffer from function below
	vertex_buffer = get_quad_buffer(.5f);

	// initialize shader
	shader = assets->get_shader("Circle");

	matrix_ID = glGetUniformLocation(shader, "VP");
	color_ID = glGetUniformLocation(shader, "Color");
	screensize_ID = glGetUniformLocation(shader, "ScreenSizeHlf");
	radii_ID = glGetUniformLocation(shader, "Radii");
	midpos_ID = glGetUniformLocation(shader, "MidPos");
	camera_right_ID = glGetUniformLocation(shader, "CameraRight_worldspace");
	camera_up_ID = glGetUniformLocation(shader, "CameraUp_worldspace");
	billboard_pos_ID = glGetUniformLocation(shader, "BillboardPos");
	billboard_size_ID = glGetUniformLocation(shader, "BillboardSize");

	radius = rad / 10;
}

Circle::~Circle() { 
	// Nothing to see here, yet
}

void Circle::draw_c(Camera* camera) {
	// Get circle matrix form below
	
	glm::vec3 CameraRight_worldspace = glm::vec3(camera->view_matrix[0][0], camera->view_matrix[1][0], camera->view_matrix[2][0]);
	glm::vec3 CameraUp_worldspace = glm::vec3(camera->view_matrix[0][1], camera->view_matrix[1][1], camera->view_matrix[2][1]);
	
	glm::vec3 mid_pos = world_position;
	glm::vec4 gl_mid_Position = camera->vp_matrix * glm::vec4(mid_pos, 1);
	glm::vec3 screen_mid_position = get_screen_coordinates(camera->screensize, glm::vec3(gl_mid_Position));

	float half_extends = radius * .5 * camera->screensize.y / (tan(camera->fov) * (gl_mid_Position.z));
	// (need work here ^^^)

	// set shader
	glUseProgram(shader);
	glUniform2ui(screensize_ID, camera->screensize.x / 2, camera->screensize.y / 2);
	float min_rad = half_extends - settings->line_thickness;
	glUniform2f(radii_ID, min_rad * min_rad, half_extends * half_extends);
	glUniform3f(midpos_ID, screen_mid_position.x, screen_mid_position.y, screen_mid_position.z);
	glUniform3f(color_ID, settings->draw.r, settings->draw.g, settings->draw.b);
	glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &camera->vp_matrix[0][0]);
	glUniform3f(camera_right_ID, CameraRight_worldspace.x, CameraRight_worldspace.y, CameraRight_worldspace.z);
	glUniform3f(camera_up_ID, CameraUp_worldspace.x, CameraUp_worldspace.y, CameraUp_worldspace.z);
	glUniform3f(billboard_pos_ID, world_position.x, world_position.y, world_position.z);
	glUniform2f(billboard_size_ID, radius, radius);

	// Draw the shape
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);
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