#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>



#include "Channel.h"
#include "Video.h"
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include "subParser.h"

#include "rapidxml-1.13/rapidxml_ext.h"



using namespace rapidxml;
using namespace std;

std::string subParser::getUpdatedTime(){
	return _lastUpdated;


}
void subParser::setUpdatedTime(std::string lastUpdated){
	_lastUpdated = lastUpdated;
}

bool subParser::compareFuncAlpha (Channel cha, Channel chb) {
	std::string a = cha.getChannelName();
	std::string b = chb.getChannelName();
	return a<b;

}


void subParser::parseOPML(){
  xml_document<> doc;
	xml_node<> * root_node;


	// Read the xml file into a vector
	ifstream theFile ("subscription_manager");
	////cout << "Parsing " << xml << endl;
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("opml")->first_node("body")->first_node("outline");




	ofstream outfile("temp/channelXMLList.conf");
	if(!outfile.is_open()) {
		cerr << "Couldn't open 'temp/channelXMLList.conf'" << endl;

	}
	else{
		//cout << "openned " << '\n';
	}



	for (xml_node<> * outline_node = root_node->first_node("outline"); outline_node; outline_node = outline_node->next_sibling("outline"))
	{
		string textStr = outline_node->first_attribute("text")->value();
		cout<<textStr<<endl;
	//	if(textStr.compare("YouTube Subscriptions") == 0) {
	//		cout<<"first one - it matches"<<endl;
	//		break;
	//	}
		//cout<< "here"<< endl;
		//cout << "xmlUrl" << outline_node->first_attribute("xmlUrl")->value()<< '\n';
	  //cout << "xmlUrl" << outline_node->first_attribute("xmlUrl")->value()<< '\n';
		outfile << outline_node->first_attribute("xmlUrl")->value() << endl;

	}
	outfile.close();


}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
void subParser::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    setUpdatedTime(buf);
}


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
	if ( !boost::filesystem::exists( "subscription_manager" ) ){
		system("./src/getID.sh");

		system("./src/downloadXMLs.sh");
	}
	else{
		subParser::parseOPML();
		system("./src/downloadXMLs.sh");
		//cout<<"done"<<endl;
	}


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

// returns difference in hours from UTC at given time
// or at current time if not specified
long subParser::tz_offset(time_t when)
{

    auto const tm = *std::localtime(&when);
    std::ostringstream os;
    os << std::put_time(&tm, "%z");
    std::string s = os.str();
    // s is in ISO 8601 format: "±HHMM"
    int h = std::stoi(s.substr(0,3), nullptr, 10);
    int m = std::stoi(s[0]+s.substr(3), nullptr, 10);

    return h;
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
		sort(chanVect.begin(),chanVect.end(),compareFuncAlpha);
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

int subParser::numDigits(int no){
	int a;
	while(no>0){
		no=no/10;
		a++;
	}
	return a;
}
std::string subParser::normaliseDate(std::string datestr){

	std::string year, month, day, hour, minute;
	int yeari, monthi, dayi, houri, minutei;
	//2019-06-15T08:42:59+00:00
	sscanf(datestr.c_str(), "%4d-%2d-%2dT%2d:%2d",
    &yeari, &monthi, &dayi, &houri, &minutei);
	std::tm lDate;

	lDate.tm_sec = 0;
	lDate.tm_min = minutei;
	lDate.tm_hour = houri;
	lDate.tm_mday = dayi;
	lDate.tm_mon = monthi -1;
	lDate.tm_year = yeari;
	time_t lTimeEpoch = mktime(&lDate);

	houri = houri + +tz_offset(lTimeEpoch); // add the timezone
	if(numDigits(monthi)<=1) month = "0" + std::to_string(monthi);
	else month = std::to_string(monthi);
	if(numDigits(dayi)<=1) day = "0" + std::to_string(dayi);
	else day = std::to_string(dayi);
	if(numDigits(houri)<=1) hour = "0" + std::to_string(houri);
	else hour = std::to_string(houri);
	if(numDigits(minutei)<=1) minute = "0" + std::to_string(minutei);
	else minute = std::to_string(minutei);
	year = std::to_string(yeari);

	std::string output =  hour + ":" + minute + " " + day + "/" + month + "/" +year;


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
//cout<<"hey2"<<endl;
		if(!hasEnding(entry.path().string(),".1") && boost::filesystem::exists(entry.path().string()+".1")){
			//cout<<"hey3"<<endl;
			//cout<<entry.path().string()<<endl;

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
			if(oldroot_node->first_node("entry") != 0) {
				//cout<<"NO ENTRY!!!!!!!!!!!!!!!!!"<<endl;
				std::string firstOldentryID = oldroot_node->first_node("entry")->first_node("id")->value();

				for (xml_node<> * entry_node = root_node->first_node("entry"); entry_node; entry_node = entry_node->next_sibling())
				{
					std::string firstentryID = entry_node->first_node("id")->value();
					cout<<entry_node->first_node("title")->value()<<endl;
					// xml_node<> * nodeToInsert = doc.allocate_node(node_element, "entry");
					// xml_node<> * nodeToInsert2 = doc.allocate_node(node_element, "cunt");
					// nodeToInsert->append_node(nodeToInsert2);


					//cout<<"firstentryID = "<<firstentryID<<endl;
					//cout<<"firstOldentryID = "<<firstOldentryID<<endl;

					if( firstentryID == firstOldentryID ) break;
					xml_node<>* a = entry_node;
					xml_node<> *node = olddoc.clone_node( a );
					xml_node<> *whereNode = olddoc.clone_node(oldroot_node->first_node("published"));
					olddoc.first_node("feed")->prepend_node(node);

				}
			}

			std::ofstream file_stored(entry.path().string());
			file_stored << olddoc;
			file_stored.close();
			olddoc.clear();
			doc.clear();
			//cout<<"hey4"<<endl;
			boost::filesystem::remove(entry.path().string()+".1");
			//cout<<"hey5"<<endl;
		}

	}

	boost::filesystem::remove("temp/channelXMLList.conf");

	currentDateTime();


}

vector<Channel> subParser::updateGetChannelVector(){

	updateChanXML();
	boost::filesystem::path cachePath("cache");
	//boost::filesystem::path cacheNewPath("cache/new");


	vector<Channel> chanVect;
	for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(cachePath)){
		chanVect.push_back(openChannel(entry.path().string()));

	}

		//sort(chanVect.begin().getChannelName(),chanVect.end().getChannelName(),compareFuncAlpha);
		sort(chanVect.begin(),chanVect.end(),compareFuncAlpha);
    return chanVect;

}
// int main(void)
// {
// 	// subParser s;
// 	// s.createCache();
// 	// s.updateChanXML();

// }
