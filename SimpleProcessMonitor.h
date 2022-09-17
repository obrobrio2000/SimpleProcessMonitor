#include <stdio.h>  // for printf(), scanf() and a lot of other things...
#include <stdlib.h> // for exit(), system(), atoi()
#include <unistd.h> // for sysconf(), getpagesize(), sleep()
#include <signal.h> // for signal()
#include <string.h> // for strcmp(), strncmp(), strstr(), strlen(), strcpy(), strcspn()
#include <dirent.h> // for DIR, opendir(), readdir(), closedir()
#include <ctype.h>  // for isdigit()
#include <pwd.h>    // for getpwuid()
#include <grp.h>    // for getgrgid()

#define CLOCK_TICKS_PER_SECOND sysconf(_SC_CLK_TCK)
#define PAGE_SIZE getpagesize()
#define MINIMUM_INTERVAL 1
#define DEFAULT_INTERVAL 3

void trap1(int signal);
void trap2(int signal);
int compare(const void *a, const void *b);
void inputHighlight();
void inputInterval();
void inputSort();
void inputAction(int action);
void input();
void monitor();
