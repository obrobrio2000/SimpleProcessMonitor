// Process monitor that allows to terminate, kill, suspend and resume running processes. The processes are accessed via the /proc filesystem. The program monitors the resources used by each process (CPU usage / Memory usage). To close the program, press Ctrl+C.

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

/* DA FARE:
    1) Fixare bug che se si fa Ctrl+C dopo una richiesta di kill stampa l'help
    2) Aggiungere username e groupname alla lista dei processi
    3) Far uscire %CPU come su top
    4) Porting processo su struttura dati
    5) Rimuovere campi inutili dalla fscanf
    6) Rimuovere processi doppioni dalla lista
*/

int refreshInterval = 1; // Refresh interval in seconds

int execute;
void trap(int signal)
{
    execute = 0;
}

// flush input
void flushInput()
{
    fseek(stdin, 0, SEEK_END);
    fflush(stdin);
}

int main()
{
start:
    signal(SIGINT, &trap);

    execute = 1;

    while (execute)
    {
        // clear console
        system("clear");

        // print green
        printf("\033[1;92m");

        printf("Process Monitor by Giovanni Magliocchetti\n\n");

        // print yellow
        printf("\033[1;93m");

        printf("Press Ctrl+C once to enter command, twice to exit.\n\n");

        // print hcyn
        printf("\033[1;96m");

        // print header with pid, ppid, command, cpu usage, memory usage, user, time
        printf("PID\tPPID\tSTATE\t%%CPU\t%%MEM\tTIME+\t\tCOMMAND\n\n");

        // print reset
        printf("\033[0m");

        // get maximum number of processes from /proc/sys/kernel/pid_max
        int max_processes = 0;
        FILE *fp = fopen("/proc/sys/kernel/pid_max", "r");
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
                if (isdigit(ent->d_name[0]))
                {
                    pids[atoi(ent->d_name)] = 1;
                    // printf("%s\n", ent->d_name);
                }
            }
            closedir(dir);
        }
        else
        {
            printf("Error: Could not open /proc\n");
            return 1;
        }

        // print all processes
        for (int i = 0; i < max_processes; i++)
        {
            if (pids[i] == 1)
            {
                char filename[1000];
                sprintf(filename, "/proc/%d/stat", i);
                FILE *f = fopen(filename, "r");

                int pid;
                char command[1000];
                char state;
                int ppid;
                int pgrp;
                int session;
                int tty_nr;
                int tpgid;
                unsigned int flags;
                long unsigned int minflt;
                long unsigned int cminflt;
                long unsigned int majflt;
                long unsigned int cmajflt;
                long unsigned int utime;
                long unsigned int stime;
                long int cutime;
                long int cstime;
                long int priority;
                long int nice;
                long int num_threads;
                long int itrealvalue;
                long long unsigned int starttime;
                long unsigned int vsize;
                long int rss;
                long unsigned int rsslim;
                long unsigned int startcode;
                long unsigned int endcode;
                long unsigned int startstack;
                long unsigned int kstkesp;
                long unsigned int kstkeip;
                long unsigned int signal;
                long unsigned int blocked;
                long unsigned int sigignore;
                long unsigned int sigcatch;
                long unsigned int wchan;
                long unsigned int nswap;
                long unsigned int cnswap;
                int exit_signal;
                int processor;
                unsigned int rt_priority;
                unsigned int policy;
                long long unsigned int delayacct_blkio_ticks;
                long unsigned int guest_time;
                long int cguest_time;
                long unsigned int start_data;
                long unsigned int end_data;
                long unsigned int start_brk;
                long unsigned int arg_start;
                long unsigned int arg_end;
                long unsigned int env_start;
                long unsigned int env_end;
                int exit_code;

                if (f != NULL)
                {
                    fscanf(f, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d", &pid, command, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads, &itrealvalue, &starttime, &vsize, &rss, &rsslim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor, &rt_priority, &policy, &delayacct_blkio_ticks, &guest_time, &cguest_time, &start_data, &end_data, &start_brk, &arg_start, &arg_end, &env_start, &env_end, &exit_code);

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

                // ========== CPU USAGE ==========

                // get /proc/uptime
                double uptime = 0;
                fp = fopen("/proc/uptime", "r");
                if (fp != NULL)
                {
                    fscanf(fp, "%lf", &uptime);
                    fclose(fp);
                }
                // cpu usage in percent
                double cpu_usage = (utime / sysconf(_SC_CLK_TCK) + stime / sysconf(_SC_CLK_TCK)) / (uptime - starttime / sysconf(_SC_CLK_TCK)) * 100;

                // ========== MEMORY USAGE ==========

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
                // rss in kB
                long unsigned int rss_kB = rss * getpagesize() / 1024;
                // get memory usage in percent
                double memory_usage = (double)rss_kB / (double)total_memory * 100;

                // total cpu time used by the process since it started
                double total_cpu_time = utime + stime;
                // total cpu time used by the process since it started in seconds
                double total_cpu_time_sec = total_cpu_time / sysconf(_SC_CLK_TCK);

                // total cpu time in MM:SS:ms format
                int total_cpu_time_h = (int)total_cpu_time_sec / 3600;
                int total_cpu_time_m = (int)(total_cpu_time_sec - total_cpu_time_h * 3600) / 60;
                int total_cpu_time_s = (int)(total_cpu_time_sec - total_cpu_time_h * 3600 - total_cpu_time_m * 60);
                int total_cpu_time_ms = (int)((total_cpu_time_sec - total_cpu_time_h * 3600 - total_cpu_time_m * 60 - total_cpu_time_s) * 1000);
                char total_cpu_time_str[1000];
                sprintf(total_cpu_time_str, "%02d:%02d:%02d", total_cpu_time_m, total_cpu_time_s, total_cpu_time_ms / 10);

                printf("%d\t%d\t%c\t%.2f\t%.2f\t%s\t%s\n", pid, ppid, state, cpu_usage, memory_usage, total_cpu_time_str, command);

                pids[i] = 0;
            }
        }
        sleep(refreshInterval);
    }

input:

    signal(SIGINT, SIG_DFL);

    // print yellow
    printf("\033[1;93m");

    printf("\n\nEnter command: ");

    // print reset
    printf("\033[0m");

    char command[1000];

    // get command (accepts spaces)
    fgets(command, 1000, stdin);

    // strip newline
    command[strcspn(command, "\n")] = 0;

    // if user writes "terminate <pid>", terminate process with pid <pid>
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
            if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n')
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
    // if user writes "kill <pid>", kill process with pid <pid>
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
            if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n')
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
    // if user writes "suspend <pid>", suspend process with pid <pid>
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
            if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n')
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
    // if user writes "resume <pid>", resume process with pid <pid>
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
            if (!isdigit(pid[i]) && pid[i] != ' ' && pid[i] != '\n')
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
    // if user writes "interval <seconds>", set refresh interval to <seconds>
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
            if (!isdigit(interval[i]) || interval[i] == ' ' || interval[i] == '\n')
            {
                // print red
                printf("\033[1;91m");

                printf("\nError: invalid interval!\n");

                // print reset
                printf("\033[0m");

                goto inputInterval;
            }
        }

        // if interval is 0, set interval to 1
        if (atoi(interval) == 0)
        {
            refreshInterval = 1;
        }
        else
        {
            refreshInterval = atoi(interval);
        }

        goto start;
    }
    // if user writes "exit", exit program
    else if (strcmp(command, "exit") == 0)
    {
        printf("\nHave a nice day! 😊\n\n");

        exit(0);
    }
    // if user writes "monitor" or "back", go back to start
    else if (strcmp(command, "monitor") == 0 || strcmp(command, "back") == 0)
    {
        goto start;
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
        printf("\t\t\t  Default is 1 second, minimum is 1 second\n");
        printf("help\t\t\t- Print help\n");
        printf("exit\t\t\t- Exit program");

        goto input;
    }

    return 0;
}