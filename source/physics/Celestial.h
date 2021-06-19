#pragma once
#include "circle.h"
#include "unscaling_billboard.h"
#include "CameraContainer.h"
#include "RingSystem.h"
#include "Orbiter.h"
#include "space_ship.h"
#include "mesh.h"

//typedef unsigned short pixel_indx;

//struct ScreenPos { pixel_indx x; pixel_indx y; };

class Celestial : 
	public Orbiter
{
public:
	static const AgentType type = AgentType::CELESTIAL;

	Celestial();
	Celestial(io::DataStructurePtr, Orbiter*);
	void setup(io::DataStructurePtr, Orbiter*) override;

	double Mu();
	void draw_step(double) override;
	void orbiter_step(double, agent_id) override;

	LongMatrix4x4 get_focus_transformation() override;

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

	AgentType const get_type() override;
	bool const is_instance_of(AgentType) override;

private:

	std::vector<Orbiter*> sattelites;
	void update_drawmode(Camera*);
	DrawMode draw_mode = DrawMode::CloseUp;
	LongMatrix4x4 surface_transform;

	short radius_meta;
};
