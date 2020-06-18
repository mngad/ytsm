#include <iostream>
#include <signal.h>
#include <ncurses.h>
#include <curses.h>
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
#define ENTER 13
#define NEXT 93
#define PREV 91
#define MKEY 109
#define UKEY 117
#define AKEY 97


char *MenuItems[1024];
int SelItem = 0;
int LastItem = 0;
int SelItemS = 0;
int shiftS = 0;
int SelItemA = 0;
int shiftA = 0;
WINDOW *win;
bool Terminated;
unsigned int Screen = 0;
unsigned int LastKey;
unsigned int MaxX = 0;
unsigned int MaxY = 0;
unsigned int CurrentItem =0;
std::string currentAction;
unsigned int currChan = 0;
unsigned int windowtype =2;
unsigned int maxWinType =2;
bool updating = false;
long unsigned int shift = 0;
subParser p;
std::vector<Channel> channelVector;

void openVideo(std::string url);
int processPageView(vector<Channel> chvec);
void ClearLine(int y, int l);
void ClearLinePart(int y, int x, int l);
vector<Video> getVVec(vector<Channel> chanVec, int currID);
int processSubListMode(vector<Channel> chvec);
int processAllMode(vector<Channel> chvec);
int processKeys(vector<Channel> chvec);


void shiftFactor(int dir){
  if(shift >= 0){
    if((SelItem-shift > MaxY-4)&&(dir<0))shift+=(abs(dir));//down
    if((SelItem < shift)&&(dir>0))shift-=(abs(dir));//up
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

  if (LastKey == 's'){
    windowtype = 1;
    SelItem =SelItemS;
    shift = shiftS;
  }
  if (LastKey == 'a'){
    windowtype =2;
    SelItem =SelItemA;
    shift =shiftA;
  }
  if (LastKey == '?'){
    windowtype =3;
    SelItem =0;
    shift =0;
  }
  if (LastKey == UKEY){
    currentAction ="Updating...";
    update(ch);
    windowtype =2;
    SelItem =0;
    shift =0;

  }
  switch (Screen) {
  default:
    if (windowtype==0){
      processPageView(chvec);
    }
    if(windowtype==1){
      processSubListMode(chvec);
    }
    if(windowtype==2){
      processAllMode(chvec);
    }
    if(windowtype==3){
      processKeys(chvec);
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
  for (unsigned int i = 0; i <= MaxY; i++) ClearLine(i, MaxX);

  // draw header
  attrset(A_BOLD|COLOR_PAIR(2));
  ClearLine(0, MaxX);
  //mvaddstr(0, 0, HeaderText);
  mvaddstr(0, 1, title.c_str());
  mvaddstr(0, MaxX-20, p.getUpdatedTime().c_str());
  // draw body
  attrset(COLOR_PAIR(1));
  return 0;
}

int processGenericPost(){
  currentAction = (to_string(SelItem) + " " + to_string(shift)).c_str();
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
  if (LastKey == KEY_PPAGE) {
      if(SelItem -(MaxY/2) <LastItem) {
        SelItem= SelItem-(MaxY/2);
       shiftFactor(+(MaxY/2));
      }
    }
    if (LastKey == KEY_NPAGE) {
        if(SelItem +(MaxY/2) <LastItem) {
          SelItem= SelItem+(MaxY/2);
         shiftFactor(-(MaxY/2));
        }
      }
  return 0;

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
  SelItemA = SelItem; //Save the state when returning
  shiftA=shift; //Save the state when returning
  currentAction = v[SelItem].getVideoUrl();
  if (LastKey == ENTER){

    std::string url = v[SelItem].getVideoUrl();
    currentAction = "opening " + url;
    openVideo(url);
  }
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}

int processPageView(vector<Channel> chvec) {

  //cout<<"here"<<endl;
  vector<Video> v = getVVec(chvec,currChan);
  //cout<<to_string(currChan)<<endl;
  std::string title = chvec[currChan].getChannelName();
  //cout<<"here2"<<endl;
  processGenericPre(v, title);
  //cout<<"here3"<<endl;
  int size;
  if(v.size()>MaxY)size = MaxY-3;
  else size = LastItem;
  //cout<<"here4"<<endl;
  for (int i = 0; i < size; i++) {
    if (SelItem == i+shift) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    //cout<<"here5"<<endl;
    mvaddstr(i +1, 0, (v[i+shift].getVideoTitle()+
        fillIn(v[i+shift].getVideoTitle().length(),MaxX-18)).c_str());
    mvaddstr(i+1, MaxX-18, " |");

    mvaddstr(i +1, MaxX-16, p.normaliseDate(v[i+shift].getVideoDate()).c_str());
  }
  //cout<<"here6"<<endl;



  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  processGenericPost();
  currentAction = v[SelItem].getVideoUrl();

  if (LastKey == ENTER){
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

int processKeys(vector<Channel> chvec) {
  vector<Video> v = getVVec(chvec,currChan);

  vector<std::string> keys = {"a    | View All Channels Videos",
  "s    | View list of Channels",
  "[    | Previous Channel",
  "]    | Next Channel",
  "u    | Update Feed",
  "?    | View this list",
  "q    | Quit"};
  processGenericPre(v, "Keys");
  int size;
  //if(v.size()>MaxY)size = MaxY-3;
  size = keys.size();
  LastItem = size;
  for (int i = 0; i < size; i++) {
    if (SelItem == i+shift) {
      attrset(COLOR_PAIR(3));
    } else {
      attrset(COLOR_PAIR(1));
    }
    mvaddstr(i +1, 0, (keys[i+shift]).c_str());
    //mvaddstr(i +2, 0, (fillIn(0,MaxX-10)).c_str());


  }

  processGenericPost();

  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;
}

int processSubListMode(vector<Channel> chvec) {

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
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  processGenericPost();
  SelItemS = SelItem; //Save the state when returning
  shiftS=shift; //Save the state when returning
  if (LastKey == ENTER){

    shift =0; //scroll of chanViewer to 0
    currChan = SelItem;
    windowtype = 0; //Go to chanViewer
    SelItem = 0; //top of chanViewer


  }
  if (SelItem >= LastItem) SelItem = LastItem;
  if (SelItem < 0) SelItem = 0;
  return 0;


}



void openVideo(std::string url){
  std::string callFunc = "mpv --no-terminal " + url + " &";
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





  for (auto & element : channelVector) {
    nameVector.push_back(element.getChannelName());
  }




  currentAction = "chvex.size" + std::to_string(channelVector.size());

  while (!Terminated) {
    Process(nameVector[currChan], channelVector);
    usleep(10000);
  }

  cout << "Terminated" << endl;

  endwin();
}
