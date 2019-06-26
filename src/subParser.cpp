#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>

#include "Channel.h"
#include "Video.h"
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include "subParser.h"

#include "rapidxml-1.13/rapidxml_ext.h"



using namespace rapidxml;
using namespace std;



void subParser::clearCache(){
	boost::filesystem::remove_all("cache");
	boost::filesystem::remove_all("temp");
}

void subParser::createCache(){
	
	boost::filesystem::create_directory("cache");
	boost::filesystem::create_directory("temp");
	

}

void subParser::dlChannelXMLs(){
	
	subParser::createCache();
	system("./src/getID.sh");
	
	system("./src/downloadXMLs.sh");

}


Channel subParser::openChannel(string xml){
	
	Channel currChannel;
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile (xml);
	////cout << "Parsing " << xml << endl;
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("feed");
	// Iterate over the brewerys
	//cout<<"openChannel1"<<endl;
	currChannel.setChannelName(root_node->first_node("author")->first_node("name")->value());
	currChannel.setChannelUrl(root_node->first_node("author")->first_node("uri")->value());
		//cout<<"openChannel2"<<endl;

	vector<Video> videoVector;
	for (xml_node<> * entry_node = root_node->first_node("entry"); entry_node; entry_node = entry_node->next_sibling("entry"))
	{
		Video currVid;
		currVid.setVideoTitle(entry_node->first_node("title")->value());
		currVid.setVideoUrl(entry_node->first_node("link")->first_attribute("href")->value());
		currVid.setVideoDate(entry_node->first_node("published")->value());
		
		currVid.setVideoChannel(entry_node->first_node("author")->first_node("name")->value());
		
		
	   	videoVector.push_back(currVid);
		
	}
	currChannel.setChannelVideoVector(videoVector);
	return currChannel;

}

vector<Channel> subParser::getChannelVector(){
	
	boost::filesystem::path cachePath("cache");
	  //cout<<"heygetChannelVector1"<<endl;

	if (!boost::filesystem::is_empty(cachePath)){
		vector<Channel> chanVect;
			  //cout<<"heygetChannelVector2"<<endl;

		for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(cachePath)){
				  //cout<<"heygetChannelVector3"<<endl;
			//cout<<entry.path().string()<<endl;
			chanVect.push_back(openChannel(entry.path().string()));
			
			  //cout<<"heygetChannelVector4"<<endl;

		}

		return chanVect;
	}
	else return updateGetChannelVector();


}

bool subParser::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
std::string subParser::normaliseDate(std::string datestr){
	char day[256], month[256], year[256], hour[256], minute[256];
	//2019-06-15T08:42:59+00:00
	sscanf(datestr.c_str(), "%4s-%2s-%2sT%2s:%2s",
    year, month, day, hour, minute);
	std::string d(day), m(month), y(year), h(hour), mi(minute);
	
	std::string output =  h + ":" +  mi + " "
		+  d + "/" +  m + "/" + y;
	return output;


}

bool subParser::wayToSort(Video a, Video b){
	int i = a.getVideoUnixTime();
	int j = b.getVideoUnixTime();
	return i>j;
}

std::vector<Video> subParser::getAllVector(std::vector<Channel> chans){

	std::vector<Video> allchansVideoVect;
	for (auto & element : chans) {
		for (auto & vids : element.getVideoVector()){
			allchansVideoVect.push_back(vids);
		}
	}
	std::sort(allchansVideoVect.begin(), allchansVideoVect.end(), wayToSort);
	

	return allchansVideoVect;
}

void subParser::updateChanXML(){
	
	boost::filesystem::path cachePath("cache");

	dlChannelXMLs();
	//cout<<"hey1"<<endl;
	for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(cachePath)){

		if(!hasEnding(entry.path().string(),".1") && boost::filesystem::exists(entry.path().string()+".1")){
			//cout<<"hey2"<<endl;
			xml_document<> olddoc;
			xml_node<> * oldroot_node;
			// Read the xml file into a vector
			ifstream theOldFile (entry.path().string());
			////cout << "Parsing " << xml << endl;
			vector<char> oldbuffer((istreambuf_iterator<char>(theOldFile)), istreambuf_iterator<char>());
			oldbuffer.push_back('\0');
			// Parse the buffer using the xml file parsing library into doc 
			olddoc.parse<0>(&oldbuffer[0]);
			// Find our root node
			oldroot_node = olddoc.first_node("feed");
			std::string firstOldentryTitle = oldroot_node->first_node("entry")->first_node("title")->value();

			xml_document<> doc;
			xml_node<> * root_node;
			// Read the xml file into a vector
			ifstream theFile (entry.path().string()+".1");
			////cout << "Parsing " << xml << endl;
			vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
			buffer.push_back('\0');
			// Parse the buffer using the xml file parsing library into doc 
			doc.parse<0>(&buffer[0]);
			// Find our root node
			root_node = doc.first_node("feed");
			

			for (xml_node<> * entry_node = root_node->first_node("entry"); entry_node; entry_node = entry_node->next_sibling())
			{
				std::string firstentryTitle = entry_node->first_node("title")->value();
				// xml_node<> * nodeToInsert = doc.allocate_node(node_element, "entry");
				// xml_node<> * nodeToInsert2 = doc.allocate_node(node_element, "cunt");
				// nodeToInsert->append_node(nodeToInsert2);

				
				//cout<<"firstentryTitle = "<<firstentryTitle<<endl;
				//cout<<"firstOldentryTitle = "<<firstOldentryTitle<<endl;

				if( firstentryTitle == firstOldentryTitle ) break;
				xml_node<>* a = entry_node;
				xml_node<> *node = olddoc.clone_node( a );
				xml_node<> *whereNode = olddoc.clone_node(oldroot_node->first_node("published"));
				olddoc.first_node("feed")->prepend_node(node);
				
			}
			
			std::ofstream file_stored(entry.path().string());
			file_stored << olddoc;
			file_stored.close();
			olddoc.clear();
			doc.clear();
			boost::filesystem::remove(entry.path().string()+".1");
		}
	
	}

	boost::filesystem::remove("temp/channelXMLList.conf");




}

vector<Channel> subParser::updateGetChannelVector(){

	updateChanXML();
	boost::filesystem::path cachePath("cache");
	//boost::filesystem::path cacheNewPath("cache/new");

	
	vector<Channel> chanVect;
	for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(cachePath)){
		chanVect.push_back(openChannel(entry.path().string()));
		
	}


    return chanVect;
	
}
// int main(void)
// {	
// 	// subParser s;
// 	// s.createCache();
// 	// s.updateChanXML();

// }
