#pragma once

class Celestial;
class CameraContainer;

constexpr uint64_t CONSOLE_FLAG_UPDATE = 0x01;
constexpr uint64_t CONSOLE_FLAG_CLEAR = 0x02;

class Blackboard {
public:
	uint64_t console_flags;
	CameraContainer* main_camera;
	Celestial* hierarchy_entrypoint;
	double time; // seconds
};

extern Blackboard* global_blackboard;