#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <exception>
#include <unistd.h>
#include <time.h>
#include <string>
#include <sstream>

void isCritPerformance(double *cpu, double *ram)
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

    *cpu = cpu_used / 2.0;
    *ram = percent_ram;
}

void sendWarningMsg(const char *currenttime, const double &cpu, const double &ram)
{
    std::ostringstream ss;
    char percent_stat[128];
    sprintf(percent_stat, "AVGCPU used: %.2f %%, AVGRAM used: %.2f %%", cpu, ram);

    ss << "curl -H \"Content-Type: application/json\" --data";
    ss << " '{\"chat_id\":-495061825, \"text\":\"[DAILY REPORT DB garudaprofit.gameplace.asia]\n"
       << currenttime << percent_stat << "\"}'";
    ss << " https://api.telegram.org/bot1220023412:AAF19w_vUpv2ctznq6HR88WxNIe_XMM8KK4/sendMessage";
    ss << " >>/dev/null 2>>/dev/null";

    std::string command(ss.str());

    system(command.c_str());
}

void checkLocalTime(char *target_tz = "Asia/Jakarta")
{
    time_t local;
    struct tm *localtm, *globaltm;
    char tzsetup[20];

    time(&local);
    localtm = localtime(&local);

    local = mktime(localtm);

    sprintf(tzsetup, "TZ=%s", target_tz);
    putenv(tzsetup);
    globaltm = localtime(&local);

    printf("current time : %s", asctime(globaltm));

    FILE *log = fopen("check_cron.txt", "a");

    printf("%d-%d-%d", globaltm->tm_hour, globaltm->tm_min, globaltm->tm_sec);

    fprintf(log, asctime(globaltm));
}

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if (pid > 0)
    {
        printf("Process is running in the background PID: %d\n", pid);
        exit(0);
    }

    int nCpu = 0;
    int nRam = 0;
    double totCpu = 0.0; // in percent
    double totRam = 0.0; // in percent

    while (1)
    {
        sleep(1);
        double cpu_used, ram_used;

        isCritPerformance(&cpu_used, &ram_used);
        totCpu += cpu_used;
        totRam += ram_used;

        nCpu++;
        nRam++;

        time_t local;
        struct tm *localtm, *globaltm;
        char tzsetup[20];

        time(&local);
        localtm = localtime(&local);

        local = mktime(localtm);

        sprintf(tzsetup, "TZ=%s", "Asia/Jakarta");
        putenv(tzsetup);
        globaltm = localtime(&local);

        if ((globaltm->tm_hour == 12 && globaltm->tm_min == 00 && globaltm->tm_sec == 0) ||
            (globaltm->tm_hour == 23 && globaltm->tm_min == 59 && globaltm->tm_sec == 0))
        {
            double avgCpu = 0.0, avgRam = 0.0;

            if (nCpu != 0)
                avgCpu = (double)(totCpu / nCpu);

            if (nRam != 0)
                avgRam = (double)(totRam / nRam);

            sendWarningMsg(asctime(globaltm), (double)(totCpu / nCpu), (double)(totRam / nRam));

            totCpu = 0.0;
            totRam = 0.0;
            nCpu = 0;
            nRam = 0;
        }
    }

    return 0;
}
