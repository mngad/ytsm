#include <iostream>
#include <signal.h>
#include <ncurses.h>
#include <menu.h>
#include <string>
#include <unistd.h>
#include <boost/thread.hpp>
#include <cmath>
#include "subParser.h"
#include "Video.h"
#include "Channel.h"
#include <ctime>

using namespace std;

#define SCREEN_MAIN 0

#define KKEY_DOWN 258
#define KKEY_UP 259
#define KEY_q 113
#define KEY_ENTER 13
#define NEXT 93
#define PREV 91
#define MKEY 109
#define UKEY 117
#define AKEY 97


char *MenuItems[1024];
int SelItem = 0;
int LastItem = 0;
char *HeaderText;

WINDOW *win;
bool Terminated;
int Screen = 0;
int LastKey;
int MaxX = 0;
int MaxY = 0;
int CurrentItem =0;
std::string currentAction;
int currChan = 0;
int windowtype =0;
int maxWinType =2;
bool updating = false;
int shift = 0;
subParser p;
std::vector<Channel> channelVector;

void openVideo(std::string url);
int processPageView(vector<Channel> chvec);
void ClearLine(int y, int l);
void ClearLinePart(int y, int x, int l);
vector<Video> getVVec(vector<Channel> chanVec, int currID);
int processIndividualMode(vector<Channel> chvec);
int processAllMode(vector<Channel> chvec);

void shiftFactor(int dir){
  if(shift >= 0){
    if((SelItem > MaxY-4)&&(dir<0))shift++;
    if((SelItem < shift)&&(dir>0))shift--;
  }
}

std::string fillIn(int textSize, int endpoint){
  int spaceSize = std::abs(endpoint - textSize);
  char Str[spaceSize];
  for (int i = 0; i < spaceSize; i++) Str[i] = ' ';
  return Str;
}

void upchVecThread(){



  std::vector<Channel> channelVectorTemp;
  updating = true;
  channelVectorTemp = p.updateGetChannelVector();
  channelVector.clear();
  channelVector = channelVectorTemp;
  updating = false;
  ClearLine(2,MaxY);
  SelItem=0;
  
  currentAction = "Updated";
    
}

void update(std::string ch){
    
  subParser p;
  boost::thread thread = boost::thread(upchVecThread);
  thread.detach();
  //channelVector = p.updateGetChannelVector();
    
  ClearLine(2,MaxY);
  SelItem=0;
  //windowtype=1;
  

}

void changeScreen(){
  
  if(windowtype+1<=maxWinType)windowtype++;
  else {
    windowtype =0;
    currChan = SelItem;
  }
  SelItem =0;
}

int Process(std::string ch,vector<Channel> chvec) {
  getmaxyx(win, MaxY, MaxX);

  if (LastKey == MKEY){
    windowtype = 1;
    SelItem =0;
  }
  if (LastKey == AKEY){
    windowtype =2;
    SelItem =0;
  }
  if (LastKey == UKEY){
    currentAction ="Updating...";
    update(ch);
    
  }
  switch (Screen) {
  default: 
    if (windowtype==0){
      processPageView(chvec);
    }
    if(windowtype==1){
      processIndividualMode(chvec);
    }
    if(windowtype==2){
      processAllMode(chvec);
    }
  }

  if (LastKey == KEY_q) {
    Terminated = true;
  }
  return 0;
}

int processGenericPre(vector<Video> v , std::string title){
 LastItem = v.size();
  
  attrset(COLOR_PAIR(1));
  for (int i = 0; i <= MaxY; i++) ClearLine(i, MaxX);

  // draw header
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(0, MaxX);
  mvaddstr(0, 0, HeaderText);
  mvaddstr(0, 1, title.c_str());
  mvaddstr(0, MaxX-20, p.getUpdatedTime().c_str());
  // draw body
  attrset(COLOR_PAIR(1));

}
int processGenericPost(){
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(MaxY - 2, MaxX);
  mvaddstr(MaxY - 2, 0, currentAction.c_str());
  
  curs_set(0);
  refresh();
  
  LastKey = getch();
  
  if (LastKey == KEY_UP) {
    if(SelItem -1 >= 0){
      SelItem--;
      shiftFactor(1);
    }
    //cout<<CurrentItem<<endl;
  }
  if (LastKey == KEY_DOWN) {
    if(SelItem +1 <LastItem) {
      SelItem= SelItem+1;
     shiftFactor(-1);
    }
  }

}

