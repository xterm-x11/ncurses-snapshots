
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


/*
**	lib_overlay.c
**
**	The routines overlay(), copywin(), and overwrite().
**
*/

#include "curses.priv.h"

static void overlap(WINDOW *const s, WINDOW *d, int flag)
{ 
int sminrow, smincol, dminrow, dmincol, dmaxrow, dmaxcol;

	T(("overlap : sby %d, sbx %d, smy %d, smx %d, dby %d, dbx %d, dmy %d, dmx %d",
		s->_begy, s->_begx, s->_maxy, s->_maxx, 
		d->_begy, d->_begx, d->_maxy, d->_maxx));
	sminrow = max(s->_begy, d->_begy) - s->_begy;
	smincol = max(s->_begx, d->_begx) - s->_begx;
	dminrow = max(s->_begy, d->_begy) - d->_begy;
	dmincol = max(s->_begx, d->_begx) - d->_begx;
	dmaxrow = min(s->_maxy+s->_begy, d->_maxy+d->_begy) - d->_begy;
	dmaxcol = min(s->_maxx+s->_begx, d->_maxx+d->_begx) - d->_begx;

	copywin(s, d, sminrow, smincol, dminrow, dmincol, dmaxrow, dmaxcol, flag);
}

/*
**
**	overlay(win1, win2)
**
**
**	overlay() writes the overlapping area of win1 behind win2
**	on win2 non-destructively.
**
**/

int overlay(WINDOW *const win1, WINDOW *win2)
{
	overlap(win1, win2, TRUE);
	return OK;
}

/*
**
**	overwrite(win1, win2)
**
**
**	overwrite() writes the overlapping area of win1 behind win2
**	on win2 destructively.
**
**/

int overwrite(WINDOW *win1, WINDOW *win2)
{
	overlap(win1, win2, FALSE);
	return OK;
}

int copywin(WINDOW *src, WINDOW *dst, 
	int sminrow, int smincol,
	int dminrow, int dmincol, int dmaxrow, int dmaxcol, 
	int over)
{
int sx, sy, dx, dy;

	T(("copywin(%p, %p, %d, %d, %d, %d, %d, %d, %d)",
	    	src, dst, sminrow, smincol, dminrow, dmincol, dmaxrow, dmaxcol, over));
	
	/* make sure rectangle exists in source */
	if ((sminrow + dmaxrow - dminrow) > (src->_maxy + 1) ||
	    (smincol + dmaxcol - dmincol) > (src->_maxx + 1)) {
		return ERR;
	}

	T(("rectangle exists in source"));

	/* make sure rectangle fits in destination */
	if (dmaxrow > dst->_maxy || dmaxcol > dst->_maxx) {
		return ERR;
	}

	T(("rectangle fits in destination"));

	for (dy = dminrow, sy = sminrow; dy <= dmaxrow; sy++, dy++) {
		dst->_line[dy].firstchar = dmincol;
		dst->_line[dy].lastchar = dmincol;
		for (dx = dmincol, sx = smincol; dx <= dmaxcol; sx++, dx++) {
			if (over == TRUE ) {
				if (((src->_line[sy].text[sx] & A_CHARTEXT) != ' ') && (dst->_line[dy].text[dx] != src->_line[sy].text[sx]))  {	
					dst->_line[dy].text[dx] = src->_line[sy].text[sx];
					dst->_line[dy].lastchar = dx;
				} else
					dst->_line[dy].firstchar++;
			} else {
				if (dst->_line[dy].text[dx] != src->_line[sy].text[sx]) {  	
					dst->_line[dy].text[dx] = src->_line[sy].text[sx];
					dst->_line[dy].lastchar = dx;
				} else
					dst->_line[dy].firstchar++;
			}
		}
	}
	T(("finished copywin"));
	return OK;
}
