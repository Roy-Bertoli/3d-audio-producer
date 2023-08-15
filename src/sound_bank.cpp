#include "sound_bank.h"

#include "audio-stream-container.h"
#include <sndfile.h>

///Data Directory File Path

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

//DATADIR macro used by automake for path to data files
//if DATADIR macro is not defined, define it as data
#ifndef DATADIR
    #define DATADIR "../data/resources"
#endif

#define DATADIR_NAME STR(DATADIR)

#include <unistd.h>

#include "raygui/raygui.h"

#include "raygui/gui_file_dialog.h"

#include <cstring>

#include "dialog_var.h"

static GuiFileDialogState fileDialogState;

//for .flac audio file decoding
#define DR_FLAC_IMPLEMENTATION
#include "backends/dr_flac.h"

//for .wav file decoding
#define DR_WAV_IMPLEMENTATION
#include "backends/dr_wav.h"

SoundBank::SoundBank()
{
	SoundBank::InitDataDirectory("");
	
	fileDialogState  = InitGuiFileDialog(GetWorkingDirectory());
	fileDialogState.windowBounds = {200, 200, 440, 310};
	
}

struct TextBoxParam
{
	char char_name[20] = " ";
	bool name_box_pressed = false;
};

std::array <TextBoxParam,10> name_textboxes;

std::array <std::string,10> filepath_textboxes;
char fileNameToLoad[512] = { 0 };
std::uint8_t current_file_button_edit = 0;


void SoundBank::DrawGui_Item()
{
	//draw rectangle
	
	float leftX = GetScreenWidth() - 200;
	
	//for each account from start to last account number
	for(std::uint8_t i = 0; i < m_sound_accounts.size(); i++)
	{
		//draw account number
		char num[3];
		strncpy ( num, std::to_string(i).c_str(), sizeof(num) );
		GuiTextBox((Rectangle){leftX,100.f + i*30,20,25}, &num[0], 20, false);
		//draw textbox with name of sound and account number
		if( GuiTextBox((Rectangle){leftX + 25,100.f + i*30,100,25},
						name_textboxes[i].char_name, 20, name_textboxes[i].name_box_pressed
					) )
		{
			name_textboxes[i].name_box_pressed = !name_textboxes[i].name_box_pressed;
			SoundBank::ChangeSoundNameForAccount( i , std::string(name_textboxes[i].char_name) );
		}
			
	}
	
	
	if (fileDialogState.SelectFilePressed)
	{
		// Load image file (if supported extension)
		if (IsFileExtension(fileDialogState.fileNameText, ".wav") || IsFileExtension(fileDialogState.fileNameText, ".flac"))
		{
			strcpy(fileNameToLoad, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
			std::string filepath = std::string(fileNameToLoad);
			//filepath_textboxes[current_file_button_edit] = filepath;
			//m_sound_bank_save_data.sound_account_data[current_file_button_edit].stream_file_path = filepath;
			
			//std::cout << "filepath in main gui editor: " << filepath << std::endl;
			//load audio data
			SoundBank::LoadAudioDataFromFileToAccount(filepath,current_file_button_edit);
		}

		fileDialogState.SelectFilePressed = false;
		global_dialog_in_use = false;
	}
	
	if (fileDialogState.windowActive){ GuiLock(); global_dialog_in_use = true;}
	
	for(std::uint8_t i = 0; i < 10; i++)
	{
		//draw open file button
		if( GuiButton( (Rectangle){ leftX + 125,100.f + i*30,50,25 }, filepath_textboxes[i].c_str() ) )
		{
			current_file_button_edit = i;
			fileDialogState.windowActive = true; //activate file dialog
			break; //stop loop
		}
	}
	
	GuiUnlock();
	
	//if clicked on call GuiFileDialog menu
	GuiFileDialog(&fileDialogState);
	
	
}

void SoundBank::ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name)
{
	m_sound_accounts[account_num].name = new_name;
	account_look_up[account_num] = new_name;
	
	m_sound_bank_save_data.sound_account_data[account_num].name = new_name;
}
	
