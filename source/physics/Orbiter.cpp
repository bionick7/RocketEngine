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
	position = get_point(get_situation(), 0).to_float_vec();

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

	position = get_point(get_situation(), 0).to_float_vec();
	meta_position = (position * position_scale).to_float_vec();
}

OrbitSituation Orbiter::get_situation() {
	return {
		orbit,
		current_anomaly,
		parent,
		ellipsis_data
	};
}

point_array_t Orbiter::get_points(bool meta) {
	point_array_t res = point_array_t();
	for (unsigned i = 0; i < points_number; i++) {
		res.push_back((get_point(get_situation(), double(i) / points_number) * (meta? position_scale : 1)).to_float_vec());
	}
	return res;
}

InternalEllipsisData calculate_ellipsis_data (OrbitData inp) {
	InternalEllipsisData ied;
	ied.a = inp.semi_major_axis * 2;
	ied.b = ied.a * sqrt(1 - inp.eccentricity * inp.eccentricity);
	ied.c = sqrt(ied.a*ied.a - ied.b*ied.b);

	glm::vec3 inclination_axis = glm::vec3(sin(-inp.longitude_of_ascending_note), 0, cos(-inp.longitude_of_ascending_note));
	ied.rotation = LongQuaternion(inp.inclination, inclination_axis, false);

	LongVector local_up = ied.rotation * LongVector(0.0, 1.0, 0.0);

	ied.rotation = LongQuaternion(inp.longitude_of_ascending_note + inp.argument_of_periapsis, local_up, true) * ied.rotation;

	ied.offset = ied.rotation * LongVector(ied.c, 0, 0);
	return ied;
}

LongVector get_point(OrbitSituation sit, double position){
	return sit.parent.position + sit.ellipse.rotation * LongVector(
		sin(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.a,
		0,
		cos(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.b
	);
}

double get_radius(OrbitSituation sit){
	double a = sit.data.semi_major_axis, ecc = sit.data.eccentricity;
	return (a * (1 - ecc * ecc)) / (1 + ecc * cos(sit.anomaly));
}

// Needs work
OrbitSituation project_orbit(OrbitSituation prev_data, double time) {
	for (int i = 0; i < 100; i++) {
		double distance = get_radius(prev_data);
		double linear_velocity = sqrt(prev_data.parent.mu * (2 / distance - 1 / prev_data.data.semi_major_axis));
		if (distance > 2 * prev_data.data.semi_major_axis) {
			linear_velocity = 0;
		}
		prev_data.anomaly += linear_velocity / distance;
	}
	return prev_data;
}

LongVector get_position(OrbitSituation sit) {
	InternalEllipsisData ellipsis_data = calculate_ellipsis_data(sit.data);
	return sit.parent.position + ellipsis_data.offset + ellipsis_data.rotation * LongVector(
		sin(2 * pi_ + sit.anomaly) * ellipsis_data.a,
		0,
		cos(2 * pi_ + sit.anomaly) * ellipsis_data.b
	);
}

LongVector get_velocity(OrbitSituation sit) {
	/*
	LongVector position = get_position(sit);
	double distance = sit.parent_distance();
	double linear_velocity = sqrt(sit.parent.mu * (2 / distance - 1 / sit.data.semi_major_axis));
	if (distance > 2 * sit.data.semi_major_axis) {
		linear_velocity = 0;
	}
	sit.anomaly += linear_velocity / distance;
	*/
	const double delta = 1.0f;
	return (get_position(sit) - get_position(project_orbit(sit, delta))) / delta;
}


double calculate_transfer_delta_v (OrbitSituation vessel, OrbitSituation target, double travel_time) {
	const int angle_number = 20;
	const int time_number = 30;

	const LongVector up = LongVector(0.0, 0.0, 1.0);
	const LongVector fore = LongVector(1.0, 0.0, 0.0);

	double* possible_routes = new double[angle_number * time_number];
	OrbitSituation* possible_situations = new OrbitSituation[angle_number * time_number];
	int routes_index = 0;
	
	// skip forward travel_time

	OrbitSituation future_target = project_orbit(target, travel_time);

	double
		end_angle = future_target.anomaly,
		end_radius = get_radius(future_target);

	for (double i = 0; i < time_number; i++) {
		// skip forward travel_time / time_number
		OrbitSituation future_vessel = project_orbit(vessel, travel_time);

		// Calculates inclination and aoan
		LongVector 
			relative_p1 = get_position(future_vessel) - future_target.parent.position,
			relative_p2 = get_position(future_target) - future_target.parent.position;

		LongVector plane_normal = longvec_cross(relative_p1, relative_p2);
		double inclination = longvec_angle(up, plane_normal);
		double argument = longvec_angle(fore, project2plane(plane_normal, up));

		double
			start_angle = future_vessel.anomaly,
			start_radius = get_radius(future_vessel);

		for (double j = 0; j < angle_number; j++) {
			// test for different angles
			double theta_plus = 2.0 * pi_ * j / angle_number;
			double rel_start_angle = start_angle + theta_plus;
			double rel_end_angle = end_angle + theta_plus;

			// math 
			// don't touch

			// we we know the rotation
			// find the eccentricity and a
			double ecc = (1.0 - start_radius / end_radius) / (start_radius / end_radius * cos(end_angle) - cos(start_angle));
			double a = start_radius * (1 + ecc * cos(rel_start_angle)) / (1 - ecc * ecc);

			OrbitData data = {
				ecc,
				a,
				inclination,
				argument,
				theta_plus, // +pi?
			};

			OrbitSituation transfer_orbit = {		// time of contact
				data,
				rel_start_angle,
				vessel.parent,
				calculate_ellipsis_data(data)
			};

			double dv_required = longvec_magnitude(get_velocity(transfer_orbit) - get_velocity(future_vessel));

			possible_situations[routes_index] = transfer_orbit;
			possible_routes[routes_index++] = dv_required;
		}
	}

	double best = INFINITY;
	OrbitSituation best_situation;
	for (int i = 0; i < angle_number * time_number; i++) {
		if (possible_routes[i] < best) {
			best = possible_routes[i];
			best_situation = possible_situations[i];
		}
	}

	delete[] possible_routes;
	delete[] possible_situations;
	return best;
}


OrbitData get_orbitdata_from_points(LongVector center, LongVector point1, LongVector point2, PolarCoordinates p1_polar, PolarCoordinates p2_polar) {

}

double calculate_minimum_delta_v(OrbitData parking1, Orbiter* parent1, OrbitData parking2, Orbiter* parent2, double czrrent_time, double travel_time) {
	return 0;
}
