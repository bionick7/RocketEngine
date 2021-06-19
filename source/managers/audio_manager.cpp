#include "audio_manager.h"
#include "asset_manager.h"

void check_al_error(std::string message) {
	ALCenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cout << "OpenAL Error(" << std::hex << int(error) << std::dec << ") on \"" << message << "\"" << std::endl;
	}
}

void DynamicSound::operator()() {
	al_name = generate();
}

ALuint DynamicSound::generate() {
	return 0u;
}

ALuint DynamicSound::get_al_name() {
	return al_name;
}

ModemNoises::ModemNoises(double p_play_time) {
	play_time = p_play_time;
}

float rand_float(float min, float max) {
	// Provisorical
	int r = rand();
	return min + fmod(float(r) / 10000.0f, max - min);
}

AudioPlayer::AudioPlayer() {
	alGenSources((ALuint)1, &al_name);
	alSourcef(al_name, AL_PITCH, 1);
	alSourcef(al_name, AL_GAIN, 1);
	alSource3f(al_name, AL_POSITION, 0, 0, 0);
	alSource3f(al_name, AL_VELOCITY, 0, 0, 0);
	alSourcei(al_name, AL_LOOPING, AL_FALSE);

	pitch = 1;
	gain = 1;
	position = LongVector(0.0, 0.0, 0.0);
	velocity = LongVector(0.0, 0.0, 0.0);
	looping = false;
}

AudioPlayer::~AudioPlayer() {
	alDeleteSources((ALuint)1, &al_name);
}

ALint AudioPlayer::get_status() {
	ALint source_state;
	alGetSourcei(al_name, AL_SOURCE_STATE, &source_state);
	check_al_error("Audioplayer - get status");
	return source_state;
}

void AudioPlayer::set_position(LongVector pos) { 
	alSource3f(al_name, AL_POSITION, pos.x, pos.y, pos.z); 
	position = pos;
}

void AudioPlayer::set_velocity(LongVector vel) { 
	alSource3f(al_name, AL_VELOCITY, vel.x, vel.y, vel.z);
	velocity = vel;
}

void AudioPlayer::set_pitch(double _pitch) {
	alSourcef(al_name, AL_PITCH, _pitch);
	pitch = _pitch;
}

void AudioPlayer::set_gain(double _gain) { 
	alSourcef(al_name, AL_GAIN, _gain);
	gain = _gain;
}

void AudioPlayer::set_loop(bool is_looping) {
	alSourcei(al_name, AL_LOOPING, is_looping);
	looping = is_looping;
}

ALuint AudioPlayer::get_al_name() {
	return al_name;
}

void AudioPlayer::play_dynamic(DynamicSound* sound) {
	// Connect buffer and source
	alSourcei(get_al_name(), AL_BUFFER, sound->get_al_name());
	check_al_error("Dynamic sound - Connect buffer");

	// Play
	alSourcePlay(get_al_name());
	check_al_error("Dynamic sound - Play");
}

void AudioPlayer::play_static(AudioResource sound_resource) {
	// Connect buffer and source
	alSourcei(get_al_name(), AL_BUFFER, sound_resource.ID);
	check_al_error("Static sound " + sound_resource.name + " - Connect buffer");

	// Play
	alSourcePlay(get_al_name());
	check_al_error("Static sound " + sound_resource.name + " - Play");
}

void AudioPlayer::stop() {
	alSourceStop(get_al_name());
}

void AudioPlayer::pause() {
	alSourcePause(get_al_name());
}


void AudioPlayer::resume() {
	alSourcePlay(get_al_name());
}

ALuint ModemNoises::generate() {
	ALuint buffer;
	alGenBuffers((ALuint)1, &buffer);
	check_al_error("ModemNoises - Create buffer");

	int sample_rate = 44100;
	int samples = int(sample_rate * play_time);

	float frequency = 440 * std::pow(2, rand_float(-3, 3));; // Hz

	char* data = new char[samples];
	
	for (int i = 0; i < samples - 2; i++) {
		float a = sin(frequency * 6.283 * (float)i / (float)sample_rate);
		a = a * 0.5 + 0.5;
		data[i] = (char)(a * 255);
		if (rand_float(0, 1) < 0.0001) {
			frequency = 440 * std::pow(2, rand_float(-3, 3));
		}
	}

	// Load data to buffer
	alBufferData(buffer, AL_FORMAT_MONO8, data, samples * sizeof(char), sample_rate);
	delete[] data;
	check_al_error("ModemNoises - Fill buffer");
	return buffer;
}

AudioManager::AudioManager() {
	alGetError();
	device = alcOpenDevice(nullptr);
	if (!device) {
		std::cout << "Device couldn't open" << std::endl;
	}
	check_al_error("AudioManager Setup - initiate device");

	ALboolean enumeration;

	enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE) {
		std::cout << "AL Enumeration not supported" << std::endl;
		return;
	}
	check_al_error("AudioManager Setup - check enumeration");

	// Create context
	context = alcCreateContext(device, nullptr);
	check_al_error("AudioManager Setup - create context");
	if (!alcMakeContextCurrent(context)) {
		std::cout << "Can't create context" << std::endl;
		return;
	}
	check_al_error("AudioManager Setup - make context");
}

AudioManager::~AudioManager() {
	device = alcGetContextsDevice(context);
	alcDestroyContext(context);
	check_al_error("Destructer - deleting context");
	alcMakeContextCurrent(nullptr);
	check_al_error("Destructer - resetting context");
	alcCloseDevice(device);
	check_al_error("Destructer - deleting device");

	for (AudioPlayer* player : player_pool) {
		delete player;
	}
}

void AudioManager::setup() {
	return;
	Manager::setup();

	// Listener
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	// Test
	test();
}

void AudioManager::step(double dt) {
	return;
	Manager::step(dt);
	for (AudioPlayer* player : player_pool) {
		if (player->occupied) {
			//std::cout << (player->get_status() == AL_PLAYING) << std::endl;
			if (player->get_status() != AL_PLAYING) {
				player->occupied = false;
			}
		}
	}
}

void AudioManager::test() {
	// Audio buffer
	auto sound = ModemNoises(2);
	sound();  // generate
	ALuint buffer = sound.get_al_name();

	auto resource = assets->get<AudioResource>(io::ResourceType::AUDIO, "KeyStroke", true);
	get_free_player()->play_static(*resource);
	//play_sound_dynamic(get_free_player(), &sound);
}

AudioPlayer* AudioManager::get_free_player() {
	for (AudioPlayer* player : player_pool) {
		if (!player->occupied) {
			return player;
		}
	}
	AudioPlayer* new_player = new AudioPlayer();
	new_player->occupied = true;
	player_pool.push_back(new_player);
	return new_player;
}
