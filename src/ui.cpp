#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <algorithm>
#include <string>
#include <signal.h>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>


WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

void updateFD(WINDOW *mainwin, WINDOW *childwin, int feednum, int scrNum){

	wclear(mainwin);
	refresh();
}
void resizeHandler(WINDOW *mainwin, WINDOW *childwin, int feednum) {
    
	updateFD(mainwin,childwin,feednum,0);
}

int main()
{

	
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
    
    refresh();

	//vwaddstr(my_win,startx, starty, "feedVector[0].GetName().c_str()");
	//addstr(feedVector[0].GetName().c_str());

	//signal(SIGWINCH, resizeHandler(mainwin, childwin, feednum));
    ch = " ";
    int scrnum = 0;
    while(ch != "q"){
    	ch = getch();


    }

    delwin(childwin);
    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
	}
