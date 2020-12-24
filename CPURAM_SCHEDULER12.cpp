#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <exception>
#include <unistd.h>
#include <time.h>
#include <string>
#include <sstream>

bool isCritPerformance(double *cpu, double *ram)
{
    char buffer[128];
    double cpu_used = 0.0, percent_ram;

    FILE *pipe = popen("top -b -n 1 | sed -n 3,4p", "r");

    if (!pipe)
    {
        printf("FD pipe failed to be created\n");
        exit(-1);
    }

    try
    {
        while (fgets(buffer, 128, pipe))
        {
            // CPU processing
            if (memcmp(buffer, "%", 1) == 0)
            {
                size_t pos = 0;
                char *token = strtok(&buffer[8], ",");

                while (token)
                {
                    pos++;
                    if (pos == 1 || pos == 2)
                    {
                        char numa[10];
                        memset(numa, 0x00, 10);
                        memcpy(numa, token, 6);
                        cpu_used += atof(numa);
                        // printf("[%s]\n", token);
                    }
                    token = strtok(NULL, ",");
                }
                // printf("CPU used : %.2f %%\n", cpu_used);
            }
            // RAM processing
            else
            {
                size_t pos = 0;
                char *token = strtok(&buffer[9], ",");
                double ram_used, ram_total;

                while (token)
                {
                    pos++;
                    if (pos == 1)
                    {
                        char val[12];
                        memset(val, 0x00, 12);
                        memcpy(val, token, 10);
                        ram_total = atof(val);
                    }
                    else if (pos == 3)
                    {
                        char val[12];
                        memset(val, 0x00, 12);
                        memcpy(val, token, 10);
                        ram_used = atof(val);
                    }
                    // printf("[%s]\n", token);
                    token = strtok(NULL, ",");
                }
                percent_ram = 100 * ram_used / ram_total;
                // printf("RAM used : %.2f %%\n", percent_ram);
            }
        }
    }
    catch (const std::exception &e)
    {
        pclose(pipe);
        printf("%s\n", e.what());
    }

    pclose(pipe);

    *cpu = cpu_used / 4.0;
    *ram = percent_ram;

    if (percent_ram >= 70.0 || *cpu >= 70.0)
    {
        return true;
    }
    return false;
}

void sendWarningMsg(const char *currenttime, const double *cpu, const double *ram)
{
    std::ostringstream ss;
    char percent_stat[128];
    sprintf(percent_stat, "CPU used: %.2f %%, RAM used: %.2f %%", *cpu, *ram);

    ss << "curl -H \"Content-Type: application/json\" --data";
    ss << " '{\"chat_id\":-495061825, \"text\":\"[WARNING garudaprofit.gameplace.asia]\n"
       << currenttime << percent_stat << "\"}'";
    ss << " https://api.telegram.org/bot1220023412:AAF19w_vUpv2ctznq6HR88WxNIe_XMM8KK4/sendMessage";
    ss << " >>/dev/null 2>>/dev/null";

    std::string command(ss.str());

    system(command.c_str());
}

void checkLocalTime()
{
    time_t local;
    struct tm *localtm;

    time(&local);
    localtm = localtime(&local);

    printf("current time : %s", asctime(localtm));

    FILE *log = fopen("check_cron.txt", "a");

    fprintf(log, asctime(localtm));
}

int main(int argc, char *argv[])
{
    checkLocalTime();

    // pid_t pid = fork();

    // if (pid > 0)
    // {
    //     printf("Process run in the background PID: %d\n", pid);
    //     exit(0);
    // }

    // while (1)
    // {
    //     sleep(1);
    //     double cpu_used, ram_used;

    //     if (isCritPerformance(&cpu_used, &ram_used))
    //     {
    //         time_t local;
    //         struct tm *localtm;

    //         time(&local);
    //         localtm = localtime(&local);

    //         char filename[128], program[128];
    //         strcpy(program, argv[0]);
    //         memset(filename, 0x00, sizeof(filename));
    //         sprintf(filename, "%s/%s%02d%02d.log", getenv("LOG_HOME"), argv[0],
    //                 localtm->tm_mon + 1, localtm->tm_mday);

    //         FILE *log = fopen(filename, "a");
    //         if (log != NULL)
    //         {
    //             fprintf(log, "%s", asctime(localtm));
    //             fprintf(log, "CPU used: %.2f %%\n", cpu_used);
    //             fprintf(log, "RAM used: %.2f %%\n\n", ram_used);

    //             sendWarningMsg(asctime(localtm), &cpu_used, &ram_used);
    //         }
    //         fclose(log);
    //     };
    // }

    return 0;
}
