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
	vertex_buffer = get_quad_buffer(1);

	// initialize shader
	shader = (assets->get<Shader>(io::ResourceType::SHADER, "Circle"))->ID;
	//shader = ((Shader*)assets->get(io::ResourceType::SHADER, "Simple"))->ID;

	vp_ID = glGetUniformLocation(shader, "VP");
	transform_ID = glGetUniformLocation(shader, "Transform");

	thickness_ID = glGetUniformLocation(shader, "Edge");
	edge_ID = glGetUniformLocation(shader, "Space");
	screenwith_ID = glGetUniformLocation(shader, "ScreenWidth");
	radius_ID = glGetUniformLocation(shader, "Radius");
	camera_pos_ID = glGetUniformLocation(shader, "CameraPos");
	camera_dir_ID = glGetUniformLocation(shader, "CameraDir");

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

	LongVector world_position = LongVector(glm::vec3(transform[3]));  // Loss of precision, but eh
	LongVector center_to_camera = (LongVector)camera->world_position - world_position;  // In world space
	double camera_distance = longvec_magnitude(center_to_camera);
	LongVector center_to_camera_normalized = center_to_camera / camera_distance;

	LongVector v = LongVector(0.0, 1.0, 0.0);  // Chosen arbitrarily
	if (std::abs(longvec_dot(v, center_to_camera_normalized)) > 0.9) {	 // Not risking linear dependance
		v = LongVector(1.0, 0.0, 0.0);
	}

	v = (v - center_to_camera_normalized * longvec_dot(v, center_to_camera_normalized)).normalized();  // Project on plane to get orthogonal vector

	LongMatrix3x3 new_model_matrix_basis = LongMatrix3x3(
		-longvec_cross(v, center_to_camera_normalized),
		v,
		center_to_camera_normalized
	);  // Orthonormal and x-y plane is orthogonal to planet-canera vector

	double offset = radius * radius / camera_distance;  // Offset from midpoint to plane in render-world coordinate system
	double h = std::sqrt(radius * radius - offset * offset);  // plane scale in render-world coordinate system

	glm::mat4 new_model_matrix = LongMatrix4x4(new_model_matrix_basis).to_float_mat();	// Model to view such that model faces you

	new_model_matrix *= h;
	new_model_matrix[3] = transform[3] + glm::vec4((center_to_camera_normalized * offset).to_float_vec(), 0);

	// Set parameters
	glUseProgram(shader);
	glUniform1f(edge_ID, settings->line_thickness);
	glUniform1f(thickness_ID, settings->line_thickness);
	glUniform1f(screenwith_ID, settings->width);
	glUniform1f(radius_ID, radius);
	glUniform3f(camera_pos_ID, camera->world_position.x, camera->world_position.y, camera->world_position.z);
	glUniform3f(camera_dir_ID, camera->direction.x, camera->direction.y, camera->direction.z);
	glUniform3f(glGetUniformLocation(shader, "Normal"), center_to_camera_normalized.x, center_to_camera_normalized.y, center_to_camera_normalized.z);
	glUniformMatrix4fv(vp_ID, 1, GL_FALSE, &camera->vp_matrix[0][0]);
	glUniformMatrix4fv(transform_ID, 1, GL_FALSE, &new_model_matrix[0][0]);

	// Draw the shape
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);
}

glm::mat4 Circle::get_adjusted_transform(LongVector camera_position, glm::mat4 transform) {
	LongVector world_position = LongVector(glm::vec3(transform[3]));  // Loss of precision, but eh
	LongVector center_to_camera = camera_position - world_position;  // In world space
	double camera_distance = longvec_magnitude(center_to_camera);
	LongVector center_to_camera_normalized = center_to_camera / camera_distance;

	LongVector v = LongVector(0.0, 1.0, 0.0);  // Chosen arbitrarily
	if (std::abs(longvec_dot(v, center_to_camera_normalized)) > 0.9) {	 // Not risking linear dependance
		v = LongVector(1.0, 0.0, 0.0);
	}

	v = (v - center_to_camera_normalized * longvec_dot(v, center_to_camera_normalized)).normalized();  // Project on plane to get orthogonal vector

	LongMatrix3x3 new_model_matrix_basis = LongMatrix3x3(
		-longvec_cross(v, center_to_camera_normalized),
		v,
		center_to_camera_normalized
	);  // Orthonormal and x-y plane is orthogonal to planet-canera vector

	double offset = radius * radius / camera_distance;  // Offset from midpoint to plane in render-world coordinate system
	double h = std::sqrt(radius * radius - offset * offset);  // plane scale in render-world coordinate system

	glm::mat4 new_model_matrix = LongMatrix4x4(new_model_matrix_basis).to_float_mat();	// Model to view such that model faces you

	new_model_matrix *= h;
	new_model_matrix[3] = transform[3] + glm::vec4((center_to_camera_normalized * offset).to_float_vec(), 0);

	return new_model_matrix;
}

signed char Circle::draw_order() {
	return -1;
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