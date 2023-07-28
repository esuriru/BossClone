/**
 MusicPlayer
 @brief A class which plays background music like a jukebox
 By: Toh Da Jun
 Date: May 2023
 */
#pragma once

// Include SingletonTemplate
#include "Singleton.h"

// Include irrklang
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "Core/Core.h"

// Include string
#include <string>
// Include map storage
#include <map>
// Include vector storage
#include <vector>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"

class Transition;

class MusicPlayer : public Utility::Singleton<MusicPlayer>
{
	friend Utility::Singleton<MusicPlayer>;
public:
	// The Play Mode
	enum PLAYMODE
	{
		SINGLE = 0,
		SINGLE_LOOP,
		CONTINUOUS,
		CONTINUOUS_LOOP,
		SHUFFLE,
		SHUFFLE_LOOP,
		NUM_PLAYMODE
	};

	// The Status
	enum STATUS
	{
		STOP = 0,
		PLAY,
		PAUSE,
		NUM_STATUS
	};

    void AddTransition(Scope<Transition> newTransition);

    void UpdateTransition(float dt);

	// Initialise this class instance
	bool Init(void);

	// Add a music file
	bool AddMusic(	string filename, 
					const int ID,
					const bool bPreload = true);

	bool AddMusic(	string filename, 
					const int ID,
                    SoundInfo*& source, 
					const bool bPreload = true,
                    const bool loop = false);
	// Remove a music files  from this map
	bool RemoveMusic(const int ID);
	// Get the number of music files in this map
	int GetNumOfMusic(void) const;

	// Set the music playback status
	void SetMusicPlaybackFinished(const bool bMusicPlaybackFinished);

	// Play a music by its ID
	SoundInfo* PlayMusicByID(const int ID);
	// Play a music according to the current mode
	SoundInfo* PlayMusic(void);
    SoundInfo* PlayMusic(Scope<Transition> transition);
	// Stop the music player
	void StopPlayMusic(void);
	// Reset the music player
	void Reset(void);
    void ForceEndTransitions();

    inline ISound* GetCurrentSound()
    {
        return currentISound;
    }

	// Previous Music
	void PreviousMusic(void);
	// Next Music
	void NextMusic(void);

	// Get the play mode
	MusicPlayer::PLAYMODE GetPlayMode(void);
	// Set the play mode
	void SetPlayMode(MusicPlayer::PLAYMODE newPlayMode);

	// Get the STATUS
	MusicPlayer::STATUS GetStatus(void);
	// Set the STATUS
	void SetStatus(MusicPlayer::STATUS newStatus);

	// Increase Master volume
	bool MasterVolumeIncrease(void);
	// Decrease Master volume
	bool MasterVolumeDecrease(void);

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

protected:
	// Constructor
	MusicPlayer(void);

    std::vector<Scope<Transition>> transitions_;

	// Destructor
	virtual ~MusicPlayer(void);

	// Get a music files  from this map
	SoundInfo* GetMusic(void);
	// Get a music files  from this map
	SoundInfo* GetMusic(const int ID);

	// The current mode of music playback
	PLAYMODE ePlayMode;

	// The current status of music playback
	STATUS eStatus;

	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	// bool flag to indicated if the current music has finished playing
	bool bMusicPlaybackFinished;

	// The map of all the musicMap created
	std::map<int, SoundInfo*> musicMap;
	// iterators for navigating through musicMap
	std::map<int, SoundInfo*>::iterator mapStart, mapEnd, mapCurrent;
	// Pointer to current irrklang iSound being played
	irrklang::ISound* currentISound;

	// The vector of all shuffled music created
	std::vector<int> musicVector;
	// Variable storing index of music in musicVector being played
	int iCurrentMusicVector;
};
