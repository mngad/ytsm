#include "Video.h"
#include <string>

std::string Video::getVideoTitle(){
    return _videoTitle;
}
std::string Video::getVideoUrl(){
    return _videoUrl;
}
std::string Video::getVideoDate(){
    return _videoDate;
}
void Video::setVideoTitle(std::string videoTitle){
    _videoTitle = videoTitle;
}
void Video::setVideoUrl(std::string videoUrl){
    _videoUrl = videoUrl;
}
void Video::setVideoDate(std::string videoDate){
    _videoDate = videoDate;
}

Video::Video(){}