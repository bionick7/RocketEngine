#include "Orbiter.h"

double get_anomaly_at_time(OrbitSituation, double);

LongVector CelestialEssentials::position() { 
	return is_valid ? ptx->position : LongVector(0.0, 0.0, 0.0);
}

Orbiter::Orbiter() {
}


Orbiter::Orbiter(io::DataStructurePtr data, Orbiter* parent_ptx) : Agent() {
	setup(data, parent_ptx);
}

Orbiter::~Orbiter() { }

void Orbiter::setup(io::DataStructurePtr data, Orbiter* parent_ptx) {
	name = data->get_name();

	parent = CelestialEssentials();
	if (parent_ptx == nullptr) {
		parent.is_valid = false;
	}
	else {
		parent.is_valid = true;
		parent.mu = data->get_parent()->get_double("mass") * G;
		parent.ptx = parent_ptx;
		parent.reference = LongQuaternion(data->get_parent()->get_double("tilt"), LongVector(1.0, 0.0, 0.0), false);
	}

	orbit = OrbitData();
	orbit.eccentricity = data->get_double("Ecc");
	orbit.semi_major_axis = data->get_double("SMA");
	orbit.inclination = data->get_double("Inc");
	orbit.argument_of_periapsis = data->get_double("AoP");
	orbit.longitude_of_ascending_node = data->get_double("LoA");

	current_anomaly = 0;

	ellipsis_data = calculate_ellipsis_data(orbit);
	ellipsis = new Ellipsis();
	ellipsis->independent = true;
	ellipsis->fill_vertex_buffer(100, 0.0001);
	center_position = parent.position();
	position = get_point(get_situation(), 0);

	on_orbit_changed();

	initial_situation = get_situation();
	add_shape((Shape*)ellipsis);

	is_setup = true;
}

void Orbiter::physics_step(double dt) {
	Agent::physics_step(dt);
	if (!parent.is_valid) {
		orbiter_step(dt, 0);
	}
}

void Orbiter::orbiter_step(double dt, agent_id from) {
	if (!parent.is_valid) {
		// Root specific
		ellipsis->visible = false;
		position = LongVector(0.0, 0.0, 0.0);
		return;
	}

	current_anomaly = get_anomaly_at_time(initial_situation, scene_manager->time);
	position = get_point(get_situation(), 0);

	// recursive nature
	// Assuming parent is valid
	if (parent.ptx->id != from) {
		parent.ptx->orbiter_step(dt, id);
	}

	/*
	//center_position = parent.position() + ellipsis_data.offset;
	double distance = get_point(get_situation(), 0).distance_to(parent.position());
	//double distance = orbit.semi_major_axis * sqrt(1 - cos(current_anomaly) * cos(current_anomaly) * orbit.eccentricity * orbit.eccentricity);
	double linear_velocity = sqrt(parent.mu * (2.0 / distance - 1.0 / orbit.semi_major_axis)) * dt * time_scale;
	if (distance > 2 * orbit.semi_major_axis) {
		linear_velocity = 0;
	}
	current_anomaly += linear_velocity / distance;

	position = get_point(get_situation(), 0);
	*/
}

void Orbiter::draw_step(double dt) {
	Agent::draw_step(dt);
	ellipsis->ell_anomaly = current_anomaly;
	ellipsis->orbit_transform = get_orbit_transform().to_float_mat();

	meta_position = ((position - scene_manager->main_camera->get_focal_point()) * position_scale).to_float_vec();
	transform = glm::translate(glm::mat4(1), meta_position);
}

void Orbiter::on_orbit_changed() {
	ellipsis->a = ellipsis_data.a * position_scale;
	ellipsis->b = ellipsis_data.b * position_scale;

	rotation_matrix = get_rotation_matrix(ellipsis_data.rotation * parent.reference);
}

AgentType const Orbiter::get_type() {
	return AgentType::ORBITER;
}

bool const Orbiter::is_instance_of(AgentType other) {
	return Agent::is_instance_of(other) || other == get_type();
}

