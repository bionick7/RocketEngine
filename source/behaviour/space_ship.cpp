#include "space_ship.h"

SpaceShip::SpaceShip(io::DataStructure* data, Orbiter* parent) : Orbiter(data, parent) {
	io::DataStructure* visual = data->get_child("Visual");
	MeshResource* mesh_source = (MeshResource*) assets->get(io::ResourceType::MESH, visual->get_string("mesh"));
	main_shape = new Mesh(mesh_source);
	main_shape->render_layers = 2;
	add_shape(main_shape);

	main_shape->load();
}

void SpaceShip::draw_step(double dt) {
	Agent::draw_step(dt);
	polygone->set_point_array(get_points(true, 0.005));

	meta_position = ((position - global_blackboard->main_camera->get_focal_point())).to_float_vec();
	bool loaded = global_blackboard->main_camera->small_scale.distance < 10000 && (glm::dot(meta_position, meta_position) < 10000 * 10000);
	// Currently everything is loaded into VRAM, but not everything is rendered
	main_shape->visible = loaded;
	transform = glm::translate(glm::mat4(1), meta_position);
}