int processAllMode(vector<Channel> chvec) {
  vector<Video> v = p.getAllVector(chvec);
 
  processGenericPre(v, "All Videos");
  int size;
  if(v.size()>MaxY)size = MaxY-3;
  else size = LastItem;
  for (int i = 0; i < size; i++) {
    if (SelItem == i+shift) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    if((i+shift)<=v.size()){

      mvaddstr(i +1, 0, (v[i+shift].getVideoChannel()+ 
        fillIn(v[i+shift].getVideoChannel().length(),23)).c_str());
    
      mvaddstr(i+1, 23, " |");
      mvaddstr(i +1, 25, (v[i+shift].getVideoTitle()+
        fillIn(v[i+shift].getVideoTitle().length(),MaxX-18)).c_str());
      mvaddstr(i+1, MaxX-18, " |");
    
      mvaddstr(i +1, MaxX-16, (p.normaliseDate(v[i+shift].getVideoDate()).c_str()));
    }
  }
  //mvaddstr(0,120,("SelItem = " + std::to_string(SelItem) + "shift = "+std::to_string(shift)).c_str());
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  processGenericPost();
  currentAction = v[SelItem].getVideoUrl();
  if (LastKey == KEY_ENTER){
     
    std::string url = v[SelItem].getVideoUrl();
    currentAction = "opening " + url;
    openVideo(url);
  }
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}

int processPageView(vector<Channel> chvec) {
  vector<Video> v = getVVec(chvec,currChan);
  std::string title = chvec[currChan].getChannelName();
  
  processGenericPre(v, title);
  int size;
  if(v.size()>MaxY)size = MaxY-3;
  else size = LastItem;
  for (int i = 0; i < size; i++) {
    if (SelItem == i+shift) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    mvaddstr(i +1, 0, (v[i+shift].getVideoTitle()+
        fillIn(v[i+shift].getVideoTitle().length(),MaxX-18)).c_str());
    mvaddstr(i+1, MaxX-18, " |");
   
    mvaddstr(i +1, MaxX-16, p.normaliseDate(v[i+shift].getVideoDate()).c_str());
  }
  
  processGenericPost();
  currentAction = v[SelItem].getVideoUrl();
  if (LastKey == KEY_ENTER){
     
    std::string url = v[SelItem].getVideoUrl();
    currentAction = "opening " + url;
    openVideo(url);
  }
  if(LastKey == NEXT){
    if(currChan +1 <chvec.size())currChan++;
    ClearLine(2,MaxY);
    SelItem =0;
  }
  if(LastKey == PREV){
    if(currChan -1 >=0)currChan--;
    ClearLine(2,MaxY);
    SelItem =0;
  }


  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}

int processIndividualMode(vector<Channel> chvec) {
  vector<Video> v = getVVec(chvec,currChan);
  processGenericPre(v, "All Channels");
  LastItem = chvec.size();
  int size;
  if(chvec.size()>MaxY)size = MaxY-3;
  else size = LastItem;
  for (int i = 0; i < size; i++) {
    if (SelItem == i+shift) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    if(i+shift < chvec.size()){
    mvaddstr(i +1, 0, chvec[i+shift].getChannelName().c_str());
  }
    //mvaddstr(1, 0, std::to_string(LastKey).c_str());
    //mvaddstr(1, 1, std::to_string(MaxY).c_str());

  }

  processGenericPost();
  if (LastKey == KEY_ENTER){
     
    currChan = SelItem;
    windowtype = 0;
    SelItem = 0;
  
  }
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}



void openVideo(std::string url){
  std::string callFunc = "/usr/bin/mpv --no-terminal " + url + " &";
  system(callFunc.c_str());
  ClearLine(0,MaxY);
  refresh();

}

void ClearLine(int y, int l) {
  move(y, 1);
  l++;
  char Str[l];
  for (int i = 0; i < l; i++) Str[i] = ' ';
  Str[l - 1] = '\0';
  mvaddstr(y, 0, Str);
}
void ClearLinePart(int y, int x, int l) {
  if(l>x){
    move(y, 1);
    l++;
    char Str[l];
    for (int i = x; i < l; i++) Str[i] = ' ';
    Str[l - 1] = '\0';
    mvaddstr(y, x, Str);
  }
}

void CatchSIG(int sig) {
  Terminated = true;
}

vector<Video> getVVec(vector<Channel> chanVec, int currID){
    
  vector<Video> vvec;
  vvec = chanVec[currChan].getVideoVector();
  return vvec;
    
}
int main(int argc, char *argv[]) {

	  	
      
  int c = 0;
  subParser parser;
  
  
  channelVector = parser.getChannelVector();
  

  vector<std::string> nameVector;
  vector<Video> vvec;
  signal(SIGINT, CatchSIG);
  
  initscr();
  keypad(stdscr, true);
  nonl(); 
  cbreak();
  noecho();   
  win = newwin(0, 0, 0, 0);
  
  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_WHITE,   COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLUE);
    init_pair(3, COLOR_BLACK,   COLOR_CYAN);
  }
  
  SelItem = 0;
  LastItem = 0;
  HeaderText = "  q:Quit  ";
  
  

        

  for (auto & element : channelVector) {
    nameVector.push_back(element.getChannelName());
  }



  
  currentAction = "chvex.size" + std::to_string(channelVector.size());

  while (!Terminated) {
    Process(nameVector[currChan], channelVector);
    usleep(1000);
  }

  cout << "Terminated" << endl;

  endwin();
}
