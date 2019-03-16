#ifndef SOUNDPRODUCER_H
#define SOUNDPRODUCER_H

#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft


#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

#include <osg/ShapeDrawable> //for object to render on screen

//This is a class that hold positional info on object
//as well as source and buffer components for use with OpenALSoftAudioEngine

//DOES NOT PLAY SOUND. OpenALSoftAudioEngine plays the sound.
//It only holds information to pass to OpenALSoftAudioEngine to play sound.

class SoundProducer
{
	public:
		SoundProducer();
		~SoundProducer();
		
		void setPositionX(double& x); //set x position of sound producer
		float getPositionX(); //get x position of sound producer
		void setPositionY(double& y); //set y position of sound producer
		float getPositionY(); //get y position of sound producer
		void setPositionZ(double& z); //set z position of sound producer
		float getPositionZ(); //get z position of sound producer
		
		void InitSoundProducer(double& x, double& y, double& z);
		
		void setFilepathToSound(std::string& filepath);
		std::string& getFilepathToSound();
		
		ALuint* getBuffer();
		ALuint* getSource();
		
		osg::ShapeDrawable* getRenderObject();
		
	private:
		//position of Listener
		std::vector <double> producer_position_vector;
		enum POSITION_INDEX { X=0,Y=1,Z=2 };
		
		//file path to sound made
		std::string m_filepath;
		
		//buffer to play
		ALuint m_buffer;
    
		//source to play buffer
		ALuint m_source;
		
		//ShapeDrawable object to render
		osg::ref_ptr<osg::ShapeDrawable> renderObject;
		
		osg::ref_ptr<osg::Box> box; 
};
#endif
