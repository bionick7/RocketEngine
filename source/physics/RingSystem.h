#pragma once
#include "Orbiter.h"
#include "lineset.h"
#include "Blackboard.h"

//typedef unsigned short pixel_indx;

//struct ScreenPos { pixel_indx x; pixel_indx y; };

class RingSystem : public Orbiter
{
public:
	static const Type type = Type::RINGSYSTEM;

	RingSystem(io::DataStructure*, Orbiter*);

	void orbiter_step(double, agent_id) override;
	void draw_step(double) override;

	Type get_type() override;

private:
	// original orbital values
	double og_a, og_b;

	unsigned hatching_segments = 150;

	double width;
	Polygone* outer;
	LineSet* hatching;

	LongVector get_pt(double, OrbitSituation, bool);
	linearray_t get_hatching();

	DrawMode draw_mode = DrawMode::CloseUp;
};
