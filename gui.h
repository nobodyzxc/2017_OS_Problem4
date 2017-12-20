#ifndef UI_H
#define UI_H
void gui_init();
void gui_exit(int);
void gui_log(const char *s);
char *gui_input(char *s);
void gui_display(int row , float cur , float quo);

#define UIInit gui_init
#define UIExit gui_exit
#define UIDisp gui_display
#define UILog  gui_log
#define INPUT(s) fgets(s , sizeof(s), stdin)

#endif
