#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define CLOCK_TICKS_PER_SECOND sysconf(_SC_CLK_TCK)
#define PAGE_SIZE getpagesize()
#define MINIMUM_INTERVAL 1
#define DEFAULT_INTERVAL 5

// Refresh interval in seconds
int refreshInterval = 3;

// Ctrl+C start handler
int execute = 1;
void trap1(int signal){
    execute = 0;
}

// Ctrl+C input handler
void trap2(int signal)
{
    system("clear");
    exit(0);
}