#include <curses.h>

int
main()
{
WINDOW *win1, *win2;
int h, i;

	initscr();
	cbreak();
	noecho();
	win1 = newwin(20, 50, 10, 20);
	for (h = 0; h < 20; h++)
		for (i = 0; i < 50; i++)
			mvwaddch(win1, h, i, 'X');
	wrefresh(win1);
	getch();

	win2 = newwin(20, 50, 15, 30);
	for (h = 0; h < 20; h++)
		for (i = 0; i < 50; i++)
			mvwaddch(win2, h, i, 'Y');
	wnoutrefresh(win1);
	wnoutrefresh(win2);
	doupdate();
	getch();

	/* now, remove window 2 and restore the contents of window 1 */
	i = copywin(win1, win2, 5, 10, 0, 0, 14, 39, 0);   
	wnoutrefresh(win1);
	wnoutrefresh(win2);
	printw("copywin returns %d\n", i);
	wnoutrefresh(stdscr);
	doupdate();

	getch();

	endwin();
	return 0;
}

