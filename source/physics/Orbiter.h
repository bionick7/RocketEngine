#pragma once

#include "Camera.h"
#include "polygone.h"
#include "ellipsis.h"
#include "scene_manager.h"
#include "CameraContainer.h"

class Orbiter;

struct CelestialEssentials {
	double mu = 0;
	bool is_valid = false;
	Orbiter* ptx = nullptr;
	LongVector position();
	LongQuaternion reference = LongQuaternion();
};

struct OrbitData {
	double eccentricity = 0.0;
	double semi_major_axis = 0.0;
	double inclination = 0.0;
	double longitude_of_ascending_node = 0.0;
	double argument_of_periapsis = 0.0;
};

struct InternalEllipsisData {
	double a = 0.0;
	double b = 0.0;
	double c = 0.0;
	LongVector offset = LongVector();
	LongQuaternion rotation = LongQuaternion();
};

struct OrbitSituation {
	OrbitData data;
	double anomaly = 0.0;
	CelestialEssentials parent;
	InternalEllipsisData ellipse;
};

class Orbiter abstract : 
	public Agent
{
friend class Celestial;
public:
	Orbiter();
	Orbiter(io::DataStructurePtr, Orbiter*);
	~Orbiter();

	void virtual setup(io::DataStructurePtr, Orbiter*);

	CelestialEssentials parent;
	OrbitData orbit;

	Ellipsis* ellipsis;

	void virtual physics_step(double) override;
	void virtual orbiter_step(double, agent_id);
	void virtual draw_step(double) override;
	AgentType const virtual get_type() override;
	bool const virtual is_instance_of(AgentType) override;

	glm::vec3 meta_position = glm::vec3();
	double current_anomaly = 0.0;
	bool is_setup = false;

	OrbitSituation get_situation();

protected:
	LongVector center_position = LongVector(0.0, 0.0, 0.0);
	InternalEllipsisData ellipsis_data;
	const unsigned points_number = 100;
	point_array_t get_points(bool, double);
	void on_orbit_changed();

	OrbitSituation initial_situation;
	LongMatrix4x4 get_orbit_transform();
	LongMatrix4x4 rotation_matrix;
};

// double get_anomaly(OrbitSituation, double);
LongVector get_point(OrbitSituation, double);
double get_radius(OrbitSituation);
LongVector get_position(OrbitSituation);
LongVector get_velocity(OrbitSituation);
InternalEllipsisData calculate_ellipsis_data(OrbitData);

double calculate_transfer_delta_v(OrbitSituation, OrbitSituation, double);
