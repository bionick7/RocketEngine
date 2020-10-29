#include "shape.h"
#include "Camera.h"

Shape::Shape()
{
}

Shape::~Shape()
{
}

void Shape::draw(const Camera* camera, glm::mat4 transform) {

}

signed char Shape::draw_order(){
	return 0;
}

GraphicsServer::GraphicsServer() {
	shape_vector_array = new std::vector<Shape*>[256];
}


void GraphicsServer::draw() {
	for (Camera* cam : all_cameras) {
		if (cam->is_active) {
			for (int i = 0; i < 256; i++) {
				for (Shape* s : shape_vector_array[i]) {
					if (s->visible && s->render_layers & cam->render_layers) {
						s->draw(cam, get_shape_transform(s));
					}
				}
			}
		}
	}
}