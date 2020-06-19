#include "Channel.h"
#include <string>
#include <vector>
#include "Video.h"
#include "subParser.h"

std::string Channel::getChannelName(){
    return _channelName;
}

std::string Channel::getChannelUrl(){
    return _channelUrl;
}

void Channel::setChannelName(std::string channelName){
    _channelName = channelName;
}
void Channel::setChannelUrl(std::string channelUrl){
    _channelUrl =channelUrl;
}

std::vector<Video> Channel::getVideoVector(){
  subParser sP;
  std::vector<Video> videoVect;
  for (auto & element : _videoVector) {

      videoVect.push_back(element);

  }
  std::sort(videoVect.begin(), videoVect.end(), sP.wayToSort);
  _videoVector = videoVect;
    return _videoVector;
}

Channel::Channel(){}
Channel::Channel(std::string channelName, std::string channelUrl){
    _channelName = channelName;
    _channelUrl = channelUrl;


}
void Channel::setChannelVideoVector(std::vector<Video> vidVec){
    _videoVector = vidVec;
}
