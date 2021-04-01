#ifndef SOUND_BANK_H
#define SOUND_BANK_H

#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft

#include <array>
#include <string>
#include <cstdint>



struct SoundAccount
{
	std::string name; //name of sound
	std::uint8_t account_number; //index in sound accounts array
	std::string stream_file_path; //path to audio file
};

class SoundBank
{
public:
	SoundBank();
	
	void DrawGui_Item();
	
	//operations for handling sound accounts
	
	//change name of sound account
	void ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name);
	
	//function to load audio data to account from a file
	void LoadAudioDataFromFileToAccount(std::string filepath,std::uint8_t account_num);
	
	//used for querying sound
	std::array <std::string,10> &GetAccountLookupTable();
		
	friend class ImmediateModeSoundPlayer;
	
private:

	std::array <SoundAccount,10> m_sound_accounts;
	
	std::array <std::string,10> account_look_up;
	
};
#endif
