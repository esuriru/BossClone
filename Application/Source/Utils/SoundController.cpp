/**
 SoundController
 @brief A class which manages the sound objects
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "SoundController.h"

#include <iostream>
#include <glm/glm.hpp>
using namespace std;

/**
 @brief Constructor
 */
SoundController::SoundController(void)
	: cSoundEngine(NULL)
	, vec3dfListenerPos(vec3df(0, 0, 0))
	, vec3dfListenerDir(vec3df(0, 0, 1))
{
}

/**
 @brief Destructor
 */
SoundController::~SoundController(void)
{
	// Iterate through the entityMap
	for (std::map<int, SoundInfo*>::iterator it = soundMap.begin(); it != soundMap.end(); ++it)
	{
		// If the value/second was not deleted elsewhere, then delete it here
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	// Remove all elements in entityMap
	soundMap.clear();

	// Since we have already dropped the ISoundSource, then we don't need to delete the cSoundEngine
	//// Clear the sound engine
	//if (cSoundEngine)
	//{
	//	cSoundEngine->drop();
	//	cSoundEngine = NULL;
	//}
}

/**
 @brief Initialise this class instance
 @return A bool value. true is this class instance was initialised, else false
 */
bool SoundController::Init(void)
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
 @brief Load a sound
 @param filename A string variable storing the name of the file to read from
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @param bPreload A const bool variable which indicates if this iSoundSource will be pre-loaded into memory now.
 @param bIsLooped A const bool variable which indicates if this iSoundSource will have loop playback.
 @param eSoundType A SOUNDTYPE enum variable which states the type of sound
 @param vec3dfSoundPos A vec3df variable which contains the 3D position of the sound
 @return A bool value. True if the sound was loaded, else false.
 */
bool SoundController::LoadSound(	string filename,
									const int ID,
									const bool bPreload,
									const bool bIsLooped,
									SoundInfo::SOUNDTYPE eSoundType,
									vec3df vec3dfSoundPos)
{
	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_NO_STREAMING, 
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
	RemoveSound(ID);

	// Add the entity now
	SoundInfo* cSoundInfo = new SoundInfo();
	if (eSoundType == SoundInfo::SOUNDTYPE::_2D)
		cSoundInfo->Init(ID, pSoundSource, bIsLooped);
	else
		cSoundInfo->Init(ID, pSoundSource, bIsLooped, eSoundType, vec3dfSoundPos);

	// Set to soundMap
	soundMap[ID] = cSoundInfo;

	return true;
}

bool SoundController::LoadSound(	SoundInfo*& sound, string filename,
									const int ID,
									const bool bPreload,
									const bool bIsLooped,
									SoundInfo::SOUNDTYPE eSoundType,
									vec3df vec3dfSoundPos)
{
	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_NO_STREAMING, 
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
	RemoveSound(ID);

	// Add the entity now
	SoundInfo* cSoundInfo = new SoundInfo();
	if (eSoundType == SoundInfo::SOUNDTYPE::_2D)
		cSoundInfo->Init(ID, pSoundSource, bIsLooped);
	else
		cSoundInfo->Init(ID, pSoundSource, bIsLooped, eSoundType, vec3dfSoundPos);

    sound = cSoundInfo;

	// Set to soundMap
	soundMap[ID] = cSoundInfo;

	return true;
}

/**
 @brief Play a sound by its ID
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 */
irrklang::ISound* SoundController::PlaySoundByID(const int ID, bool forcePlay)
{
	SoundInfo* pSoundInfo = GetSound(ID);
	if (!pSoundInfo)
	{
		cout << "Sound #" << ID << " is not playable." << endl;
		return nullptr;
	}

	else if (!forcePlay && cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		cout << "Sound #" << ID << " is currently being played." << endl;
		return nullptr;
	}

	if (pSoundInfo->GetSoundType() == SoundInfo::SOUNDTYPE::_2D)
	{
		return cSoundEngine->play2D(	pSoundInfo->GetSound(), 
								pSoundInfo->GetLoopStatus(), false, false, true);
	}
	else if (pSoundInfo->GetSoundType() == SoundInfo::SOUNDTYPE::_3D)
	{
		cSoundEngine->setListenerPosition(vec3dfListenerPos, vec3dfListenerDir);
		return cSoundEngine->play3D(	pSoundInfo->GetSound(), 
								pSoundInfo->GetPosition(), 
								pSoundInfo->GetLoopStatus(), false, false, true);
	}

    return nullptr;
}

/**
 @brief Increase Master volume
 @return true if successfully increased volume, else false
 */
bool SoundController::MasterVolumeIncrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() + 0.1f;
	// Check if the maximum volume has been reached
	if (fCurrentVolume > 1.0f)
		fCurrentVolume = 1.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeIncrease: fCurrentVolume = " << fCurrentVolume << endl;

    // cSoundEngine->get

	return true;
}

/**
 @brief Decrease Master volume
 @return true if successfully decreased volume, else false
 */
bool SoundController::MasterVolumeDecrease(void)
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

void SoundController::SetMasterVolume(float volume)
{
    cSoundEngine->setSoundVolume(glm::clamp(volume, 0.f, 1.0f));
}

float SoundController::GetVolume()
{
    return cSoundEngine->getSoundVolume();
}

/**
 @brief Increase volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool SoundController::VolumeIncrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetSound(ID)->GetSound();
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
bool SoundController::VolumeDecrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetSound(ID)->GetSound();
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

// For 3D sounds only
/**
 @brief Set Listener position
 @param x A const float variable containing the x-component of a position
 @param y A const float variable containing the y-component of a position
 @param z A const float variable containing the z-component of a position
 */
void SoundController::SetListenerPosition(const float x, const float y, const float z)
{
	vec3dfListenerPos.set(x, y, z);
}

/**
 @brief Set Listener direction
 @param x A const float variable containing the x-component of a direction
 @param y A const float variable containing the y-component of a direction
 @param z A const float variable containing the z-component of a direction
 */
void SoundController::SetListenerDirection(const float x, const float y, const float z)
{
	vec3dfListenerDir.set(x, y, z);
}

/**
 @brief Get an sound from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return A SoundInfo* variable
 */
SoundInfo* SoundController::GetSound(const int ID)
{
	if (soundMap.count(ID) != 0)
		return soundMap[ID];

	return nullptr;
}

/**
 @brief Remove an sound from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return true if the sound was successfully removed, else false
 */
bool SoundController::RemoveSound(const int ID)
{
	SoundInfo* pSoundInfo = GetSound(ID);
	if (pSoundInfo != nullptr)
	{
		delete pSoundInfo;
		soundMap.erase(ID);
		return true;
	}
	return false;
}

/**
@brief Get the number of sounds in this map
@return The number of sounds currently stored in the Map
*/
int SoundController::GetNumOfSounds(void) const
{
	return soundMap.size();
}