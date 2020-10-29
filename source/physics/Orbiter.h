#ifndef ORBITER_H
#define ORBITER_H
#include "Camera.h"
#include "polygone.h"
#include "CameraContainer.h"

extern double time_scale;

class Orbiter;

struct CelestialEssentials {
	double mu;
	bool is_valid;
	Orbiter* ptx;
	LongVector position();
	LongQuaternion reference;
};

struct OrbitData {
	double eccentricity{ 0 };
	double semi_major_axis{ 0 };
	double inclination{ 0 };
	double longitude_of_ascending_note{ 0 };
	double argument_of_periapsis{ 0 };
};

struct InternalEllipsisData {
	double a;
	double b;
	double c;
	LongVector offset;
	LongQuaternion rotation;
};

struct OrbitSituation {
	OrbitData data;
	double anomaly;
	CelestialEssentials parent;
	InternalEllipsisData ellipse;
};

class Orbiter abstract : public Agent
{
friend class Celestial;
public:
	Orbiter(io::DataStructure*, Orbiter*);
	~Orbiter();

	CelestialEssentials parent;
	OrbitData orbit;

	Polygone* polygone;

	void virtual physics_step(double) override;
	void virtual orbiter_step(double, agent_id);
	void virtual draw_step(double) override;

	glm::vec3 meta_position = glm::vec3();
	double current_anomaly = 0.0;

	OrbitSituation get_situation();

protected:
	LongVector center_position = LongVector(0.0, 0.0, 0.0);
	InternalEllipsisData ellipsis_data;
	const unsigned points_number = 100;
	point_array_t get_points(bool, double);
};

// needed?
extern std::vector<Orbiter*> all_orbiters;

// double get_anomaly(OrbitSituation, double);
LongVector get_point(OrbitSituation, double);
double get_radius(OrbitSituation);
LongVector get_position(OrbitSituation);
LongVector get_velocity(OrbitSituation);
InternalEllipsisData calculate_ellipsis_data(OrbitData);

//double calculate_minimum_delta_v(OrbitSituation, OrbitSituation, double);

#endif // ORBITER_H