//used for querying sound
std::array <std::string,10> &SoundBank::GetAccountLookupTable(){return account_look_up;}

#define	BUFFER_LEN	1024
#define	MAX_CHANNELS	2

bool ReadAndCopyDataFromInputFile_WAV(std::vector<int32_t> *audio_data_input_copy_ptr,std::string inputSoundFilePath)
{
	//The code taken from example code in dr_wav.h uses a version of the API that 
	//converts the audio data to a consistent format 
	//(32-bit signed PCM, in this case)

	//read data
	
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32("my_song.wav", &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL) {
        // Error opening and reading WAV file.
    }

    //To Do: copy audio data to vector

    drwav_free(pSampleData, NULL);

	
}

bool ReadAndCopyDataFromInputFile_FLAC(std::vector<int32_t> *audio_data_input_copy_ptr,std::string inputSoundFilePath)
{
	
	unsigned int channels;
    unsigned int sampleRate;
    drflac_uint64 totalPCMFrameCount;
    
    //decode audio data
    drflac_int32* pSampleData = drflac_open_file_and_read_pcm_frames_s32("MySong.flac", &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL) {
        // Failed to open and decode FLAC file.
    }

    //To Do: copy audio data to vector

    drflac_free(pSampleData, NULL);
	
}

bool WriteAudioDataToWAVFile(std::vector<int32_t> *audio_data_input_copy_ptr,std::string outputSoundFilePath){
	
	//
	drwav_data_format format;
    format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
    format.format = DR_WAVE_FORMAT_PCM;          // <-- Any of the DR_WAVE_FORMAT_* codes.
    format.channels = 1;
    format.sampleRate = 44100;
    format.bitsPerSample = 32; //32 bit
    
    //To Do : copy audio data stored in vector to wav.
    drwav wav;
    
    //initialize output file
    drwav_init_file_write(&wav, outputSoundFilePath.c_str(), &format, NULL);
	
	//write to output file
    //drwav_uint64 framesWritten = drwav_write_pcm_frames(pWav, frameCount, pSamples);
}

bool ReadAndCopyDataFromInputFile(std::vector<double> *audio_data_input_copy_ptr,std::string inputSoundFilePath,SF_INFO& input_sfinfo)
{
	SNDFILE *inputFile;
	
	//Read data from file
	if (! (inputFile = sf_open (inputSoundFilePath.c_str(), SFM_READ, &input_sfinfo)))
	{	
		// Open failed, so print an error message.
		std::cout << "Not able to open input file " <<  inputSoundFilePath << std::endl;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		return false;
	} 
		
	if (input_sfinfo.channels > MAX_CHANNELS)
	{
		std::cout << "Not able to process more than" <<  MAX_CHANNELS << "channels.\n";
		return false;
	}
	
	std::cout << "Successfully loaded " << inputSoundFilePath << " saving data..." << std::endl;
	
	//read input data
	std::vector<double> read_buf(BUFFER_LEN);
	size_t read_size = 0;
	
	
	while( (read_size = sf_read_double(inputFile, read_buf.data(), read_buf.size()) ) != 0)
	{
		audio_data_input_copy_ptr->insert(audio_data_input_copy_ptr->end(), read_buf.begin(), read_buf.begin() + read_size);
	}
	
	/* Close input and stream files. */
	sf_close(inputFile);
	
	return true;
}



