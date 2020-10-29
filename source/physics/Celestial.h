#pragma once
#include "circle.h"
#include "unscaling_billboard.h"
#include "CameraContainer.h"
#include "RingSystem.h"
#include "Orbiter.h"

//typedef unsigned short pixel_indx;

//struct ScreenPos { pixel_indx x; pixel_indx y; };

class Celestial : public Orbiter
{
public:
	static const Type type = Type::CELESTIAL;

	Celestial(io::DataStructure*, Orbiter*);

	double Mu();
	void draw_step(double) override;
	void orbiter_step(double, agent_id) override;

	glm::mat4 get_focus_transformation() override;

	double radius;
	double mass;
	double tilt;
	double rotational_speed;

	Circle* circle;
	Polygone* equator;
	Polygone* polar_rings;
	Billboard* billboard;

	unsigned short sattelites_num;
	unsigned add_sattelite(Orbiter* sattelite);
	Orbiter* const get_sattelite(unsigned position);

	Type get_type() override;

private:

	std::vector<Orbiter*> sattelites;
	void update_drawmode(Camera);
	DrawMode draw_mode = DrawMode::CloseUp;

	short radius_meta;
};
