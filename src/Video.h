#ifndef VIDEO_H
#define VIDEO_H
#include <string>

class Video{

    private:
        std::string _videoTitle;
        std::string _videoUrl;
        std::string _videoDate;
        std::string _videoChannel;
        int _videoUnixTime;
        int _videoWatched;

    public:
        std::string getVideoChannel();
        std::string getVideoTitle();
        std::string getVideoUrl();
        std::string getVideoDate();
        int getVideoWatched();
        void setVideoWatched(int videoWatched);
        int getVideoUnixTime();
        void setVideoChannel(std::string videoChannel);
        void setVideoTitle(std::string videoTitle);
        void setVideoUrl(std::string videoUrl);
        void setVideoDate(std::string videoDate);
        void setVideoUnixTime(int videoUnixTime);
        int unixTime(std::string datestr);
        Video();


};

#endif
