#ifndef VIDEO_H
#define VIDEO_H
#include <string>

class Video{

    private:
        std::string _videoTitle;
        std::string _videoUrl;
        std::string _videoDate;

    public:
        std::string getVideoTitle();
        std::string getVideoUrl();
        std::string getVideoDate();
        void setVideoTitle(std::string videoTitle);
        void setVideoUrl(std::string videoUrl);
        void setVideoDate(std::string videoDate);
        Video();


};

#endif