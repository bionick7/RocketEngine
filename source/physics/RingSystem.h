#pragma once
#include "Orbiter.h"
#include "lineset.h"
#include "scene_manager.h"

//typedef unsigned short pixel_indx;

//struct ScreenPos { pixel_indx x; pixel_indx y; };

class RingSystem : public Orbiter
{
public:
	static const AgentType type = AgentType::RINGSYSTEM;

	RingSystem(io::DataStructurePtr, Orbiter*);

	void orbiter_step(double, agent_id) override;
	void draw_step(double) override;

	AgentType const get_type() override;
	bool const is_instance_of(AgentType) override;

protected:
	void on_orbit_changed();

private:
	// original orbital values
	double og_a, og_b;

	unsigned hatching_segments = 150;

	double width;
	Polygone* inner;
	Polygone* outer;
	LineSet* hatching;

	LongVector get_pt(double, OrbitSituation, bool);
	linearray_t get_hatching();

	DrawMode draw_mode = DrawMode::CloseUp;
};
