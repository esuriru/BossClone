/**
 SoundController
 @brief A class which manages the sound objects
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "Singleton.h"

// Include GLEW
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Include string
#include <string>
// Include map storage
#include <map>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"

class SoundController : public Utility::Singleton<SoundController>
{
	friend Utility::Singleton<SoundController>;
public:
	// Initialise this class instance
	bool Init(void);

	// Load a sound
	bool LoadSound(	string filename, 
					const int ID,
					const bool bPreload = true,
					const bool bIsLooped = false,
					SoundInfo::SOUNDTYPE eSoundType = SoundInfo::SOUNDTYPE::_2D,
					vec3df vec3dfSoundPos = vec3df(0.0f, 0.0f, 0.0f));

	bool LoadSound(	SoundInfo*& sound, string filename, 
					const int ID,
					const bool bPreload = true,
					const bool bIsLooped = false,
					SoundInfo::SOUNDTYPE eSoundType = SoundInfo::SOUNDTYPE::_2D,
					vec3df vec3dfSoundPos = vec3df(0.0f, 0.0f, 0.0f));

	// Play a sound by its ID
	irrklang::ISound* PlaySoundByID(const int ID, bool forcePlay = false);

	// Increase Master volume
	bool MasterVolumeIncrease(void);
	// Decrease Master volume
	bool MasterVolumeDecrease(void);

    void SetMasterVolume(float volume);
    float GetVolume();

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

	// For 3D sounds only
	// Set Listener position
	void SetListenerPosition(const float x, const float y, const float z);
	// Set Listener direction
	void SetListenerDirection(const float x, const float y, const float z);

	SoundInfo* GetSound(const int ID);

protected:
	// Constructor
	SoundController(void);

	// Destructor
	virtual ~SoundController(void);

	// Get an sound from this map
	// Remove an sound from this map
	bool RemoveSound(const int ID);
	// Get the number of sounds in this map
	int GetNumOfSounds(void) const;

	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	// The map of all the entity created
	std::map<int, SoundInfo*> soundMap;

	// For 3D sound only: Listener position
	vec3df vec3dfListenerPos;
	// For 3D sound only: Listender view direction
	vec3df vec3dfListenerDir;
};
