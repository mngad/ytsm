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




using namespace rapidxml;
using namespace std;

Channel openChannel(string xml);
vector<Channel> getChannelVector(string xmlFile);
void createCache();

void clearCache(){
	boost::filesystem::remove_all("cache");
}

void createCache(){
	
	boost::filesystem::create_directory("cache");
	

}

void dlChannelXMLs(){
	system("./getID.sh");
	createCache();
	system("./downloadXMLs.sh");

}


Channel openChannel(string xml){
	
	Channel currChannel;
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile (xml);
	cout << "Parsing " << xml << endl;
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("feed");
	// Iterate over the brewerys
	currChannel.setChannelName(root_node->first_node("author")->first_node("name")->value());
	currChannel.setChannelUrl(root_node->first_node("author")->first_node("uri")->value());
	vector<Video> videoVector;
	for (xml_node<> * entry_node = root_node->first_node("entry"); entry_node; entry_node = entry_node->next_sibling())
	{
		Video currVid;
		currVid.setVideoTitle(entry_node->first_node("title")->value());
		currVid.setVideoUrl(entry_node->first_node("link")->first_attribute("href")->value());
		currVid.setVideoDate(entry_node->first_node("published")->value());
	   videoVector.push_back(currVid);

	}
	currChannel.setChannelVideoVector(videoVector);
	return currChannel;

}


int main(void)
{	
	clearCache();
	boost::filesystem::path cachePath("cache");
	dlChannelXMLs();
	vector<Channel> chanVect;
	for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(cachePath)){

		chanVect.push_back(openChannel(entry.path().string()));
	}
    

  	
  	for (auto & element : chanVect) {
		cout<<element.getChannelName()<<endl;
	  	vector<Video> vvec = element.getVideoVector();
	  	for(auto & video : vvec) {
			cout<<video.getVideoTitle()<<endl;
		  	cout<<video.getVideoUrl()<<"\n"<<endl;
	  }
}

}
