#ifndef CHANNEL_H
#define CHANNEL_H
#include <string>
#include <vector>
#include "Video.h"


class Channel{
    private:
        std::string _channelName;
        std::string _channelUrl;
        std::vector<Video> _videoVector;

    public:
        std::string getChannelName();
        std::string getChannelUrl();
        void setChannelUrl(std::string channelUrl);
        void setChannelName(std::string channelName);
        std::vector<Video> getVideoVector();
        Channel(std::string _channelName, std::string _channelUrl);
        Channel();
        void setChannelVideoVector(std::vector<Video> vidVec);


};

#endif