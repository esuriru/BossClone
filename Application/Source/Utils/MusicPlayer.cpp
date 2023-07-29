/**
 MusicPlayer
 @brief A class which plays background music like a jukebox
 By: Toh Da Jun
 Date: May 2023
 */
#include "MusicPlayer.h"
#include <glm/glm.hpp>

#include <algorithm>    // std::shuffle

// For generating random numbers
#include <cstdlib>
#include <time.h>

#include "Core/Core.h"

// Include filesystem
#include "filesystem.h"
#include "Audio/Transition.h"

#include <iostream>
using namespace std;

// A class to pass to irrklang to receive isound stop events
class CSoundStopReceiver : public irrklang::ISoundStopEventReceiver
{
public:
	virtual void OnSoundStopped(irrklang::ISound* sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData)
	{
		// This is called when the sound has ended playing
		cout << "Sound has ended playback" << endl;

		MusicPlayer::Instance()->SetMusicPlaybackFinished(true);

		switch (MusicPlayer::Instance()->GetPlayMode())
		{
		case MusicPlayer::PLAYMODE::SINGLE:
			MusicPlayer::Instance()->SetStatus(MusicPlayer::STATUS::STOP);
			break;
		// Can customise how the music player will behave for these modes after completing a music playback.
		// case MusicPlayer::PLAYMODE::CONTINUOUS:
		// case MusicPlayer::PLAYMODE::SHUFFLE:
		// 	break;
		// case MusicPlayer::PLAYMODE::SINGLE_LOOP:
		// case MusicPlayer::PLAYMODE::CONTINUOUS_LOOP:
		// case MusicPlayer::PLAYMODE::SHUFFLE_LOOP:
		// 	break;
		default:
			break;
		}

		// Try to play another music
		if (MusicPlayer::Instance()->GetStatus() == MusicPlayer::STATUS::PLAY)
			MusicPlayer::Instance()->PlayMusic();
	}
};

CSoundStopReceiver* cSoundStopReceiver = NULL;

/**
 @brief Constructor
 */
MusicPlayer::MusicPlayer(void)
	: cSoundEngine(NULL)
	, bMusicPlaybackFinished(true)
	, ePlayMode(PLAYMODE::SINGLE)
	, eStatus(STATUS::STOP)
	, currentISound(NULL)
	, iCurrentMusicVector(0)
{
	cSoundStopReceiver = new CSoundStopReceiver();

	// Reset the instance
	Reset();
}

/**
 @brief Destructor
 */
int MusicPlayer::GetID(string name)
{
    auto it = musicMap_.find(name);
    if (it != musicMap_.end())
        return it->second;
    return -1;
}

MusicPlayer::~MusicPlayer(void)
{
	// Clear the musicVector without deleting, since it will be deleted in musicMap
	musicVector.clear();

	// Iterate through the musicMap
	for (std::map<int, SoundInfo*>::iterator it = musicMap.begin(); it != musicMap.end(); ++it)
	{
		// If the value/second was not deleted elsewhere, then delete it here
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	// Drop the MySoundEndReceiver instance
	if (cSoundStopReceiver != NULL)
	{
		delete cSoundStopReceiver;
		cSoundStopReceiver = NULL;
	}

	// Remove all elements in musicMap
	musicMap.clear();

	// Drop the sound engine
	if (cSoundEngine)
	{
		cSoundEngine->drop();
		cSoundEngine = NULL;
	}
}

void MusicPlayer::AddTransition(Scope<Transition> newTransition)
{
    transitions_.emplace_back(std::move(newTransition));
}

void MusicPlayer::UpdateTransition(float dt)
{
    for (int i = 0; i < static_cast<int>(transitions_.size()); ++i)
    {
        auto& t = transitions_[i];
        if (!t->IsDone())
        {
            t->Update(dt);
        }

        if (t->IsDone())
        {
            transitions_.erase(transitions_.begin() + i);
            --i;
        }
    }
}

void MusicPlayer::SetPause(bool pause)
{
    currentISound->setIsPaused(pause);
}

bool MusicPlayer::Init(void)
{
	// Initialise the sound engine with default parameters
	cSoundEngine = createIrrKlangDevice(/*ESOD_WIN_MM, ESEO_MULTI_THREADED*/);
	if (cSoundEngine == NULL)
	{
		cout << "Unable to initialise the IrrKlang sound engine" << endl;
		return false;
	}

	return true;
}

/**
 @brief Add a music file
 @param filename A string variable storing the name of the file to read from
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @param bPreload A const bool variable which indicates if this iSoundSource will be pre-loaded into memory now.
 @return A bool value. True if the sound was loaded, else false.
 */
bool MusicPlayer::AddMusic(	string filename,
								const int ID,
								const bool bPreload)
{
	if (FileSystem::DoesFileExists(filename) == false)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_AUTO_DETECT, 
																	bPreload);


	// Trivial Rejection : Invalid pointer provided
	if (pSoundSource == nullptr)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Force the sound source not to have any streaming
	pSoundSource->setForcedStreamingThreshold(-1);

	// Clean up first if there is an existing Entity with the same name
	RemoveMusic(ID);

	// Add the entity now
	SoundInfo* cSoundInfo = new SoundInfo();
	cSoundInfo->Init(ID, pSoundSource);

	// Set to musicMap
	musicMap[ID] = cSoundInfo;

	Reset();

	return true;
}