bool CopyInputDataIntoAudioDataStream(std::vector<double> *audio_data_input_copy_ptr, AudioStreamContainer* audio_data_stream_ptr,std::string streamSoundFilePath,SF_INFO& input_sfinfo)
{
	
	//copy input audio data references to audio data stream
	audio_data_stream_ptr->ResizeAudioStream(audio_data_input_copy_ptr->size());
	for(size_t i=0; i < audio_data_stream_ptr->GetSize(); i++)
	{
		double* ref_at_i = &(audio_data_input_copy_ptr->at(i));
		audio_data_stream_ptr->SetPointerToDataAtThisSampleIndex(ref_at_i,i);
	}
	
	double slen;
	slen = audio_data_stream_ptr->GetSize() * sizeof(uint16_t);
	
	std::cout << "Size of data in bytes: " << slen << "\n";
	//if sample buffer is null or size of buffer data is zero, notify of error
	if(slen == 0)
	{
		std::string messageString;
		messageString.append("Failed to read audio from file.\n");
		return false;
	}
	
	double seconds = (1.0 * input_sfinfo.frames) / input_sfinfo.samplerate ;
	std::cout << "Duration of sound:" << seconds << "s. \n";
	
	audio_data_stream_ptr->WriteStreamContentsToFile(streamSoundFilePath, input_sfinfo.format, input_sfinfo.channels, input_sfinfo.samplerate,int(BUFFER_LEN));
	
	return true;
}

void SoundBank::LoadAudioDataFromFileToAccount(std::string filepath,std::uint8_t account_num)
{
	//object to hold audio data for streaming
	AudioStreamContainer audio_data_stream;
	
	//Hold data for left channel and right channel
	std::vector <double> audio_data_input_copy;
	
	//audio format info
	SF_INFO input_sfinfo;
		
	std::cout << "Input Sound file path:" << filepath << std::endl;
	
	std::cout << "Stream sound file path: " << m_sound_accounts[account_num].stream_file_path << std::endl;
	
	bool readDone = true;
	bool copyDone = true;
	
	readDone = ReadAndCopyDataFromInputFile(&audio_data_input_copy,filepath,input_sfinfo);
	
	copyDone = CopyInputDataIntoAudioDataStream(&audio_data_input_copy,&audio_data_stream, 
									m_sound_accounts[account_num].stream_file_path,
									input_sfinfo);
									
	m_sound_accounts[account_num].active = readDone && copyDone;
	
	//clear data stored
	audio_data_stream.ClearStreamDataStored();
	audio_data_input_copy.clear();
}

void SoundBank::LoadSaveData(SoundBankSaveData& data)
{
	m_sound_bank_save_data = data;
	
	//set sound account data from save data
	//set account lookup from save data
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i] = data.sound_account_data[i];
		
		#if defined(WIN32)
		m_sound_accounts[i].stream_file_path = m_data_dir_path  + "stream-file" + std::to_string(i) + ".wav";
		#else
		m_sound_accounts[i].stream_file_path = m_data_dir_path + "stream-file" + std::to_string(i) + ".wav";
		#endif
		
		account_look_up[i] = data.sound_account_data[i].name;
		
		SoundBank::ChangeSoundNameForAccount( i ,  data.sound_account_data[i].name);
		
		strncpy(name_textboxes[i].char_name,m_sound_accounts[i].name.c_str(),20);
		name_textboxes[i].char_name[19] = '\0';
		
		
		//filepath_textboxes[i] = data.sound_account_data[i].stream_file_path;
		
		if(data.sound_account_data[i].active)
		{
			if(!data.sound_account_data[i].stream_file_path.empty())
			{
				SoundBank::LoadAudioDataFromFileToAccount(m_sound_accounts[i].stream_file_path, i);
			}
		}
				
	}
	
}

SoundBankSaveData& SoundBank::GetSaveData(){return m_sound_bank_save_data;}

void SoundBank::InitDataDirectory(std::string filepath)
{
	m_data_dir_path = filepath;
	
	//for all sound accounts, initialize account number and stream file path
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i].account_number = i;
				
		std::string filepath_stream = m_data_dir_path + "stream-file" + std::to_string(i) + ".wav";
		
		m_sound_bank_save_data.sound_account_data[i] = m_sound_accounts[i];
		m_sound_accounts[i].stream_file_path = filepath_stream;
		m_sound_accounts[i].active = false;
	}
}
