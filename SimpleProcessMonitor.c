// SimpleProcessMonitor by Giovanni Magliocchetti
// Allows to terminate, kill, suspend and resume running processes.
// The processes are accessed via the /proc filesystem.
// The program monitors the resources used by each process (CPU usage / Memory usage).

#include "SimpleProcessMonitor.h"

// struct of the process (PID, PPID, USER, GROUP, PR, NI, state, %CPU, %MEM, TIME+, VIRT, RES, SHR, COMMAND)
struct process
{
    int pid;
    int ppid;
    char user[10];
    char group[10];
    long int pr;
    long int ni;
    char state;
    float cpu;
    float mem;
    char time[10];
    long int virt;
    long int res;
    long int shr;
    char command[100];
};

// Refresh interval in seconds
int refreshInterval = DEFAULT_INTERVAL;

// Highlighted PID
int highlightedPID = 0;

// Ctrl+C monitor handler
int execute = 1;
void trap1(int signal)
{
    execute = 0;
}

// Ctrl+C input handler
void trap2(int signal)
{
    system("clear");
    exit(0);
}

// sort the processes by pid, cpu or mem
int sort_by = 1; // 0 = PID, 1 = CPU, 2 = MEM

// compare function for qsort based on the sort_by variable
int compare(const void *a, const void *b)
{
    struct process *ia = (struct process *)a;
    struct process *ib = (struct process *)b;
    if (sort_by == 1)
    {
        if (ia->cpu < ib->cpu)
            return 1;
        else if (ia->cpu > ib->cpu)
            return -1;
        else
            return 0;
    }
    else if (sort_by == 2)
    {
        if (ia->mem < ib->mem)
            return 1;
        else if (ia->mem > ib->mem)
            return -1;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}

void inputHighlight()
{
    // print yellow
    printf("\033[1;93m");

    printf("\nEnter PID: ");

    // print reset
    printf("\033[0m");

    char pid[7];
    scanf("%s", pid);

    // iterate pid and check if it is a number or space or newline
    for (int i = 0; i < strlen(pid); i++)
    {
        if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n' && pid[i] != '\0')
        {
            // print red
            printf("\033[1;91m");

            printf("\nError: invalid PID!\n");

            // print reset
            printf("\033[0m");

            inputHighlight();
        }

        // set pid to be highlighted
        highlightedPID = atoi(pid);
    }
}

void inputInterval()
{
    // print yellow
    printf("\033[1;93m");

    printf("\nEnter refresh interval: ");

    // print reset
    printf("\033[0m");

    char interval[5];
    scanf("%s", interval);

    for (int i = 0; i < strlen(interval); i++)
    {
        if (!isdigit(interval[i]) && interval[i] != ' ' && interval[i] != '\n' && interval[i] != '\0')
        {
            // print red
            printf("\033[1;91m");

            printf("\nError: invalid interval!\n");

            // print reset
            printf("\033[0m");

            inputInterval();
        }
    }

    // if interval is "default", set interval to DEFAULT_INTERVAL
    if (strcmp(interval, "default") == 0)
    {
        refreshInterval = DEFAULT_INTERVAL;
    }

    // if interval is 0, set interval to MINIMUM_INTERVAL
    if (atoi(interval) == 0)
    {
        refreshInterval = MINIMUM_INTERVAL;
    }
    else
    {
        refreshInterval = atoi(interval);
    }
}

void inputSort()
{
    // print yellow
    printf("\033[1;93m");

    printf("\nSort by: ");

    // print reset
    printf("\033[0m");

    char sort[5];
    scanf("%s", sort);

    if (strcmp(sort, "pid") == 0)
    {
        sort_by = 0;
    }
    else if (strcmp(sort, "cpu") == 0)
    {
        sort_by = 1;
    }
    else if (strcmp(sort, "mem") == 0)
    {
        sort_by = 2;
    }
    else
    {
        // print red
        printf("\033[1;91m");

        printf("\nError: invalid sort!\n");

        // print reset
        printf("\033[0m");

        inputSort();
    }
}

void inputAction(int action)
{
    // print yellow
    printf("\033[1;93m");

    printf("\nEnter PID: ");

    // print reset
    printf("\033[0m");

    char pid[7];
    scanf("%s", pid);

    // iterate pid and check if it is a number or space or newline
    for (int i = 0; i < strlen(pid); i++)
    {
        if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n' && pid[i] != '\0')
        {
            // print red
            printf("\033[1;91m");

            printf("\nError: invalid PID!\n");

            // print reset
            printf("\033[0m");

            inputAction(action);
        }
    }

    // if int = 0, resume process
    if (action == 0)
    {
        kill(atoi(pid), SIGCONT);
    }
    // else if int = 1, suspend process
    else if (action == 1)
    {
        kill(atoi(pid), SIGSTOP);
    }
    // else if int = 2, kill process
    else if (action == 2)
    {
        kill(atoi(pid), SIGKILL);
    }
    // else if int = 3, terminate process
    else if (action == 3)
    {
        kill(atoi(pid), SIGTERM);
    }
}

void input()
{
    // Ctrl+C input
    signal(SIGINT, &trap2);

    // print reset
    printf("\033[0m");

    char command[1000];

    // get command (accepts spaces and long commands)
    fgets(command, 1000, stdin);

    // strip newline (must be done after fgets)
    command[strcspn(command, "\n")] = 0;

    // if user writes "terminate", terminate process with pid
    if (strcmp(command, "terminate") == 0)
    {
        inputAction(3);
        monitor();
    }
    // if user writes "kill", kill process with pid
    else if (strcmp(command, "kill") == 0)
    {
        inputAction(2);
        monitor();
    }
    // if user writes "suspend", suspend process with pid
    else if (strcmp(command, "suspend") == 0)
    {
        inputAction(1);
        monitor();
    }
    // if user writes "resume", resume process with pid
    else if (strcmp(command, "resume") == 0)
    {
        inputAction(0);
        monitor();
    }
    // if user writes "interval", set refresh interval to
    else if (strcmp(command, "interval") == 0)
    {
        inputInterval();
        monitor();
    }
    // if user writes "exit" or "quit", exit program
    else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
    {
        system("clear");
        exit(0);
    }
    // if user writes "monitor" or "back", go back to monitor
    else if (strcmp(command, "monitor") == 0 || strcmp(command, "back") == 0)
    {
        monitor();
    }
    // if users writes "highlight", highlight process with pid
    else if (strcmp(command, "highlight") == 0)
    {
        inputHighlight();
        monitor();
    }
    // if user writes "unhighlight", unhighlight process with pid
    else if (strcmp(command, "unhighlight") == 0)
    {
        highlightedPID = 0;
        monitor();
    }
    // if user writes "sort", sort processes
    else if (strcmp(command, "sort") == 0)
    {
        inputSort();
        monitor();
    }
    // if user writes newline or space or ^C
    else if (command[0] == '\n' || command[0] == ' ' || command[0] == '\0')
    {
        input();
    }
    // if user writes something else, print error message
    else
    {
        printf("\nAvailable commands:\n\n");
        printf("monitor | back\t\t- Go back to monitoring processes\n");
        printf("terminate\t\t- Terminate process with pid\n");
        printf("kill\t\t\t- Kill process with pid\n");
        printf("suspend\t\t\t- Suspend process with pid\n");
        printf("resume\t\t\t- Resume process with pid\n");
        printf("interval\t\t- Change monitoring refresh interval\n");
        printf("\t\t\t  Measured in seconds, only integers allowed\n");
        printf("\t\t\t  Default is 3 seconds, minimum is 1 second\n");
        printf("highlight\t\t- Highlight process with pid\n");
        printf("unhighlight\t\t- Unhighlight currently highlighted process\n");
        printf("help\t\t\t- Print help dialog\n");
        printf("exit | quit\t\t- Exit program");

        // print yellow
        printf("\033[1;93m");

        // ask for command (fixes bug where this else condition popped up after user terminated/killed/suspended/resumed process)
        printf("\n\nEnter command: ");

        input();
    }
}

void monitor()
{
    // Ctrl+C monitor
    signal(SIGINT, &trap1);

    // execute flag used for Ctrl+C monitor handler
    execute = 1;

    while (execute)
    {
        // clear console
        system("clear");

        // print green
        printf("\033[1;92m");

        printf("SimpleProcessMonitor by Giovanni Magliocchetti\n\n");

        // print reset
        printf("\033[0m");

        FILE *fp;

        // print CPU info (manufacturer, model, speed, cores)
        // print only once
        printf("CPU:");
        fp = fopen("/proc/cpuinfo", "r");
        char line[100];
        while (fgets(line, 100, fp) != NULL)
        {
            if (strstr(line, "model name") != NULL)
            {
                // print without "model name"
                printf("%s", line + 12);
                break;
            }
        }
        fclose(fp);

        // get total physical memory of host
        long unsigned int total_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "MemTotal:", 9) == 0)
                {
                    sscanf(line, "%*s %lu", &total_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get free memory of host
        long unsigned int free_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "MemFree:", 8) == 0)
                {
                    sscanf(line, "%*s %lu", &free_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get buff memory of host
        long unsigned int buff_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "Buffers:", 8) == 0)
                {
                    sscanf(line, "%*s %lu", &buff_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get cache memory of host
        long unsigned int cache_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "Cached:", 7) == 0)
                {
                    sscanf(line, "%*s %lu", &cache_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // Formula used by Top: free memory = total memory - used memory - buff/cache memory
        // get used memory of host
        long unsigned int used_memory = total_memory - free_memory - buff_memory - cache_memory;

        // print memory info (total, free, used, buff/cache)
        printf("Memory: %lu MB total | %lu MB free | %lu MB used | %lu MB buff/cache\n", total_memory / 1024, free_memory / 1024, used_memory / 1024, (buff_memory + cache_memory) / 1024);

        // get total swap memory of host
        long unsigned int swap_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "SwapTotal:", 10) == 0)
                {
                    sscanf(line, "%*s %lu", &swap_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get swap free memory of host
        long unsigned int swap_free_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "SwapFree:", 9) == 0)
                {
                    sscanf(line, "%*s %lu", &swap_free_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get avail memory of host
        long unsigned int avail_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[100];
            while (fgets(line, sizeof(line), fp) != NULL)
            {
                if (strncmp(line, "MemAvailable:", 13) == 0)
                {
                    sscanf(line, "%*s %lu", &avail_memory);
                    break;
                }
            }
            fclose(fp);
        }

        // get used swap memory of host
        long unsigned int used_swap_memory = swap_memory - swap_free_memory;

        // print swap info (total, free, used) + available memory
        printf("Swap: %lu MB total | %lu MB free | %lu MB used | %lu MB avail\n\n", swap_memory / 1024, swap_free_memory / 1024, used_swap_memory / 1024, avail_memory / 1024);

        // print yellow
        printf("\033[1;93m");

        printf("Press Ctrl+C once to enter command, twice to exit.\n\n");

        // print hcyn
        printf("\033[1;96m");

        // print header with pid, ppid, command, cpu usage, memory usage, user, time
        printf("PID\tPPID\tUSER/GROUP\tPR\tNI\tSTATE\t%%CPU\t%%MEM\tTIME+\t\tVIRT\tRES\tSHR\tCOMMAND\n\n");

        // print reset
        printf("\033[0m");

        // array of pids (with calloc) with sizeof struct "process"
        int num_pids = 0;
        struct process *pids = calloc(num_pids, sizeof(struct process));

        // get all subfolders in /proc whose name is all digits and store them in an array
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir("/proc")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                // check if subfolder is all digits
                int is_number = 1;
                int i;
                for (i = 0; i < strlen(ent->d_name); i++)
                {
                    if (!isdigit(ent->d_name[i]))
                    {
                        is_number = 0;
                        break;
                    }
                }
                if (!is_number)
                {
                    continue;
                }
                num_pids++;
                pids = realloc(pids, num_pids * sizeof(struct process));
                pids[num_pids - 1].pid = atoi(ent->d_name);
            }
            closedir(dir);
        }
        else
        {
            // print red
            printf("\033[1;31m");

            // print error message if /proc could not be opened
            printf("Error: Could not open /proc! Exiting...\n");

            // exit with code 1
            exit(1);
        }

        // store processes data in array of structs
        for (int i = 0; i < num_pids; i++)
        {
            char filename[18];
            sprintf(filename, "/proc/%d/stat", pids[i].pid);
            FILE *f = fopen(filename, "r");

            char command[100];
            long unsigned int utime;
            long unsigned int stime;
            long long unsigned int starttime;
            long int rss;

            if (f != NULL)
            {
                fscanf(f, "%*d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %ld %ld %*d %*d %llu %*u %ld %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*u %*u %*u %*u %*d %*u %*u %*u %*u %*u %*u %*u %*d", command, &pids[i].state, &pids[i].ppid, &utime, &stime, &pids[i].pr, &pids[i].ni, &starttime, &rss);

                fclose(f);
            }

            // ========== COMMAND ==========

            // strip command of parentheses
            int j = 0;
            for (int k = 0; k < strlen(command); k++)
            {
                if (command[k] != '(' && command[k] != ')')
                {
                    command[j] = command[k];
                    j++;
                }
            }
            command[j] = '\0';

            // set process struct command
            strcpy(pids[i].command, command);

            // ========== %CPU ==========

            // get /proc/uptime
            double uptime = 0;
            fp = fopen("/proc/uptime", "r");
            if (fp != NULL)
            {
                fscanf(fp, "%lf", &uptime);
                fclose(fp);
            }
            // cpu usage in percent
            double cpu_usage = (utime / CLOCK_TICKS_PER_SECOND + stime / CLOCK_TICKS_PER_SECOND) / (uptime - starttime / CLOCK_TICKS_PER_SECOND) * 100;

            // set process struct cpu
            pids[i].cpu = cpu_usage;

            // ========== %MEM ==========

            // rss in kB
            long unsigned int rss_kB = rss * PAGE_SIZE / 1024;
            // get memory usage in percent
            double memory_usage = (double)rss_kB / (double)total_memory * 100;

            // set process struct memory
            pids[i].mem = memory_usage;

            // ========== TIME+ ==========

            // total cpu time used by the process since it started
            double total_cpu_time = utime + stime;
            // total cpu time used by the process since it started in seconds
            double total_cpu_time_sec = total_cpu_time / CLOCK_TICKS_PER_SECOND;

            // total cpu time in MM:SS:ms format
            int total_cpu_time_h = (int)total_cpu_time_sec / 3600;
            int total_cpu_time_m = (int)(total_cpu_time_sec - total_cpu_time_h * 3600) / 60;
            int total_cpu_time_s = (int)(total_cpu_time_sec - total_cpu_time_h * 3600 - total_cpu_time_m * 60);
            int total_cpu_time_ms = (int)((total_cpu_time_sec - total_cpu_time_h * 3600 - total_cpu_time_m * 60 - total_cpu_time_s) * 1000);
            char total_cpu_time_str[9];
            sprintf(total_cpu_time_str, "%02d:%02d:%02d", total_cpu_time_m, total_cpu_time_s, total_cpu_time_ms / 10);

            // set process struct time
            strcpy(pids[i].time, total_cpu_time_str);

            // ========== USER ==========

            // make "/proc/<pid>/status" string
            char status_str[20];
            sprintf(status_str, "/proc/%d/status", pids[i].pid);

            // get UID of process
            int uid = 0;
            fp = fopen(status_str, "r");
            if (fp != NULL)
            {
                char line[100];
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    if (strncmp(line, "Uid:", 4) == 0)
                    {
                        sscanf(line, "%*s %d", &uid);
                        break;
                    }
                }
                fclose(fp);
            }

            // get user name from UID
            char user_name[100];
            sprintf(user_name, "unknown");
            struct passwd *pw = getpwuid(uid);
            if (pw != NULL)
            {
                strcpy(user_name, pw->pw_name);
            }

            // if user name is long more than 6 characters, truncate it and add "+" at the end
            if (strlen(user_name) > 6)
            {
                user_name[6] = '+';
                user_name[7] = '\0';
            }

            // set process struct user
            strcpy(pids[i].user, user_name);

            // ========== GROUP ==========

            // get GID of process
            int gid = 0;
            fp = fopen(status_str, "r");
            if (fp != NULL)
            {
                char line[100];
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    if (strncmp(line, "Gid:", 4) == 0)
                    {
                        sscanf(line, "%*s %d", &gid);
                        break;
                    }
                }
                fclose(fp);
            }

            // get group name from GID
            char group_name[100];
            sprintf(group_name, "unknown");
            struct group *gr = getgrgid(gid);
            if (gr != NULL)
            {
                strcpy(group_name, gr->gr_name);
            }

            // if group name is long more than 6 characters, truncate it and add "+" at the end
            if (strlen(group_name) > 6)
            {
                group_name[6] = '+';
                group_name[7] = '\0';
            }

            // set process struct group
            strcpy(pids[i].group, group_name);

            // ========== VIRT ==========

            // get VIRT
            long unsigned int virt = 0;
            fp = fopen(status_str, "r");
            if (fp != NULL)
            {
                char line[100];
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    if (strncmp(line, "VmSize:", 7) == 0)
                    {
                        sscanf(line, "%*s %lu", &virt);
                        break;
                    }
                }
                fclose(fp);
            }

            // set process struct virt
            pids[i].virt = virt;

            // ========== RES ==========

            // get RES
            long unsigned int res = 0;
            fp = fopen(status_str, "r");
            if (fp != NULL)
            {
                char line[100];
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    if (strncmp(line, "VmRSS:", 6) == 0)
                    {
                        sscanf(line, "%*s %lu", &res);
                        break;
                    }
                }
                fclose(fp);
            }

            // set process struct res
            pids[i].res = res;

            // ========== SHR ==========

            // get SHR
            long unsigned int shr = 0;
            fp = fopen(status_str, "r");
            if (fp != NULL)
            {
                char line[100];
                while (fgets(line, sizeof(line), fp) != NULL)
                {
                    if (strncmp(line, "VmData:", 7) == 0)
                    {
                        sscanf(line, "%*s %lu", &shr);
                        break;
                    }
                }
                fclose(fp);
            }

            // set process struct shr
            pids[i].shr = shr;
        }

        // sort pids array by value of global variable "sort_by"
        if (sort_by == 1 || sort_by == 2)
        {
            qsort(pids, num_pids, sizeof(struct process), compare);
        }

        // print all processes
        for (int i = 0; i < num_pids; i++)
        {
            // if pid is the one to be highlighted, print it in yellow
            if (pids[i].pid == highlightedPID && highlightedPID != 0)
            {
                // print yellow
                printf("\033[1;33m");
            }

            // print process info
            printf("%d\t%d\t%s/%s\t%ld\t%ld\t%c\t%.2f\t%.2f\t%s\t%ld\t%ld\t%ld\t%s\n", pids[i].pid, pids[i].ppid, pids[i].user, pids[i].group, pids[i].pr, pids[i].ni, pids[i].state, pids[i].cpu, pids[i].mem, pids[i].time, pids[i].virt, pids[i].res, pids[i].shr, pids[i].command);

            // print reset
            printf("\033[0m");
        }

        free(pids);

        sleep(refreshInterval);
    }

    // print yellow
    printf("\033[1;93m");

    // ask for command
    printf("\n\nEnter command: ");

    input();
}

