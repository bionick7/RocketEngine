#include "space_ship.h"

SpaceShip::SpaceShip(io::DataStructurePtr data, Orbiter* parent) : Orbiter(data, parent) {
	io::DataStructurePtr visual = data->get_child("Visual");
	MeshResourcePtr mesh_source = assets->get<MeshResource>(io::ResourceType::MESH, visual->get_string("mesh"));
	main_shape = new Mesh(mesh_source);
	main_shape->render_layers = CAMERALAYER_CLOSE;
	add_shape(main_shape);
}

void SpaceShip::draw_step(double dt) {
	Orbiter::draw_step(dt);

	meta_position = (position - scene_manager->main_camera->get_focal_point()).to_float_vec();
	transform = glm::translate(glm::mat4(1), meta_position);
	
	bool in_view = scene_manager->main_camera->small_scale->distance < 10000 && (glm::dot(meta_position, meta_position) < 10000 * 10000);
	if (in_view && !main_shape->is_loaded) {
		main_shape->load();
	}
	else if (!in_view && main_shape->is_loaded) {
		main_shape->unload();
	}
}