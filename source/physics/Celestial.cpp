#include "Celestial.h"

point_array_t get_equator_points(float radius) {
	point_array_t res = point_array_t();
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(radius * glm::vec3(sin(i / 50.0 * pi_), 0, cos(i / 50.0 * pi_)));
	}
	return res;
}

point_array_t get_polar_points(float radius) {
	point_array_t res = point_array_t();
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(radius * glm::vec3(0, cos(i / 50.0 * pi_), sin(i / 50.0 * pi_)));
	}
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(radius * glm::vec3(sin(i / 50.0 * pi_), cos(i / 50.0 * pi_), 0));
	}
	return res;
}

Celestial::Celestial() : Orbiter::Orbiter() {

}


Celestial::Celestial(io::DataStructurePtr data, Orbiter* parent_ptx) : Orbiter::Orbiter(data, parent_ptx) {
	setup(data, parent_ptx);
}

void Celestial::setup(io::DataStructurePtr data, Orbiter* parent_ptx) {

	radius = data->get_double("radius");
	tilt = data->get_double("tilt");
	rotational_speed = pi_ * 2.0 / data->get_double("day length");

	circle = new Circle(radius * position_scale * radius_enlarger);
	equator = new Polygone(LineType::DASHED);
	polar_rings = new Polygone(LineType::DASHED);
	billboard = new Billboard(.004);

	equator->set_point_array(get_equator_points(radius * position_scale * radius_enlarger));
	polar_rings->set_point_array(get_polar_points(radius * position_scale * radius_enlarger));

	add_shape(circle);
	add_shape(billboard);
	add_shape(equator);
	add_shape(polar_rings);

	for (io::DataStructurePtr ds : data->get_all_children()) {
		int orbiter_type = ds->get_int("type", 0, true);
		if (orbiter_type == 0) {
			add_sattelite(new Celestial(ds, this));
		}
		else if (orbiter_type == 1) {
			add_sattelite(new RingSystem(ds, this));
		}
		else if (orbiter_type == 2) {
			add_sattelite(new SpaceShip(ds, this));
		}
	}

	bind_advanced_variable(&surface_transform, "surface_transform", "linear_algebra", "Transform", sizeof(LongMatrix4x4));
	BIND(radius)
}

double Celestial::Mu() { return G * mass; }

void Celestial::orbiter_step(double dt , agent_id from) {
	Orbiter::orbiter_step(dt, from);
	for (Orbiter* sattelite : sattelites) {
		if (sattelite->id != from) {
			sattelite->orbiter_step(dt, id);
		}
	}
}

void Celestial::draw_step(double delta_t) {
	Orbiter::draw_step(delta_t);
	update_drawmode(scene_manager->main_camera->large_scale);

	// Maybe avoid using focus transform as planet orientation; It can be overridden!
	LongVector rotation_axis = get_focus_transformation().mul(LongVector(0.0, 1.0, 0.0), 0);
	surface_transform = get_focus_transformation() * glm::rotate(glm::mat4(1), (float)(scene_manager->time * rotational_speed), rotation_axis.to_float_vec()); // Implement using LMatrix

	equator->shape_transform = surface_transform.to_float_mat();
	polar_rings->shape_transform = surface_transform.to_float_mat();
}

LongMatrix4x4 Celestial::get_focus_transformation() { // Implement using LMatrix
	if (parent.is_valid)
		return parent.ptx->get_focus_transformation() * glm::rotate(glm::mat4(1), (float)(tilt * deg2rad), glm::vec3(1, 0, 0));  
	// rotated tilt° over x fixed x is a mistake
	// TODO: find data over exact planet rotation (detail)
	return glm::rotate(glm::mat4(1), (float)(tilt * deg2rad), glm::vec3(1, 0, 0));
}

unsigned Celestial::add_sattelite(Orbiter* sattelite) {
	sattelites.push_back(sattelite);
	add_child(sattelite);
	return sattelites_num++;
}

Orbiter* const Celestial::get_sattelite(unsigned position) {
	if (position >= sattelites.size()) {
		throw new std::out_of_range("there are only " + std::to_string(sattelites.size()) + " sattelites on this body.\n You requested number " + std::to_string(position));
	}
	return sattelites[position];
}

void Celestial::update_drawmode(Camera* c) {
	float tan = radius * position_scale * radius_enlarger / glm::distance(glm::vec3(c->view_matrix[3]), meta_position);
	//if (name == "Sun") std::cout << tan << std::endl;
	if (tan < .004) {
		draw_mode = DrawMode::Far;
	}
	else if (tan > .04) {
		draw_mode = DrawMode::CloseUp;
	}
	else {
		draw_mode = DrawMode::Medium;
	}
	circle->visible =		draw_mode != DrawMode::Far;
	billboard->visible =	draw_mode == DrawMode::Far;
	equator->visible =		draw_mode == DrawMode::CloseUp;
	polar_rings->visible =	draw_mode == DrawMode::CloseUp;
}

AgentType const Celestial::get_type() {
	return AgentType::CELESTIAL;
}

bool const Celestial::is_instance_of(AgentType other) {
	return Orbiter::is_instance_of(other) || other == get_type();
}
