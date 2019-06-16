#include <stdio.h>
#include <ncurses.h>
#include "dlxml.h"
#include "feed.h"
#include "Item.h"
#include <signal.h>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>



WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
std::vector<Feed> feedVector;

std::string replaceString(std::string str){
    
    boost::replace_all(str, "’", "'");
    boost::replace_all(str, "‘", "'");
    boost::replace_all(str, "&quot;", "'");
    boost::replace_all(str, "&apos;", "'");
    return str;

}

void printFeed(WINDOW *childwin, int feedNum, int startscr){

	box(childwin, 0, 0);
    feedVector[feedNum].SortDate();
    mvwaddstr(childwin, 1, COLS/2 - feedVector[feedNum].GetName().length(), feedVector[feedNum].GetName().c_str());
    int count = startscr +3;

    for(Item item : feedVector[feedNum].GetItemArray()){

        if(feedNum == 0){
            mvwaddstr(childwin, count, 2, item.GetFeedTitle().c_str());
            count +=1;

        }
    	mvwaddstr(childwin, count, 2, replaceString(item.GetTitle()).c_str());
    	mvwaddstr(childwin, count+1, 2, item.GetUrl().c_str());
        if(item.GetUrl().length()>COLS -5){
            mvwaddstr(childwin, count+3, 2, item.GetDate().c_str());
            count += 5;
        }
        else{
        	mvwaddstr(childwin, count+2, 2, item.GetDate().c_str());
            count += 4;
        }

    	//mvwaddstr(childwin, count + 1, 2, item.GetDescr().c_str());
    	
    }


}
void updateFD(WINDOW *mainwin, WINDOW *childwin, int feednum, int scrNum){

	wclear(mainwin);
	printFeed(childwin, feednum, scrNum);
	refresh();
}
void resizeHandler(WINDOW *mainwin, WINDOW *childwin, int feednum) {
    
	updateFD(mainwin,childwin,feednum,0);
}

int main()
{
	Dlxml dlxml;

	feedVector = dlxml.init();
    
    std::cout<<"here222"<<std::endl;

	//std::vector<Feed> fe =  dlxml.getFeedVec();

	
    WINDOW * mainwin, * childwin;
    std::string      ch;

    if ( (mainwin = initscr()) == NULL ) {
	fprintf(stderr, "Error initialising ncurses.\n");
	exit(EXIT_FAILURE);
    }
    

  

    /*  Switch of echoing and enable keypad (for arrow keys)  */

    noecho();
    keypad(mainwin, TRUE);


    /*  Make our child window, and add
	a border and some text to it.   */

	int      width = COLS -2, height = LINES -2;
    int      rows  = LINES, cols   = COLS;
    int      x = (cols - width)  / 2;
    int      y = (rows - height) / 2;

	childwin = subwin(mainwin, height, width, y, x);
    int feednum = 0;
    
	printFeed(childwin, feednum,0);
    refresh();

	//vwaddstr(my_win,startx, starty, "feedVector[0].GetName().c_str()");
	//addstr(feedVector[0].GetName().c_str());

	//signal(SIGWINCH, resizeHandler(mainwin, childwin, feednum));
    ch = " ";
    int scrnum = 0;
    while(ch != "q"){
    	ch = getch();

		if(ch == "]" && (feednum + 1)<=feedVector.size()-1){
            scrnum=0;

			feednum++;
            // dlxml.update()
			updateFD(mainwin,childwin,feednum, 0);

		}
		if(ch=="[" && (feednum-1)>=0){
            scrnum=0;

			feednum--;
            // dlxml.update();
			updateFD(mainwin,childwin,feednum, 0);
			
		}
        if(ch=="u" ){
            scrnum=0;
            dlxml.update();
            int      width = COLS -2, height = LINES -2;
            int      rows  = LINES, cols   = COLS;
            int      x = (cols - width)  / 2;
            int      y = (rows - height) / 2;

            childwin = subwin(mainwin, height, width, y, x);
            updateFD(mainwin,childwin,feednum,0);
            
        }
        if(ch=="k" ){
            scrnum-=1;
            updateFD(mainwin,childwin,feednum, scrnum);
            
        }
         if(ch=="j" ){
            if(scrnum<=0){
                scrnum+=1;
                updateFD(mainwin,childwin,feednum, scrnum);
            }
            
        }
         if(ch=="K" ){
            scrnum-=height;
            updateFD(mainwin,childwin,feednum, scrnum);
            
        }
         if(ch=="J" ){
            if(scrnum<=0){
                scrnum+=height;
                updateFD(mainwin,childwin,feednum, scrnum);
            }
            
        }

		
	   /*  Clean up after ourselves  */


    }

    delwin(childwin);
    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
	}