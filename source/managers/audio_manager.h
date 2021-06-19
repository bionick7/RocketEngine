#pragma once
#include "asset_manager.h"
#include "manager.h"

void check_al_error(std::string message);

class DynamicSound abstract {
public:
	void operator()();
	double play_time = 0.0;

	ALuint get_al_name();

protected:
	virtual ALuint generate();
	//void check(std::string message);
private:
	ALuint al_name;
};

class ModemNoises : public DynamicSound {
public:
	ModemNoises(double p_play_time);
	ALuint generate() override;
};

class AudioPlayer {
public:
	AudioPlayer();
	~AudioPlayer();
	ALint get_status();

	void set_position(LongVector pos);
	void set_velocity(LongVector vel);
	void set_pitch(double _pitch);
	void set_gain(double _gain);
	void set_loop(bool is_looping);

	void play_dynamic(DynamicSound*);
	void play_static(AudioResource);

	void stop();
	void pause();
	void resume();

	ALuint get_al_name();
	bool occupied = false;

private:
	ALuint al_name;
	LongVector position;
	LongVector velocity;
	double pitch;
	double gain;
	bool looping;
};

class AudioManager:
	public Manager
{
public:
	AudioManager();
	~AudioManager();

	void setup();
	void step(double dt) override;

	void test();

	AudioPlayer* get_free_player();

private:
	ALCdevice* device = nullptr;
	ALCcontext* context = nullptr;

	std::vector<AudioPlayer*> player_pool = std::vector<AudioPlayer*>();
};

extern AudioManager* audio_manager;