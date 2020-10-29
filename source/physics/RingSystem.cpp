#include "RingSystem.h"

RingSystem::RingSystem(io::DataStructure* data, Orbiter* parent_ptx) : Orbiter::Orbiter(data, parent_ptx) {
	width = data->get_double("width");
	polygone->line_type = LineType::CLOSED;
	outer = new Polygone(LineType::CLOSED);
	og_a = ellipsis_data.a;
	og_b = ellipsis_data.b;

	ellipsis_data.a = og_a * (1 - width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 - width / (2 * orbit.semi_major_axis));

	ellipsis_data.a = og_a * (1 + width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 + width / (2 * orbit.semi_major_axis));

	hatching = new LineSet();
	hatching->update(get_hatching());

	add_shape(outer);
	add_shape(hatching);
}

void RingSystem::orbiter_step(double dt , agent_id from) {
	//Orbiter::orbiter_step(dt, from);

	current_anomaly = 0;
	center_position = parent.position() + ellipsis_data.offset;

	position = get_point(get_situation(), 0);

	// recursive nature
	// Assuming parent is valid
	if (parent.ptx->id != from) {
		parent.ptx->orbiter_step(dt, id);
	}
}

int clamp(int in, int a, int b) {
	return (in < a ? a : (in > b ? b : in));
}

void RingSystem::draw_step(double dt) {
	Agent::draw_step(dt);
	Camera* cam = &global_blackboard->main_camera->large_scale;
	float tan = orbit.semi_major_axis * position_scale / glm::distance(glm::vec3(cam->view_matrix[3]), ((center_position - global_blackboard->main_camera->get_focal_point()) * position_scale).to_float_vec());

	hatching_segments = clamp((int) (sqrt(tan) * 100.0f), 1, 150);

	ellipsis_data.a = og_a * (1 - width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 - width / (2 * orbit.semi_major_axis));
	polygone->set_point_array(get_points(true, 0.0));

	ellipsis_data.a = og_a * (1 + width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 + width / (2 * orbit.semi_major_axis));
	outer->set_point_array(get_points(true, 0.0));

	hatching->update(get_hatching());
}

LongVector RingSystem::get_pt(double x, OrbitSituation sit, bool solution) {
	return (sit.parent.position() + (sit.ellipse.rotation * sit.parent.reference) *
		(LongVector(x * sit.ellipse.a, 0, sqrt(1.0 - x*x) * (solution ? -1:1) * sit.ellipse.b)) - 
		global_blackboard->main_camera->get_focal_point()) * position_scale;
}

linearray_t RingSystem::get_hatching() {
	linearray_t res = linearray_t();
	ellipsis_data.a = og_a * (1 - width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 - width / (2 * orbit.semi_major_axis));
	OrbitSituation sit_inner = get_situation();

	ellipsis_data.a = og_a * (1 + width / (2 * orbit.semi_major_axis));
	ellipsis_data.b = og_b * (1 + width / (2 * orbit.semi_major_axis));
	OrbitSituation sit_outer = get_situation();

	//hatching_segments = 100;

	double inner_multiplier = sit_outer.ellipse.a / (sit_outer.ellipse.a - width);
	for (unsigned i = 0; i < hatching_segments; i++) {
		double x = (double) i / hatching_segments * 2.0 - 1.00;
		if (x < -1 + (width / sit_outer.ellipse.a) || x > 1 - (width / sit_outer.ellipse.a)) {
			// bounds
			res.push_back({
				get_pt(x, sit_outer, 0).to_float_vec(),
				get_pt(x, sit_outer, 1).to_float_vec()
			});
		}
		else {
			res.push_back({
				get_pt(x * inner_multiplier, sit_inner, 0).to_float_vec(),
				get_pt(x, sit_outer, 0).to_float_vec()
			});
			res.push_back({
				get_pt(x * inner_multiplier, sit_inner, 1).to_float_vec(),
				get_pt(x, sit_outer, 1).to_float_vec()
			});
		}
	}
	return res;
}

Type RingSystem::get_type() {
	return Type::RINGSYSTEM;
}
