#include "Video.h"
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>

std::string Video::getVideoTitle(){
    if(Video::getVideoWatched() == 1){
      return " W | " + _videoTitle;
    }
    if(Video::getVideoWatched() == 2){
      return " N | " + _videoTitle;
    }
    else{
      return "   | " + _videoTitle;
    }

}
std::string Video::getVideoUrl(){
    return _videoUrl;
}
std::string Video::getVideoDate(){
    return _videoDate;
}
int Video::getVideoUnixTime(){
    return _videoUnixTime;
}

std::string Video::getVideoChannel(){
    return _videoChannel;
}
int Video::getVideoWatched(){


    int offset;
    std::string line;
    std::ifstream Myfile;
    Myfile.open ("watched.conf");

    if(Myfile.is_open())
    {
        while(!Myfile.eof())
        {
            getline(Myfile,line);
            if ((offset = line.find(_videoUrl, 0)) != std::string::npos)
            {
              _videoWatched = 1;
            }
        }
        Myfile.close();
    }
    int offset2;
    std::string line2;
    std::ifstream Myfile2;
    Myfile2.open ("new.conf");

    if(Myfile2.is_open())
    {
        while(!Myfile2.eof())
        {
            getline(Myfile2,line2);
            if ((offset2 = line2.find(_videoUrl, 0)) != std::string::npos)
            {
              _videoWatched = 2;
            }
        }
        Myfile2.close();
    }



    return _videoWatched;
}
void Video::setVideoWatched(int videoWatched){

    std::ofstream outfile("watched.conf", std::ios_base::app);
    if(!outfile.is_open()) {
      std::cerr << "Couldn't open 'cache/wacthed.conf'" << std::endl;
    }
    outfile << _videoUrl<<std::endl;
    outfile.close();
    _videoWatched = videoWatched;
}
void Video::setVideoTitle(std::string videoTitle){
    _videoTitle = videoTitle;
}

void Video::setVideoUrl(std::string videoUrl){
    _videoUrl = videoUrl;
}
void Video::setVideoDate(std::string videoDate){
    _videoDate = videoDate;
    _videoUnixTime = unixTime(videoDate);
}

void Video::setVideoUnixTime(int videoUnixTime){
    _videoUnixTime = videoUnixTime;
}
void Video::setVideoChannel(std::string videoChannel){
    _videoChannel = videoChannel;
}

int Video::unixTime(std::string datestr){
	int year, month, day, hour, minute;
	//2019-06-15T08:42:59+00:00
	sscanf(datestr.c_str(), "%4d-%2d-%2dT%2d:%2d",
    &year, &month, &day, &hour, &minute);
	std::tm lDate;

	lDate.tm_sec = 0;
	lDate.tm_min = minute;
	lDate.tm_hour = hour;
	lDate.tm_mday = day;
	lDate.tm_mon = month -1;
	lDate.tm_year = year;
	time_t lTimeEpoch = mktime(&lDate);
	int time = std::atoi(std::to_string(lTimeEpoch).c_str());
	return time;

}

Video::Video(){}