bool MusicPlayer::AddMusic(string filename, const int ID, SoundInfo*& source, const bool bPreload, const bool loop)
{
	if (FileSystem::DoesFileExists(filename) == false)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_AUTO_DETECT, 
																	bPreload);


	// Trivial Rejection : Invalid pointer provided
	if (pSoundSource == nullptr)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}


	// Force the sound source not to have any streaming
	pSoundSource->setForcedStreamingThreshold(-1);

	// Clean up first if there is an existing Entity with the same name
	RemoveMusic(ID);

	// Add the entity now
	SoundInfo* cSoundInfo = new SoundInfo();
	cSoundInfo->Init(ID, pSoundSource, loop);

    source = cSoundInfo; 

	// Set to musicMap
	musicMap[ID] = cSoundInfo;

	Reset();

	return true;
}

/**
 @brief Remove a music from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return true if the sound was successfully removed, else false
 */
bool MusicPlayer::RemoveMusic(const int ID)
{
	SoundInfo* pSoundInfo = GetMusic(ID);
	if (pSoundInfo != nullptr)
	{
		delete pSoundInfo;
		musicMap.erase(ID);
		return true;
	}
	return false;
}

/**
@brief Get the number of music in this map
@return The number of sounds currently stored in the Map
*/
int MusicPlayer::GetNumOfMusic(void) const
{
	return musicMap.size();
}

/**
 @brief Set the music playback status
 @param bMusicPlaybackFinished A const bool variable which indicates if the music playback has finished
 */
void MusicPlayer::SetMusicPlaybackFinished(const bool bMusicPlaybackFinished)
{
	this->bMusicPlaybackFinished = bMusicPlaybackFinished;
}

void MusicPlayer::SetIDToName(int ID, std::string name)
{
    musicMap_[name] = ID;
}

/**
 @brief Play a sound by its ID
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 */
SoundInfo* MusicPlayer::PlayMusicByID(const int ID, bool paused)
{
	// Set the play mode
	ePlayMode = PLAYMODE::SINGLE;

	SoundInfo* pSoundInfo = GetMusic(ID);
	if (!pSoundInfo)
	{
		cout << "Sound #" << ID << " is not playable." << endl;
		return nullptr;
	}
	else if (cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		cout << "Sound #" << ID << " is currently being played." << endl;
		return nullptr;
	}

	currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), pSoundInfo->GetLoopStatus(), paused, true, true);
    currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);

    return pSoundInfo;
}

/**
 @brief Play a music according to the current mode
 */
