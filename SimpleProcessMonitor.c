// SimpleProcessMonitor by Giovanni Magliocchetti
// Allows to terminate, kill, suspend and resume running processes.
// The processes are accessed via the /proc filesystem.
// The program monitors the resources used by each process (CPU usage / Memory usage).

#include "SimpleProcessMonitor.h"

int main(int argc, char *argv[])
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
        // if arguments are something else, print usage
        else
        {
            printf("Usage: ./SimpleProcessMonitor [interval <time>] [terminate <pid>] [kill <pid>] [suspend <pid>] [resume <pid>]\n");
            exit(0);
        }
    }

start:
    // Ctrl+C start
    signal(SIGINT, &trap1);

    // execute flag used for Ctrl+C start handler
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
            char line[1000];
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
            char line[1000];
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
            char line[1000];
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
            char line[1000];
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
        printf("Memory: %lu MB (total) / %lu MB (free) / %lu MB (used) / %lu MB (buff/cache)\n", total_memory / 1024, free_memory / 1024, used_memory / 1024, (buff_memory + cache_memory) / 1024);

        // get total swap memory of host
        long unsigned int swap_memory = 0;
        fp = fopen("/proc/meminfo", "r");
        if (fp != NULL)
        {
            char line[1000];
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
            char line[1000];
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
            char line[1000];
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
        printf("Swap: %lu MB (total) / %lu MB (free) / %lu MB (used) / %lu MB (avail)\n\n", swap_memory / 1024, swap_free_memory / 1024, used_swap_memory / 1024, avail_memory / 1024);

        // print yellow
        printf("\033[1;93m");

        printf("Press Ctrl+C once to enter command, twice to exit.\n\n");

        // print hcyn
        printf("\033[1;96m");

        // print header with pid, ppid, command, cpu usage, memory usage, user, time
        printf("PID\tPPID\tUSER\tGROUP\tPR\tNI\tSTATE\t%%CPU\t%%MEM\tTIME+\t\tVIRT/100\tRES/100\t\tSHR/100\t\tCOMMAND\n\n");

        // print reset
        printf("\033[0m");

        // get maximum number of processes from /proc/sys/kernel/pid_max
        int max_processes = 0;
        fp = fopen("/proc/sys/kernel/pid_max", "r");
        if (fp != NULL)
        {
            fscanf(fp, "%d", &max_processes);
            fclose(fp);
        }
        else
        {
            printf("Error: Could not open /proc/sys/kernel/pid_max\n");
            return 1;
        }

        // array of pids
        int pids[max_processes];

        // get all subfolders in /proc that start with a number and store them in an array
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir("/proc")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                // checks for a number in the subfolder name
                if (isdigit(ent->d_name[0]))
                {
                    // if process is running, set element at index <pid> to 1
                    pids[atoi(ent->d_name)] = 1;
                    // printf("%s\n", ent->d_name);
                }
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
            return 1;
        }

        // print all processes
        for (int i = 0; i < max_processes; i++)
        {
            // check if pid should be printed (also works as a duplicate check, kinda...)
            if (pids[i] == 1)
            {
                char filename[1000];
                sprintf(filename, "/proc/%d/stat", i);
                FILE *f = fopen(filename, "r");

                int pid;
                char command[1000];
                char state;
                int ppid;
                long unsigned int utime;
                long unsigned int stime;
                long int priority;
                long int nice;
                long long unsigned int starttime;
                long int rss;

                if (f != NULL)
                {
                    fscanf(f, "%d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %ld %ld %*d %*d %llu %*u %ld %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*u %*u %*u %*u %*d %*u %*u %*u %*u %*u %*u %*u %*d", &pid, command, &state, &ppid, &utime, &stime, &priority, &nice, &starttime, &rss);

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

                // ========== %MEM ==========

                // rss in kB
                long unsigned int rss_kB = rss * PAGE_SIZE / 1024;
                // get memory usage in percent
                double memory_usage = (double)rss_kB / (double)total_memory * 100;

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
                char total_cpu_time_str[1000];
                sprintf(total_cpu_time_str, "%02d:%02d:%02d", total_cpu_time_m, total_cpu_time_s, total_cpu_time_ms / 10);

                // ========== USER ==========

                // make "/proc/<pid>/status" string
                char status_str[1000];
                sprintf(status_str, "/proc/%d/status", pid);

                // get UID of process
                int uid = 0;
                fp = fopen(status_str, "r");
                if (fp != NULL)
                {
                    char line[1000];
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
                char user_name[1000];
                sprintf(user_name, "unknown");
                struct passwd *pw = getpwuid(uid);
                if (pw != NULL)
                {
                    strcpy(user_name, pw->pw_name);
                }

                // ========== GROUP ==========

                // get GID of process
                int gid = 0;
                fp = fopen(status_str, "r");
                if (fp != NULL)
                {
                    char line[1000];
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
                char group_name[1000];
                sprintf(group_name, "unknown");
                struct group *gr = getgrgid(gid);
                if (gr != NULL)
                {
                    strcpy(group_name, gr->gr_name);
                }

                // ========== VIRT ==========

                // get VIRT
                long unsigned int virt = 0;
                fp = fopen(status_str, "r");
                if (fp != NULL)
                {
                    char line[1000];
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

                // ========== RES ==========

                // get RES
                long unsigned int res = 0;
                fp = fopen(status_str, "r");
                if (fp != NULL)
                {
                    char line[1000];
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

                // ========== SHR ==========

                // get SHR
                long unsigned int shr = 0;
                fp = fopen(status_str, "r");
                if (fp != NULL)
                {
                    char line[1000];
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

                // if pid is the one to be highlighted, print it in yellow
                if (pid == highlightedPID && highlightedPID != 0)
                {
                    // print yellow
                    printf("\033[1;33m");
                }

                // print process info
                printf("%d\t%d\t%s\t%s\t%ld\t%ld\t%d\t%.2f\t%.2f\t%s\t%ld\t\t%ld\t\t%ld\t\t%s\n", pid, ppid, user_name, group_name, priority, nice, state, cpu_usage, memory_usage, total_cpu_time_str, virt / 100, res / 100, shr / 100, command);

                // print reset
                printf("\033[0m");

                pids[i] = 0;
            }
        }

        sleep(refreshInterval);
    }

    // print yellow
    printf("\033[1;93m");

    // ask for command
    printf("\n\nEnter command: ");

input:

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
    inputTerminate:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter PID: ");

        // print reset
        printf("\033[0m");

        char pid[1000];
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

                goto inputTerminate;
            }
        }

        kill(atoi(pid), SIGTERM);

        goto start;
    }
    // if user writes "kill", kill process with pid
    else if (strcmp(command, "kill") == 0)
    {
    inputKill:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter PID: ");

        // print reset
        printf("\033[0m");

        char pid[1000];
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

                goto inputKill;
            }
        }

        kill(atoi(pid), SIGKILL);

        goto start;
    }
    // if user writes "suspend", suspend process with pid
    else if (strcmp(command, "suspend") == 0)
    {
    inputSuspend:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter PID: ");

        // print reset
        printf("\033[0m");

        char pid[1000];
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

                goto inputSuspend;
            }
        }

        kill(atoi(pid), SIGSTOP);

        goto start;
    }
    // if user writes "resume", resume process with pid
    else if (strcmp(command, "resume") == 0)
    {
    inputResume:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter PID: ");

        // print reset
        printf("\033[0m");

        char pid[1000];
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

                goto inputResume;
            }
        }

        kill(atoi(pid), SIGCONT);

        goto start;
    }
    // if user writes "interval", set refresh interval to
    else if (strcmp(command, "interval") == 0)
    {
    inputInterval:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter refresh interval: ");

        // print reset
        printf("\033[0m");

        char interval[1000];
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

                goto inputInterval;
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

        goto start;
    }
    // if user writes "exit" or "quit", exit program
    else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
    {
        system("clear");
        exit(0);
    }
    // if user writes "monitor" or "back", go back to start
    else if (strcmp(command, "monitor") == 0 || strcmp(command, "back") == 0)
    {
        goto start;
    }
    // if users writes "highlight", highlight process with pid
    else if (strcmp(command, "highlight") == 0)
    {
    inputHighlight:
        // print yellow
        printf("\033[1;93m");

        printf("\nEnter PID: ");

        // print reset
        printf("\033[0m");

        char pid[1000];
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

                goto inputHighlight;
            }

            // set pid to be highlighted
            highlightedPID = atoi(pid);
        }

        goto start;
    }
    // if user writes "unhighlight", unhighlight process with pid
    else if (strcmp(command, "unhighlight") == 0)
    {
        highlightedPID = 0;

        goto start;
    }
    // if user writes newline or space or ^C
    else if (command[0] == '\n' || command[0] == ' ' || command[0] == '\0')
    {
        goto input;
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

        goto input;
    }

    return 0;
}