OrbitSituation Orbiter::get_situation() {
	return {
		orbit,
		current_anomaly,
		parent,
		ellipsis_data
	};
}

LongMatrix4x4 Orbiter::get_orbit_transform() {  // rotation matrix calculated in on_orbit_changed
	LongVector off = parent.position() + ellipsis_data.offset - scene_manager->main_camera->get_focal_point();
	LongMatrix4x4 translation_matrix = get_translation_matrix(off * position_scale);

	return rotation_matrix * translation_matrix;
}

point_array_t Orbiter::get_points(bool meta, double clearance) {
	OrbitSituation sit = get_situation();
	point_array_t res = point_array_t();
	
	if (meta) {
		LongMatrix4x4 m = get_orbit_transform();
		for (unsigned i = 0; i <= points_number; i++) {
			double position = clearance + double(i) / points_number * (1 - 4 * clearance);
			LongVector vec = LongVector(
				sin(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.a,
				0,
				cos(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.b
			);

			res.push_back(m.mul(vec, 1).to_float_vec());
		}
	}
	else {
		for (unsigned i = 0; i <= points_number; i++) {
			res.push_back(
				get_point(sit, clearance + double(i) / points_number * (1 - 4 * clearance)).to_float_vec()
			);
		}
	}
	return res;
}

InternalEllipsisData calculate_ellipsis_data (OrbitData inp) {
	InternalEllipsisData ied;
	ied.a = inp.semi_major_axis;
	ied.b = ied.a * sqrt(1 - inp.eccentricity * inp.eccentricity);
	ied.c = sqrt(ied.a*ied.a - ied.b*ied.b);

	glm::vec3 inclination_axis = glm::vec3(sin(-inp.longitude_of_ascending_node), 0, cos(-inp.longitude_of_ascending_node));
	ied.rotation = LongQuaternion(inp.inclination, inclination_axis, false);

	LongVector local_up = ied.rotation * LongVector(0.0, 1.0, 0.0);

	ied.rotation = LongQuaternion(inp.longitude_of_ascending_node + inp.argument_of_periapsis, local_up, true) * ied.rotation;

	ied.offset = ied.rotation * LongVector(ied.c, 0, 0);
	return ied;
}

LongVector get_point(OrbitSituation sit, double position){
	return sit.parent.position() + sit.ellipse.offset + (sit.ellipse.rotation * sit.parent.reference) * LongVector(
		sin(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.a,
		0,
		cos(-position * 2 * pi_ + sit.anomaly) * sit.ellipse.b
	);
}

double get_radius(OrbitSituation sit){
	double a = sit.data.semi_major_axis, ecc = sit.data.eccentricity;
	return (a * (1 - ecc * ecc)) / (1 + ecc * cos(sit.anomaly));
}

// file internal
// one sqrt; one division; i sin
double get_anomaly_at_time(OrbitSituation prev_data, double time) {
	float anomaly = prev_data.anomaly + pi_halfs;
	double mul_ = sqrt(prev_data.parent.mu / (prev_data.data.semi_major_axis * prev_data.data.semi_major_axis * prev_data.data.semi_major_axis));
	time = fmod(time, pi_ * 2 / mul_);
	for (int i = 0; i < 30; i++) {
		anomaly = time * mul_ + prev_data.data.eccentricity * sin(anomaly);
	}
	return anomaly - pi_halfs;
}

OrbitSituation project_orbit(OrbitSituation prev_data, double time) {
	prev_data.anomaly = get_anomaly_at_time(prev_data, time);
	return prev_data;
}

LongVector get_position(OrbitSituation sit) {
	InternalEllipsisData ellipsis_data = calculate_ellipsis_data(sit.data);
	return sit.parent.position() + ellipsis_data.offset + ellipsis_data.rotation * LongVector(
		sin(2 * pi_ + sit.anomaly) * ellipsis_data.a,
		0,
		cos(2 * pi_ + sit.anomaly) * ellipsis_data.b
	);
}

// Derivative of get_position; literally.
// Gotta be a better way
LongVector get_velocity(OrbitSituation sit) {
	const double delta = 0.001f;
	return (get_position(sit) - get_position(project_orbit(sit, delta))) / delta;
}

// Not finished
// Might outsource to wren
double calculate_transfer_delta_v (OrbitSituation vessel, OrbitSituation target, double travel_time) {
	/*
	The ammount of delta V to get from one orbit to another with a homan transfer
	*/
	const int angle_steps = 20;
	const int time_steps = 30;
	//const int angle_steps = 1;
	//const int time_steps = 1;

	const LongVector up = LongVector(0.0, 0.0, 1.0);
	const LongVector fore = LongVector(1.0, 0.0, 0.0);

	double* possible_routes = new double[angle_steps * time_steps];
	OrbitSituation* possible_situations = new OrbitSituation[angle_steps * time_steps];
	int routes_index = 0;
	
	// skip forward travel_time

	OrbitSituation future_target = project_orbit(target, travel_time);

	double
		end_angle = future_target.anomaly,
		end_radius = get_radius(future_target);
		
	double
		p2x = end_radius * cos(end_angle),
		p2y = end_radius * sin(end_angle);


	for (double i = 0; i < time_steps; i++) {
		// skip forward travel_time / time_steps
		double departure_time = lerp(0, travel_time, i / time_steps);

		OrbitSituation departure_vessel = project_orbit(vessel, departure_time);
		double
			departure_angle = departure_vessel.anomaly,
			departure_radius = get_radius(departure_vessel);

		double
			p1x = departure_radius * cos(departure_angle),
			p1y = departure_radius * sin(departure_angle);
		
		// cartesian coordinates

		double dx = p2x - p1x, dy = p2y - p1y;
		double l = sqrt(dx*dx + dy*dy);

		glm::mat3x3 rot = glm::mat3x3(
			-dx / l, dy / l, 0,
			-dy / l, -dx / l, 0,
			0, 0, 1
		);
		glm::mat3x3 trans = glm::mat3x3(
			1, 0, 0,
			0, 1, 0,
			-(p1x + p2x) / 2, -(p1y + p2y) / 2, 1
		);

		glm::mat3x3 planet2focalsolver_transform = rot * trans;
		glm::mat3x3 focalsolver2plane_transform = glm::inverse(planet2focalsolver_transform);

		//std::cout << l << std::endl;
		glm::vec3 p1 = glm::vec3(p1x, p1y, 1);
		glm::vec3 p2 = glm::vec3(p2x, p2y, 1);
		//print_vector(planet2focalsolver_transform * p1); // l; 0; 1
		//print_vector(planet2focalsolver_transform * p2); // -l; 0; 1

		double fc = l / 2;
		double fa = 0.5 * (end_radius - departure_radius);

		// focus
		for (int i = 0; i < angle_steps; i++) {
			double fy_local = 0.0; // correct interpolation
			double fx_local = -fa * sqrt(1 + fy_local * fy_local / (fc * fc - fa * fa));

			//print_vector(glm::vec3(fx_local, fy_local, 1));
			glm::vec3 f = focalsolver2plane_transform * glm::vec3(fx_local, fy_local, 1);
			float c = glm::length(f) * 0.5;
			float a = 0.5 * (departure_radius + glm::distance(p1, f));
			//print_vector(f);

			OrbitData data = {
				c / a,
				a,
				vessel.data.inclination,
				vessel.data.longitude_of_ascending_node,
				0// radial(f), // +pi?
			};

			OrbitSituation transfer_orbit = {		// time of contact
				data,
				departure_angle,
				vessel.parent,
				calculate_ellipsis_data(data)
			};

			print_vector(get_velocity(transfer_orbit));

			double dv_required = longvec_magnitude(get_velocity(transfer_orbit) - get_velocity(departure_vessel));

			possible_situations[routes_index] = transfer_orbit;
			possible_routes[routes_index++] = dv_required;
		}
	}

	double best = INFINITY;
	OrbitSituation best_situation;
	for (int i = 0; i < angle_steps * time_steps; i++) {
		if (possible_routes[i] < best) {
			best = possible_routes[i];
			best_situation = possible_situations[i];
		}
	}

	delete[] possible_routes;
	delete[] possible_situations;
	return best;
}


double calculate_transfer_delta_v_legacy (OrbitSituation vessel, OrbitSituation target, double travel_time) {
	/*
	The ammount of delta V to get from one orbit to another with a homan transfer
	*/
	const int angle_steps = 20;
	const int time_steps = 30;

	const LongVector up = LongVector(0.0, 0.0, 1.0);
	const LongVector fore = LongVector(1.0, 0.0, 0.0);

	double* possible_routes = new double[angle_steps * time_steps];
	OrbitSituation* possible_situations = new OrbitSituation[angle_steps * time_steps];
	int routes_index = 0;
	
	// skip forward travel_time

	OrbitSituation future_target = project_orbit(target, travel_time);

	double
		end_angle = future_target.anomaly,
		end_radius = get_radius(future_target);

	for (double i = 0; i < time_steps; i++) {
		// skip forward travel_time / time_steps
		double departure_time = lerp(0, travel_time, i / time_steps);

		OrbitSituation departure_vessel = project_orbit(vessel, departure_time);

		// Calculates inclination and aoan
		//LongVector 
		//	relative_p1 = get_position(departure_vessel) - future_target.parent.position(),
		//	relative_p2 = get_position(future_target) - future_target.parent.position();

		//LongVector plane_normal = longvec_cross(relative_p1, relative_p2);
		//double inclination = longvec_angle(up, plane_normal);
		//double argument = longvec_angle(fore, project2plane(plane_normal, up));

		// polar coordinates
		double
			start_angle = departure_vessel.anomaly,
			start_radius = get_radius(departure_vessel);

		for (double j = 0; j < angle_steps; j++) {
			// test for different angles
			double theta_plus = 2.0 * pi_ * j / angle_steps;
			double rel_start_angle = start_angle + theta_plus;
			double rel_end_angle = end_angle + theta_plus;

			// math 
			// don't touch

			// we know the rotation
			// find the eccentricity and a
			double ecc = (1.0 - start_radius / end_radius) / (start_radius / end_radius * cos(end_angle) - cos(start_angle));
			double a = start_radius * (1 + ecc * cos(rel_start_angle)) / (1 - ecc * ecc);

			OrbitData data = {
				ecc,
				a,
				vessel.data.inclination,
				vessel.data.longitude_of_ascending_node,
				theta_plus, // +pi?
			};

			OrbitSituation transfer_orbit = {		// time of contact
				data,
				rel_start_angle,
				vessel.parent,
				calculate_ellipsis_data(data)
			};

			double dv_required = longvec_magnitude(get_velocity(transfer_orbit) - get_velocity(departure_vessel));

			possible_situations[routes_index] = transfer_orbit;
			possible_routes[routes_index++] = dv_required;
		}
	}

	double best = INFINITY;
	OrbitSituation best_situation;
	for (int i = 0; i < angle_steps * time_steps; i++) {
		if (possible_routes[i] < best) {
			best = possible_routes[i];
			best_situation = possible_situations[i];
		}
	}

	delete[] possible_routes;
	delete[] possible_situations;
	return best;
}

// TBD
OrbitData get_orbitdata_from_points(LongVector center, LongVector point1, LongVector point2, PolarCoordinates p1_polar, PolarCoordinates p2_polar) {
	/*
	Return the orbital data from the center and 
	*/
	return OrbitData{
		0,0,0,0,0
	};
}

// TBD
//double calculate_minimum_delta_v(OrbitData parking1, const Orbiter* parent1, OrbitData parking2, const Orbiter* parent2, double current_time, double travel_time) {
//	return 0;
//}
