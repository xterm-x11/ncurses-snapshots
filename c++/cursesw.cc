// * this is for making emacs happy: -*-Mode: C++;-*-

/*
  Copyright (C) 1989 Free Software Foundation
  written by Eric Newton (newton@rocky.oswego.edu)

  This file is part of the GNU C++ Library.  This library is free
  software; you can redistribute it and/or modify it under the terms of
  the GNU Library General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your
  option) any later version.  This library is distributed in the hope
  that it will be useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the GNU Library General Public License for more details.
  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free Software
  Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

  modified by Ulrich Drepper  (drepper@karlsruhe.gmd.de)
          and Anatoly Ivasyuk (anatoly@nick.csh.rit.edu)
*/

#if !defined(lint)
static char vcid[] = "$Id: cursesw.cc,v 0.3.1.1 1995/05/10 20:30:52 tom Exp $";
#endif // !lint


#pragma implementation

#include "cursesw.h"


// declare static variables for the class
int NCursesWindow::count = 0;


int
NCursesWindow::scanw(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[BUFSIZ];
    int result = wgetstr(w, buf);
    if (result == OK) {
	strstreambuf ss(buf, BUFSIZ);
	result = ss.vscan(fmt, args);
    }
    va_end(args);
    return result;
}


int
NCursesWindow::scanw(int y, int x, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[BUFSIZ];
    int result = wmove(w, y, x);
    if (result == OK) {
	result = wgetstr(w, buf);
	if (result == OK) {
	    strstreambuf ss(buf, BUFSIZ);
	    result = ss.vscan(fmt, args);
	}
    }
    va_end(args);
    return result;
}


int
NCursesWindow::printw(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[BUFSIZ];
    vsprintf(buf, fmt, args);
    va_end(args);
    return waddstr(w, buf);
}


int
NCursesWindow::printw(int y, int x, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = wmove(w, y, x);
    if (result == OK) {
	char buf[BUFSIZ];
	vsprintf(buf, fmt, args);
	result = waddstr(w, buf);
    }
    va_end(args);
    return result;
}


void
NCursesWindow::init(void)
{
    noecho();
    cbreak();
    leaveok(0);
    keypad(1);
}


NCursesWindow::NCursesWindow(int lines, int cols, int begin_y, int begin_x)
{
    if (count==0)
	initscr();

    w = newwin(lines, cols, begin_y, begin_x);
    if (w == 0) {
	(*lib_error_handler)("NCursesWindow", "Cannot construct window");
    }
    init();

    alloced = 1;
    subwins = par = sib = 0;
    count++;
}

NCursesWindow::NCursesWindow(WINDOW* &window)
{
    if (count==0)
	initscr();

    w = window;
    init();
    alloced = 0;
    subwins = par = sib = 0;
    count++;
}


NCursesWindow::NCursesWindow(NCursesWindow& win, int l, int c,
			     int by, int bx, char absrel)
{
    if (absrel == 'r') { // relative origin 
	by += win.begy();
	bx += win.begx();
    }

    // Even though we treat subwindows as a tree, the standard curses
    // library needs the `subwin' call to link to the root in
    // order to correctly perform refreshes, etc.

    NCursesWindow* root = &win;
    while (root->par != 0) root = root->par;

    w = subwin(root->w, l, c, by, bx);
    if (w == 0) {
	(*lib_error_handler)("NCursesWindow", "Cannot construct subwindow");
    }

    par = &win;
    sib = win.subwins;
    win.subwins = this;
    subwins = 0;
    alloced = 1;
    count++;
}


void
NCursesWindow::kill_subwindows()
{
    for (NCursesWindow* p = subwins; p != 0; p = p->sib) {
	p->kill_subwindows();
	if (p->alloced) {
	    if (p->w != 0)
		::delwin(p->w);
	    p->alloced = 0;
	}
	p->w = 0; // cause a run-time error if anyone attempts to use...
    }
}


NCursesWindow::~NCursesWindow()
{
    kill_subwindows();

    if (par != 0) {  // Snip us from the parent's list of subwindows.
	NCursesWindow * win = par->subwins;
	NCursesWindow * trail = 0;
	for (;;) {
	    if (win == 0)
		break;
	    else if (win == this) {
		if (trail != 0)
		    trail->sib = win->sib;
		else
		    par->subwins = win->sib;
		break;
	    } else {
		trail = win;
		win = win->sib;
	    }
	}
    }

    if (alloced && w != 0)
	delwin(w);

    --count;
    if (count == 0)
	endwin();
    else if (count < 0) { // cannot happen!
	(*lib_error_handler)("NCursesWindow", "Too many windows destroyed");
    }
}


int NCursesColorWindow::colorInitialized = 0;


void
NCursesColorWindow::colorInit(void)
{
    if (colorInitialized == 0) {
	start_color();
	colorInitialized = 1;
    }
}


NCursesColorWindow::NCursesColorWindow(WINDOW* &window)
: NCursesWindow(window)
{
    colorInit();
}


NCursesColorWindow::NCursesColorWindow(int lines, int cols, 
                                       int begin_y, int begin_x)
: NCursesWindow(lines, cols, begin_y, begin_x)
{
    colorInit();
}


NCursesColorWindow::NCursesColorWindow(NCursesWindow& par,
		                       int lines, int cols,
		                       int by, int bx, char absrel)
: NCursesWindow(par, lines, cols, by, bx, absrel) 
{
    colorInit();
}


short
NCursesColorWindow::getcolor(int getback) const 
{
    short fore, back;

    if (pair_content(PAIR_NUMBER(w->_attrs), &fore, &back))
	return ERR;

    return getback ? back : fore;
}


int
NCursesColorWindow::setpalette(short fore, short back, short pair)
{
    return init_pair(pair, fore, back);
}


int
NCursesColorWindow::setcolor(short pair)
{
    if ((pair < 1) || (pair > COLOR_PAIRS))
	return ERR;

    attroff(A_COLOR);
    attrset(COLOR_PAIR(pair));

    return OK;
}


