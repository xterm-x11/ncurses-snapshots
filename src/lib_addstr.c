/***************************************************************************
*                            COPYRIGHT NOTICE                              *
****************************************************************************
*                ncurses is copyright (C) 1992-1995                        *
*                          Zeyd M. Ben-Halim                               *
*                          zmbenhal@netcom.com                             *
*                          Eric S. Raymond                                 *
*                          esr@snark.thyrsus.com                           *
*                                                                          *
*        Permission is hereby granted to reproduce and distribute ncurses  *
*        by any means and for any fee, whether alone or as part of a       *
*        larger distribution, in source or in binary form, PROVIDED        *
*        this notice is included with any such distribution, and is not    *
*        removed from any of its header files. Mention of ncurses in any   *
*        applications linked with it is highly appreciated.                *
*                                                                          *
*        ncurses comes AS IS with no warranty, implied or expressed.       *
*                                                                          *
***************************************************************************/

#include "system.h"

/*
**	lib_addstr.c
*
**	The routines waddnstr(), waddchnstr().
**
*/

#include "curses.priv.h"
#include <string.h>

int
waddnstr(WINDOW *win, char *const astr, int n)
{
unsigned char *str = (unsigned char *)astr;
int code = ERR;

	T(("waddnstr(%p,\"%s\",%d) called %s", win, visbuf(astr), n, _traceattr(win->_attrs)));

	if (str != NULL) {

		TR(TRACE_VIRTPUT, ("str is not null"));
		code = OK;
		if (n < 0)
			n = strlen(str);

		while((n-- > 0) && (*str != '\0')) {
			TR(TRACE_VIRTPUT, ("*str = %x", *str));
			if (waddch(win, (unsigned char)*str++) == ERR) {
				code = ERR;
				break;
			}
		}
	}
	TR(TRACE_VIRTPUT, ("waddnstr returns %d", code));
	return code;
}

int
waddchnstr(WINDOW *win, chtype *const astr, int n)
{
int oy = win->_cury;
int ox = win->_curx;
chtype *str = astr;
int code = OK;

	T(("waddchnstr(%p,%p,%d) called", win, str, n));

	if (n < 0) {
		n = 0;
		while (*str++ != 0)
			n++;
		str = astr;
	}

	while(n-- > 0) {
		if (waddch(win, *str++) == ERR) {
			code = ERR;
			break;
		}
	}

	win->_curx = ox;
	win->_cury = oy;
	return code;
}
