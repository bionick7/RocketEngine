#include "Orbiter.h"

std::vector<Orbiter*> all_orbiters = std::vector<Orbiter*>();

Orbiter::Orbiter(cfg::DataStructure* data)
{
	name = data->get_name();

	parent = CelestialEssentials();
	if (data->get_int("no parent", 13, true) != 13) {
		parent.is_valid = false;
	} else {
		parent.is_valid = true;
		parent.mu = data->parent->get_double("mass") * G;
	}

	orbit = OrbitData();
	orbit.eccentricity = data->get_double("Ecc");
	orbit.semi_major_axis = data->get_double("SMA");
	orbit.inclination = data->get_double("Inc");
	orbit.argument_of_periapsis = data->get_double("AoP");
	orbit.longitude_of_ascending_note = data->get_double("LoA");

	current_anomaly = 0;

	ellipsis_data = calculate_ellipsis_data(orbit);
	polygone = new Polygone();
	polygone->generate_point_array(get_points(true));
	center_position = parent.position;
	position = get_point(0);

	all_orbiters.push_back(this);
}

Orbiter::~Orbiter()
{

}

void Orbiter::draw(Camera* cam) {
	polygone->set_point_array(get_points(true));
	polygone->draw(cam);
}

void Orbiter::update(double delta_time) {
	if (!parent.is_valid) return;
	double distance = parent.position.distance_to(position);
	double linear_velocity = sqrt(parent.mu * (2 / distance - 1 / orbit.semi_major_axis)) * delta_time * time_scale;
	if (distance > 2 * orbit.semi_major_axis) {
		linear_velocity = 0;
	}
	current_anomaly += linear_velocity / distance;
	center_position = parent.position + ellipsis_data.offset;

	position = get_point(0);
	meta_position = (position * position_scale).to_float_vec();
}

point_array_t Orbiter::get_points(bool meta) {
	point_array_t res = point_array_t();
	for (unsigned i = 0; i < points_number; i++) {
		res.push_back((get_point(double(i) / points_number) * (meta? position_scale : 1)).to_float_vec());
	}
	return res;
}

LongVector Orbiter::get_point(double position) {
	return center_position + ellipsis_data.rotation * LongVector(
		sin(-position * 2 * pi_ + current_anomaly) * ellipsis_data.a,
		0,
		cos(-position * 2 * pi_ + current_anomaly) * ellipsis_data.b
	);
}

InternalEllipsisData calculate_ellipsis_data (OrbitData inp) {
	InternalEllipsisData ied;
	ied.a = inp.semi_major_axis * 2 / (sqrt(1 - inp.eccentricity * inp.eccentricity) + 1);
	ied.b = inp.semi_major_axis * 2 - ied.a;
	ied.c = sqrt(ied.a*ied.a - ied.b*ied.b);

	glm::vec3 inclination_axis = glm::vec3(sin(-inp.longitude_of_ascending_note), 0, cos(-inp.longitude_of_ascending_note));
	ied.rotation = LongQuaternion(inp.inclination, inclination_axis, false);

	LongVector local_up = ied.rotation * LongVector(0.0, 1.0, 0.0);

	ied.rotation = LongQuaternion(inp.longitude_of_ascending_note + inp.argument_of_periapsis, local_up, true) * ied.rotation;

	ied.offset = ied.rotation * LongVector(ied.c, 0, 0);
	return ied;
}
