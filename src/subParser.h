#ifndef SUBPARSER_H
#define SUBPARSER_H
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"
#include "Channel.h"
#include "Video.h"
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <ctime>
#include <iomanip>
#include <sstream>

class subParser{
    private:
        std::string _lastUpdated;
    public:
        void currentDateTime();
        std::string getUpdatedTime();
        void setUpdatedTime(std::string lastUpdated);
        Channel openChannel(std::string xml);
        std::vector<Channel> getChannelVector();
        void createCache();
        void clearCache();
        void dlChannelXMLs();
        std::vector<Channel> updateGetChannelVector();
        void updateChanXML();
        bool hasEnding (std::string const &fullString, std::string const &ending);
        std::string normaliseDate(std::string datestr);
        std::vector<Video> getAllVector(std::vector<Channel> chans);
        static bool wayToSort(Video a, Video b);
        long tz_offset(time_t when);
        int numDigits(int no);

};

#endif