#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>


const char *state_name(char code) {
    switch (code) {
        case 'R': return "Running";
        case 'S': return "Sleeping";
        case 'D': return "Disk Sleep";
        case 'Z': return "Zombie";
        case 'T': return "Stopped";
        case 't': return "Tracing Stop";
        case 'X': return "Dead";
        case 'I': return "Idle";
        default:  return "Unknown";
    }
}

int is_numeric(const char *s) {
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

int main() {
    DIR *proc = opendir("/proc");
    if (!proc) {
        perror("opendir");
        return 1;
    }

    printf("%-15s %-20s %-15s %-10s\n", "PID", "Process Name", "State", "Parent PID");
    for (int i = 0; i < 60; i++) printf("-");
    printf("\n");

    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (!is_numeric(entry->d_name)) continue;

        char path[64], line[256];
        char name[128] = "", ppid[16] = "";
        char state_code = '?';

        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Name:", 5) == 0)
                sscanf(line, "Name:\t%127[^\n]", name);
            else if (strncmp(line, "State:", 6) == 0)
                sscanf(line, "State:\t%c", &state_code);
            else if (strncmp(line, "PPid:", 5) == 0)
                sscanf(line, "PPid:\t%15[^\n]", ppid);
        }
        fclose(fp);

        printf("%-15s %-20s %-15s %-10s\n",
               entry->d_name, name, state_name(state_code), ppid);
    }

    closedir(proc);
    return 0;
}
