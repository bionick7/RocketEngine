#ifndef ORBITER_H
#define ORBITER_H
#include "../graphics/Camera.h"
#include "../graphics/polygone.h"


struct CelestialEssentials {
	LongVector position;
	double mu;
	bool is_valid;
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

class Orbiter abstract
{
friend class Celestial;
public:
	Orbiter(cfg::DataStructure*);
	~Orbiter();

	void virtual draw(Camera*);

	CelestialEssentials parent;
	OrbitData orbit;

	Polygone* polygone;

	void virtual update(double);

	std::string name = "";

	glm::vec3 meta_position;
	double current_anomaly;
	LongVector position;

	OrbitSituation get_situation();

protected:
	LongVector center_position;

	const double position_scale = 5e-10;
	const double time_scale = 2.3e7;

private:
	const unsigned points_number = 100;
	InternalEllipsisData ellipsis_data;
	point_array_t get_points(bool);
};

extern std::vector<Orbiter*> all_orbiters;

// double get_anomaly(OrbitSituation, double);
LongVector get_point(OrbitSituation, double);
double get_radius(OrbitSituation);
LongVector get_position(OrbitSituation);
LongVector get_velocity(OrbitSituation);
InternalEllipsisData calculate_ellipsis_data(OrbitData);

//double calculate_minimum_delta_v(OrbitSituation, OrbitSituation, double);

#endif // ORBITER_H