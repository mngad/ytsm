#include <iostream>
#include <signal.h>
#include <ncurses.h>
#include <menu.h>
#include <string>
#include <unistd.h>

#include "subParser.h"
#include "Video.h"
#include "Channel.h"

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
int maxWinType =1;
std::vector<Channel> channelVector;

void openVideo(std::string url);
int processPageView(std::string ch, vector<Channel> chvec);
void ClearLine(int y, int l);
vector<Video> getVVec(vector<Channel> chanVec, int currID);
int processIndividualMode(vector<Channel> chvec);
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
    changeScreen();
  }
  if (LastKey == UKEY){
    subParser p;
    channelVector.clear();
    channelVector = p.updateGetChannelVector();

  }
  switch (Screen) {
    default: 
    if (windowtype==0){
      processPageView(ch, chvec);
    }
    if(windowtype==1){
      processIndividualMode(chvec);
    }
  }

  if (LastKey == KEY_q) {
    Terminated = true;
  }
  return 0;
}

int processPageView(std::string ch,  vector<Channel> chvec) {
    vector<Video> v = getVVec(chvec,currChan);
  LastItem = v.size();
  
  attrset(COLOR_PAIR(1));
  for (int i = 0; i <= MaxY; i++) ClearLine(i, MaxX);

  // draw header
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(0, MaxX);
  mvaddstr(0, 0, HeaderText);
  mvaddstr(0, 1, ch.c_str());
  
  // draw body
  attrset(COLOR_PAIR(1));

  for (int i = 0; i < LastItem; i++) {
    if (SelItem == i) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    //cout<<vvec[i].getVideoTitle()<<endl;
    mvaddstr(i +1, 0, v[i].getVideoTitle().c_str());
    //mvaddstr((2*i)+2, 0, vvec[i].getVideoUrl().c_str());
    //cout<<vvec.size()<<endl;
    //cout<<i<<endl;
  }
    //cout<<"here"<<endl;
  // draw status line
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(MaxY - 2, MaxX);
  mvaddstr(MaxY - 2, 0, currentAction.c_str());
  //mvaddstr(MaxY -4,0,std::to_string(getch()).c_str());

  curs_set(0);
  refresh();
  
  LastKey = getch();
  
  if (LastKey == KEY_UP) {
      SelItem=SelItem-1;
      //cout<<CurrentItem<<endl;
    
        if(CurrentItem>0)CurrentItem--;

  }
  if (LastKey == KEY_DOWN) {
      SelItem= SelItem+1;

      if (CurrentItem<LastItem)CurrentItem++;
  }
  if (LastKey == KEY_ENTER){
     
        std::string url = v[CurrentItem].getVideoUrl();
    currentAction = "opening " + url;
    openVideo(url);
  }
  if(LastKey == NEXT){
      if(currChan +1 <chvec.size())currChan++;
      ClearLine(2,MaxY);
  }
  if(LastKey == PREV){
      if(currChan -1 >=0)currChan--;
            ClearLine(2,MaxY);
  }


  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}

int processIndividualMode(vector<Channel> chvec) {
    vector<Video> v = getVVec(chvec,currChan);
  LastItem = chvec.size();
  
  
  attrset(COLOR_PAIR(1));
  for (int i = 0; i <= MaxY; i++) ClearLine(i, MaxX);

  // draw header
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(0, MaxX);
  mvaddstr(0, 0, HeaderText);
  mvaddstr(0, 1, "All Channels");
  
  // draw body
  attrset(COLOR_PAIR(1));

  for (int i = 0; i < LastItem; i++) {
    if (SelItem == i) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    
    mvaddstr(i +1, 0, chvec[i].getChannelName().c_str());

  }

  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(MaxY - 2, MaxX);
  mvaddstr(MaxY - 2, 0, currentAction.c_str());
  

  curs_set(0);
  refresh();
  
  LastKey = getch();
  
  if (LastKey == KEY_UP) {
      SelItem=SelItem-1;
      
        if(CurrentItem>0)CurrentItem--;

  }
  if (LastKey == KEY_DOWN) {
      SelItem= SelItem+1;

      if (CurrentItem<LastItem)CurrentItem++;
  }
  if (LastKey == KEY_ENTER){
     
    currChan = CurrentItem;
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