void main(int argc, char *argv[])
{
    // if arguments array is not empty
    if (argc > 1)
    {
        // if arguments are "interval" and "time", set refreshInterval to "time"
        if (argc == 3 && strcmp(argv[1], "interval") == 0)
        {
            refreshInterval = atoi(argv[2]);
        }
        // if arguments are "terminate" and "pid", terminate process with pid
        else if (argc == 3 && strcmp(argv[1], "terminate") == 0)
        {
            kill(atoi(argv[2]), SIGTERM);
            exit(0);
        }
        // if arguments are "kill" and "pid", kill process with pid
        else if (argc == 3 && strcmp(argv[1], "kill") == 0)
        {
            kill(atoi(argv[2]), SIGKILL);
            exit(0);
        }
        // if arguments are "suspend" and "pid", suspend process with pid
        else if (argc == 3 && strcmp(argv[1], "suspend") == 0)
        {
            kill(atoi(argv[2]), SIGSTOP);
            exit(0);
        }
        // if arguments are "resume" and "pid", resume process with pid
        else if (argc == 3 && strcmp(argv[1], "resume") == 0)
        {
            kill(atoi(argv[2]), SIGCONT);
            exit(0);
        }
        // if arguments are "sort" and "choice", sort processes by choice
        else if (argc == 3 && strcmp(argv[1], "sort") == 0)
        {
            if (strcmp(argv[2], "pid") == 0)
            {
                sort_by = 0;
            }
            else if (strcmp(argv[2], "cpu") == 0)
            {
                sort_by = 1;
            }
            else if (strcmp(argv[2], "mem") == 0)
            {
                sort_by = 2;
            }
            else
            {
                printf("Invalid sort choice. Valid choices are: pid, cpu, mem\n");
                exit(0);
            }
        }
        // if arguments are something else, print usage
        else
        {
            printf("Usage: ./SimpleProcessMonitor [help] [terminate <pid>] [kill <pid>] [suspend <pid>] [resume <pid>] [interval <time_ss>] [sort <cpu|mem|pid>]\n");
            exit(0);
        }
    }

    monitor();
}
