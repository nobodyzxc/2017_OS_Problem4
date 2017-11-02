#ifndef UI_H
#define UI_H
void tui_init();
void tui_exit(int);
void tui_log(const char *s);
char *tui_input(char *s);
void progress(int row , float cur , float quo);
#endif
