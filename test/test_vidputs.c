/****************************************************************************
 * Copyright 2020-2022,2024 Thomas E. Dickey                                *
 * Copyright 2013-2014,2017 Free Software Foundation, Inc.                  *
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
/*
 * $Id: test_vidputs.c,v 1.16 2024/10/06 21:17:54 tom Exp $
 *
 * Demonstrate the vidputs and vidattr functions.
 * Thomas Dickey - 2013/01/12
 */

#define USE_TINFO
#include <test.priv.h>

#if HAVE_SETUPTERM && HAVE_VIDPUTS

static FILE *my_fp;
static bool p_opt = FALSE;

static
TPUTS_PROTO(outc, c)
{
    int rc;

    rc = putc(c, my_fp);
    TPUTS_RETURN(rc);
}

static bool
outs(NCURSES_CONST char *s)
{
    if (VALID_STRING(s)) {
	tputs(s, 1, outc);
	return TRUE;
    }
    return FALSE;
}

static void
cleanup(void)
{
    if (cur_term != 0) {
	outs(exit_attribute_mode);
	if (!outs(orig_colors))
	    outs(orig_pair);
	outs(cursor_normal);
    }
}

static void
change_attr(chtype attr)
{
    if (p_opt) {
	vidputs(attr, outc);
    } else {
	vidattr(attr);
    }
}

static void
test_vidputs(void)
{
    fprintf(my_fp, "Name: ");
    change_attr(A_BOLD);
    fputs("Bold", my_fp);
    change_attr(A_REVERSE);
    fputs(" Reverse", my_fp);
    change_attr(A_NORMAL);
    fputs("\n", my_fp);
}

static void
usage(int ok)
{
    static const char *tbl[] =
    {
	"Usage: test_vidputs [options]"
	,""
	,USAGE_COMMON
	,"Options:"
	," -e       use stderr (default stdout)"
	," -n       do not initialize terminal"
	," -p       use vidputs (default vidattr)"
    };
    unsigned n;
    for (n = 0; n < SIZEOF(tbl); ++n)
	fprintf(stderr, "%s\n", tbl[n]);
    ExitProgram(ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
/* *INDENT-OFF* */
VERSION_COMMON()
/* *INDENT-ON* */

int
main(int argc, char *argv[])
{
    int ch;
    bool no_init = FALSE;

    my_fp = stdout;

    while ((ch = getopt(argc, argv, OPTS_COMMON "enp")) != -1) {
	switch (ch) {
	case 'e':
	    my_fp = stderr;
	    break;
	case 'n':
	    no_init = TRUE;
	    break;
	case 'p':
	    p_opt = TRUE;
	    break;
	case OPTS_VERSION:
	    show_version(argv);
	    ExitProgram(EXIT_SUCCESS);
	default:
	    usage(ch == OPTS_USAGE);
	    /* NOTREACHED */
	}
    }
    if (optind < argc)
	usage(FALSE);

    if (no_init) {
	START_TRACE();
    } else {
	setupterm((char *) 0, fileno(my_fp), (int *) 0);
    }
    test_vidputs();
    cleanup();
    ExitProgram(EXIT_SUCCESS);
}

#else
int
main(void)
{
    fprintf(stderr, "This program requires terminfo\n");
    exit(EXIT_FAILURE);
}
#endif