SoundInfo* MusicPlayer::PlayMusic()
{
	// Set the play mode
	SetStatus(MusicPlayer::STATUS::PLAY);

	// cout << "PlayMusic: Mode = " << ePlayMode << endl;

	// Playback the music according to the playback mode
	switch (ePlayMode)
	{
	case SINGLE:
	case SINGLE_LOOP:
		{
			// Get the next music to play
			SoundInfo* pSoundInfo = GetMusic();
			if (pSoundInfo == nullptr)
				return nullptr;
			else
			{
				if (ePlayMode == SINGLE)
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				}
				else
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), true, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				}
                return pSoundInfo;
			}
		}
		break;
	case CONTINUOUS:
	case CONTINUOUS_LOOP:
		{
			if ((currentISound) && (currentISound->isFinished() == true))
				NextMusic();

			// Get the next music to play
			SoundInfo* pSoundInfo = GetMusic();
			if (pSoundInfo == nullptr)
				return nullptr;
			else
			{
				currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
				currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				bMusicPlaybackFinished = false;
			}

            return pSoundInfo;
		}
		break;
	case SHUFFLE:
	case SHUFFLE_LOOP:
		{
			if ((currentISound) && (currentISound->isFinished() == true))
				NextMusic();

			// If iCurrentMusicVector != -1, then we repeat the playback
			if (iCurrentMusicVector != -1)
			{
				// Update the mapCurrent
				mapCurrent = musicMap.find(musicVector[iCurrentMusicVector]);

				// Get the next music to play
				SoundInfo* pSoundInfo = GetMusic();
				if (pSoundInfo == nullptr)
					return nullptr;
				else
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
					bMusicPlaybackFinished = false;
				}
                
                return pSoundInfo;
			}
		}
		break;
	default:
        return nullptr;
	}
}

SoundInfo *MusicPlayer::PlayMusic(Scope<Transition> transition)
{
    return nullptr;
}

/**
 @brief Stop the music player
 */ 
void MusicPlayer::StopPlayMusic(void)
{
	// If it is not playing any music, then return
	if (currentISound == NULL)
		return;

	// Update the status
	SetStatus(STOP);

	// If cSoundEngine is valid, then stop all sounds
	if (cSoundEngine)
		cSoundEngine->stopAllSounds();

	// Reset the key parameters
	currentISound = NULL;
}

/**
 @brief Reset the music player
 */
void MusicPlayer::Reset(void)
{
	StopPlayMusic();

	// Reset the musicMap iterators and musicvector variable whenever we load a new music into musicMap
	if (!musicMap.empty())
	{
		// Reset the key parameters
		currentISound = NULL;
		mapStart = musicMap.begin();
		mapEnd = --musicMap.end();
		mapCurrent = mapStart;
		// Reset to start of playlist
		iCurrentMusicVector = 0;
	}
}

void MusicPlayer::ForceEndTransitions()
{
    for (auto& t : transitions_)
    {
        t->ForceEnd();
    }
    transitions_.clear();
}

/**
 @brief Previous Music
 */ 
void MusicPlayer::PreviousMusic(void)
{

}

/**
 @brief Next Music
 */ 
