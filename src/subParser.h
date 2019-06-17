#ifndef SUBPARSER_H
#define SUBPARSER_H
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"
#include "Channel.h"
#include "Video.h"
#include <boost/filesystem.hpp>
#include <stdlib.h>

class subParser{

    public:
        Channel openChannel(std::string xml);
        std::vector<Channel> getChannelVector();
        void createCache();
        void clearCache();
        void dlChannelXMLs();
        

};

#endif