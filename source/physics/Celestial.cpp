#include "Celestial.h"

#include <iostream>


double Celestial::Mu() { return G * mass; }

Celestial::Celestial(cfg::DataStructure* data) : Orbiter::Orbiter(data) {

	radius = data->get_double("radius");
	circle = new Circle(radius * position_scale * 10);

	for (cfg::DataStructure* ds : data->get_all_children()) {
		sattelites.push_back(new Celestial(ds));
	}
}

void Celestial::draw(Camera* cam) {
	Orbiter::draw(cam);
	//std::cout << "Circle world position" << std::endl;
	//print_vector(meta_position.to_float_vec());
	circle->world_position = meta_position;
	circle->draw_c(cam);
	for (Orbiter* sattelite : sattelites) {
		sattelite->draw(cam);
	}
}

void Celestial::update(double delta_t) {
	Orbiter::update(delta_t);
	for (Orbiter* sattelite : sattelites) {
		sattelite->parent.position = position;
		sattelite->update(delta_t);
	}
}

Orbiter* Celestial::get_sattelite(unsigned position) {
	if (position >= sattelites.size()) {
		throw new std::out_of_range("there are only " + std::to_string(sattelites.size()) + " sattelites on this body.\n You requested number " + std::to_string(position));
	}
	return sattelites[position];
}