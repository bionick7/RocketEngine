#include "Celestial.h"
#include "space_ship.h"
#include "mesh.h"

point_array_t get_equator_points(float radius) {
	point_array_t res = point_array_t();
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(0.5f * radius * glm::vec3(sin(i / 50.0 * pi_), 0, cos(i / 50.0 * pi_)));
	}
	return res;
}

point_array_t get_polar_points(float radius) {
	point_array_t res = point_array_t();
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(0.5f * radius * glm::vec3(0, cos(i / 50.0 * pi_), sin(i / 50.0 * pi_)));
	}
	for (double i = 0.0; i <= 100.0; i++) {
		res.push_back(0.5f * radius * glm::vec3(sin(i / 50.0 * pi_), cos(i / 50.0 * pi_), 0));
	}
	return res;
}

Celestial::Celestial(io::DataStructure* data, Orbiter* parent_ptx) : Orbiter::Orbiter(data, parent_ptx) {

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

	// One time test

	if (name == "Saturn") {/*
		Circle* m1 = new Circle(1);
		m1->render_layers = 2;
		add_shape(m1);
		Circle* km1 = new Circle(1000);
		km1->render_layers = 2;
		add_shape(km1);
		Circle* Mm1 = new Circle(1000000);
		Mm1->render_layers = 2;
		add_shape(Mm1);*/

		//Mesh* test_mesh = new Mesh((MeshResource*)assets->get(io::ResourceType::MESH, "Wire Monke"));
		//test_mesh->render_layers = 1;
		//test_mesh->source = (MeshResource*)assets->get(io::ResourceType::MESH, "Wire Monke");
		//test_mesh->apply();

		//add_shape(test_mesh);
		//add_shape(test_mesh);

		//test_mesh->source->store_in_file("E:\\Programing\\C++\\RocketEngine2\\resources\\objects\\test_wireframe_2.obj");
	}

	// end test


	for (io::DataStructure* ds : data->get_all_children()) {
		int orbiter_type = ds->get_int("type", 0, true);
		if (orbiter_type == 0) {
			add_sattelite(new Celestial(ds, this));
		} else if (orbiter_type == 1) {
			add_sattelite(new RingSystem(ds, this));
		} else if (orbiter_type == 2) {
			add_sattelite(new SpaceShip(ds, this));
		}
	}
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
	update_drawmode(global_blackboard->main_camera->large_scale);

	glm::mat4 rotation_matrix = get_focus_transformation() * glm::rotate(glm::mat4(1), (float)(global_blackboard->time * rotational_speed), glm::vec3(0, 1, 0));

	equator->shape_transform = rotation_matrix;
	polar_rings->shape_transform = rotation_matrix;
}

glm::mat4 Celestial::get_focus_transformation() {
	if (parent.is_valid)
		return parent.ptx->get_focus_transformation() * glm::rotate(glm::mat4(1), (float)(tilt * deg2rad), glm::vec3(1, 0, 0));
	return glm::rotate(glm::mat4(1), (float)(tilt * deg2rad), glm::vec3(1, 0, 0));
}

unsigned Celestial::add_sattelite(Orbiter* sattelite) {
	sattelites.push_back(sattelite);
	children.push_back(std::unique_ptr<Orbiter>(sattelite));
	return sattelites_num++;
}

Orbiter* const Celestial::get_sattelite(unsigned position) {
	if (position >= sattelites.size()) {
		throw new std::out_of_range("there are only " + std::to_string(sattelites.size()) + " sattelites on this body.\n You requested number " + std::to_string(position));
	}
	return sattelites[position];
}

void Celestial::update_drawmode(Camera c) {
	float tan = radius * position_scale * radius_enlarger / glm::distance(glm::vec3(c.view_matrix[3]), meta_position);
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

Type Celestial::get_type() {
	return Type::CELESTIAL;
}
