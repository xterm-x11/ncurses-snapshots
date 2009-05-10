/****************************************************************************
 * Copyright (c) 1998-2005,2009 Free Software Foundation, Inc.              *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Thomas E. Dickey          1998-on                               *
 *          Juergen Pfeifer           2009                                  *
 ****************************************************************************/

#include <curses.priv.h>
#include <term.h>

#ifndef CUR
#define CUR SP_TERMTYPE 
#endif

MODULE_ID("$Id: lib_dft_fgbg.c,v 1.21 2009/05/10 00:48:29 tom Exp $")

/*
 * Modify the behavior of color-pair 0 so that the library doesn't assume that
 * it is white on black.  This is an extension to XSI curses.
 */
NCURSES_EXPORT(int)
NCURSES_SP_NAME(use_default_colors) (NCURSES_SP_DCL0)
{
    T((T_CALLED("use_default_colors()")));
    returnCode(NCURSES_SP_NAME(assume_default_colors) (NCURSES_SP_ARGx -1, -1));
}

#if NCURSES_SP_FUNCS
NCURSES_EXPORT(int)
use_default_colors(void)
{
    return NCURSES_SP_NAME(use_default_colors) (CURRENT_SCREEN);
}
#endif

/*
 * Modify the behavior of color-pair 0 so that the library assumes that it
 * is something specific, possibly not white on black.
 */
NCURSES_EXPORT(int)
NCURSES_SP_NAME(assume_default_colors) (NCURSES_SP_DCLx int fg, int bg)
{
    T((T_CALLED("assume_default_colors(%d,%d)"), fg, bg));

    if (!orig_pair && !orig_colors)
	returnCode(ERR);

    if (initialize_pair)	/* don't know how to handle this */
	returnCode(ERR);

    SP_PARM->_default_color = isDefaultColor(fg) || isDefaultColor(bg);
    SP_PARM->_has_sgr_39_49 = (tigetflag("AX") == TRUE);
    SP_PARM->_default_fg = isDefaultColor(fg) ? COLOR_DEFAULT : (fg & C_MASK);
    SP_PARM->_default_bg = isDefaultColor(bg) ? COLOR_DEFAULT : (bg & C_MASK);
    if (SP_PARM->_color_pairs != 0) {
	bool save = SP_PARM->_default_color;
	SP_PARM->_default_color = TRUE;
	init_pair(0, (short) fg, (short) bg);
	SP_PARM->_default_color = save;
    }
    returnCode(OK);
}

#if NCURSES_SP_FUNCS
NCURSES_EXPORT(int)
assume_default_colors(int fg, int bg)
{
    return NCURSES_SP_NAME(assume_default_colors) (CURRENT_SCREEN, fg, bg);
}
#endif
