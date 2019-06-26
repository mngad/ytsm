#include "Video.h"
#include <string>
#include <ctime>

std::string Video::getVideoTitle(){
    return _videoTitle;
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