void MusicPlayer::NextMusic(void)
{
	// If a music being played, then stop the playback
	if ((currentISound) && (currentISound->isFinished() == false))
	{
		// Stop the music
		StopPlayMusic();

		// Set back to play
		SetStatus(MusicPlayer::STATUS::PLAY);
	}

	if ((ePlayMode >= SINGLE) && (ePlayMode <= CONTINUOUS_LOOP))
	{
		// Go to next song in the playlist
		mapCurrent++;

		// if the currentISound has completed playback, then we get the next music
		if (mapCurrent == musicMap.end())
		{
			// Reset to start of musicMap
			mapCurrent = mapStart;
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}
	else if (ePlayMode == SHUFFLE)
	{
		// Go to the next music
		iCurrentMusicVector++;
		if (iCurrentMusicVector >= musicVector.size())
		{
			// Reset to 0 so it won't play any music in musicMap
			iCurrentMusicVector = -1;
			// Reset to end of playlist
			mapCurrent = musicMap.end();
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}
	else if (ePlayMode == SHUFFLE_LOOP)
	{
		// Go to the next music
		iCurrentMusicVector++;
		if (iCurrentMusicVector >= musicVector.size())
		{
			// Reset to 0 since we start playing the music in SHUFFLE/SHUFFLE_LOOP mode
			iCurrentMusicVector = 0;
			// Reset to start of playlist
			mapCurrent = mapStart;
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}

	// If current status is Play, then play the music
	if (GetStatus() == MusicPlayer::STATUS::PLAY)
	{
		PlayMusic();
	}
}

/**
 @brief Get the play mode
 */
MusicPlayer::PLAYMODE MusicPlayer::GetPlayMode(void)
{
	return ePlayMode;
}

/**
 @brief Set the play mode
 */
void MusicPlayer::SetPlayMode(PLAYMODE newPlayMode)
{
	// Set the new mode
	ePlayMode = newPlayMode;
	if (ePlayMode >= PLAYMODE::NUM_PLAYMODE)
		ePlayMode = PLAYMODE::SINGLE;

	switch (ePlayMode)
	{
	case SINGLE:
		cout << "ePlayMode = SINGLE" << endl;
		break;
	case SINGLE_LOOP:
		cout << "ePlayMode = SINGLE_LOOP" << endl;
		break;
	case CONTINUOUS:
		cout << "ePlayMode = CONTINUOUS" << endl;
		break;
	case CONTINUOUS_LOOP:
		cout << "ePlayMode = CONTINUOUS_LOOP" << endl;
		break;
	case SHUFFLE:
		cout << "ePlayMode = SHUFFLE" << endl;
		break;
	case SHUFFLE_LOOP:
		cout << "ePlayMode = SHUFFLE_LOOP" << endl;
		break;
	default:
		cout << "ePlayMode = " << ePlayMode << " is unknown." << endl;
	}

	// Reset the music player
	Reset();

	// Create the musicVector ofr SHUFFLE and SHUFFLE_LOOP playback mode
	if ((ePlayMode == SHUFFLE) || (ePlayMode == SHUFFLE_LOOP))
	{
		// Clear the musicVector before we repopulate it
		if (musicVector.size() > 0)
			musicVector.clear();

		// Reset iCurrentMusicVector to the start of musicVector
		iCurrentMusicVector = 0;

		// Iterate through the musicMap
		for (std::map<int, SoundInfo*>::iterator it = musicMap.begin(); it != musicMap.end(); ++it)
		{
			musicVector.push_back(it->first);
		}

		// Use current time as seed for random generator
		srand(time(NULL));
		// Use built-in random generator to shuffle the music
		std::random_shuffle(musicVector.begin(), musicVector.end());
	}
}

/**
 @brief Get the STATUS
 */
MusicPlayer::STATUS MusicPlayer::GetStatus(void)
{
	return eStatus;
}
/**
 @brief Set the STATUS
 */
void MusicPlayer::SetStatus(MusicPlayer::STATUS newStatus)
{
	eStatus = newStatus;
}

/**
 @brief Increase Master volume
 @return true if successfully increased volume, else false
 */
bool MusicPlayer::MasterVolumeIncrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() + 0.1f;
	// Check if the maximum volume has been reached
	if (fCurrentVolume > 1.0f)
		fCurrentVolume = 1.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeIncrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

/**
 @brief Decrease Master volume
 @return true if successfully decreased volume, else false
 */
bool MusicPlayer::MasterVolumeDecrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() - 0.1f;
	// Check if the minimum volume has been reached
	if (fCurrentVolume < 0.0f)
		fCurrentVolume = 0.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeDecrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

void MusicPlayer::SetMasterVolume(float volume)
{
    cSoundEngine->setSoundVolume(glm::clamp(volume, 0.f, 1.0f));
}

float MusicPlayer::GetVolume()
{
    return cSoundEngine->getSoundVolume();
}


/**
 @brief Increase volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool MusicPlayer::VolumeIncrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetMusic(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the maximum volume has been reached
	if (fCurrentVolume >= 1.0f)
	{
		pISoundSource->setDefaultVolume(1.0f);
		return false;
	}

	// Increase the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume + 0.1f);

	return true;
}

/**
 @brief Decrease volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool MusicPlayer::VolumeDecrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetMusic(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the minimum volume has been reached
	if (fCurrentVolume <= 0.0f)
	{
		pISoundSource->setDefaultVolume(0.0f);
		return false;
	}

	// Decrease the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume - 0.1f);

	return true;
}

/**
 @brief Get a music from this map
 @return A SoundInfo* variable
 */
SoundInfo* MusicPlayer::GetMusic(void)
{
	if (mapCurrent != musicMap.end())
	{
		cout << "Current Music : " << mapCurrent->first << endl;

		return (SoundInfo*)(mapCurrent->second);
	}

	return nullptr;
}

/**
 @brief Get a music from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return A SoundInfo* variable
 */
SoundInfo* MusicPlayer::GetMusic(const int ID)
{
	if (musicMap.count(ID) != 0)
		return musicMap[ID];

	return nullptr;
}
