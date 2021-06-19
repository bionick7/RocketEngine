#include "graphic_manager.h"
#include "Camera.h"
#include "radio.h"
#include "ui_manager.h"

Shape::Shape() {

}

Shape::~Shape() {

}

void Shape::draw(const Camera* camera, glm::mat4 transform) {

}

signed char Shape::draw_order(){
	return 0;
}

GLuint generate_frame_buffer() {
	GLuint temp_frame_buffer_name = 0;
	glGenFramebuffers(1, &temp_frame_buffer_name);
	glBindFramebuffer(GL_FRAMEBUFFER, temp_frame_buffer_name);
	return temp_frame_buffer_name;
}

GLuint generate_quad_vertex_buffer() {
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	return quad_vertexbuffer;
}

// Has to be called when screen is resized !!!
void setup_render_textures(GraphicsManagerDataLookup* in, int width, int height) {
	// The texture we're going to render to
	glBindTexture(GL_TEXTURE_2D, in->render_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, in->render_texture, 0);

	// Depth texture
	/*glBindTexture(GL_TEXTURE_2D, in->depth_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, settings->width, settings->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE); */

	// The depth buffer is generated here, since it's screensize-dependent
	glBindRenderbuffer(GL_RENDERBUFFER, in->depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, in->depthrenderbuffer);
}

//void GraphicsManager::on_window_resized() {
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, settings->width, settings->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//}

void GraphicsManager::interprete_signal(RadioSignal signal) {
	if (signal.identifier == "on_window_resized") {
		setup_render_textures(&gl_ids, settings->width, settings->height);
	}
}

bool GraphicsManager::setup() {
	gl_ids.post_process_shader = (assets->get<Shader>(io::ResourceType::SHADER, "PostProcess"))->ID;
	gl_ids.frame_buffer = generate_frame_buffer();
	gl_ids.quad_vertex_buffer = generate_quad_vertex_buffer();
	glGenTextures(1, &gl_ids.render_texture);
	//glGenTextures(1, &gl_ids.depth_texture);
	glGenRenderbuffers(1, &gl_ids.depthrenderbuffer);
	setup_render_textures(&gl_ids, settings->width, settings->height);

	gl_ids.shader_time = glGetUniformLocation(gl_ids.post_process_shader, "_Time");
	gl_ids.shader_texture = glGetUniformLocation(gl_ids.post_process_shader, "_ScreenTexture");
	//gl_ids.depth_texture = glGetUniformLocation(gl_ids.post_process_shader, "_DephtTexture");
	gl_ids.shader_background = glGetUniformLocation(gl_ids.post_process_shader, "_BackgroundColor");
	gl_ids.shader_foreground = glGetUniformLocation(gl_ids.post_process_shader, "_ForegroundColor");

	post_processing_enabled = settings->render_flags & RENDER_FLAGS_POST_PROCESSING;

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		errlog("Error when configuring framebuffer");
		return false;
	}

	if (ui_manager != nullptr) {
		ui_manager->radio->connect("window_resized", radio, RadioSignal("on_window_resized"));
	}

	return true;
}

GraphicsManager::GraphicsManager() {

}

GraphicsManager::~GraphicsManager() {
	glDeleteTextures(1, &gl_ids.render_texture);
	//glDeleteTextures(1, &gl_ids.depth_texture);
	glDeleteRenderbuffers(1, &gl_ids.depthrenderbuffer);
	glDeleteBuffers(1, &gl_ids.quad_vertex_buffer);
	glDeleteFramebuffers(1, &gl_ids.frame_buffer);
}

void GraphicsManager::step(double dt) {
	Manager::step(dt);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(settings->line_thickness);

	// Render to our framebuffer
	if (post_processing_enabled) {
		glBindFramebuffer(GL_FRAMEBUFFER, gl_ids.frame_buffer);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	glViewport(0, 0, settings->width, settings->height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Simple render loop
	for (Camera* cam : all_cameras) {
		if (cam->is_active) {
			render_loop(cam);
		}
	}
}

void GraphicsManager::post_processing() {
	if (!post_processing_enabled) {
		return;
	}
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, settings->width, settings->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create and compile our GLSL program from the shaders
	glUseProgram(gl_ids.post_process_shader);

	glUniform1i(gl_ids.shader_texture, 0);
	//glUniform1i(gl_ids.depth_texture, 1);
	glUniform1f(gl_ids.shader_time, glfwGetTime());  // independent of game time
	glUniform3f(gl_ids.shader_background, settings->background.r, settings->background.g, settings->background.b);
	glUniform3f(gl_ids.shader_foreground, settings->draw.r, settings->draw.g, settings->draw.b);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_ids.render_texture);
	glActiveTexture(GL_TEXTURE0 + 1);
	//glBindTexture(GL_TEXTURE_2D, gl_ids.depth_texture);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, gl_ids.quad_vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
	glDisableVertexAttribArray(0);
}

void GraphicsManager::draw_geometry(Camera* cam) {
	for (int i = 0; i < 256; i++) {
		for (Shape* s : shape_vector_array[i]) {
			if (s->visible && (s->render_layers & cam->render_layers)) {
				s->draw(cam, get_shape_transform(s));
			}
		}
	}
}

void GraphicsManager::render_loop(Camera* cam) {
	draw_geometry(cam);
}