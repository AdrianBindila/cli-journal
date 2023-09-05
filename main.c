#include <stdio.h>
#include <time.h>
#include <string.h>

void addNewEntry(struct tm tm) {
    char timebuf[80];
    strftime(timebuf, sizeof(timebuf), "%A, %B %d, %Y %I:%M%p", &tm);
    printf("%s",timebuf);

    FILE *file = fopen(timebuf, "w");
    if (file == NULL) {
        perror("Error opening file!");
        return;
    }

    char line[2048];
    printf("Happy writing! To finish writing, type *quit* on a new line!\n");
    while (1) {
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, "*quit*\n") == 0) {
            fclose(file);
            break;
        }

        fputs(line, file);
    }
}

void listEntries() {
    //check for existing entries and list them 10 each per page
}

int main() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *time = asctime(&tm);
    printf("It is currently: %s\n", time);

    char c;
    do {
        printf("1) Start writing your journal now!\n");
        printf("2) Check previous entries!\n");

        c = (char) getchar();
        getc(stdin);//clear stdin buffer
    } while (c != '1' && c != '2');

    switch (c) {
        case '1':
            addNewEntry(tm);
            break;
        case '2':
            listEntries();
            break;
        default:
            break;
    }
    return 0;
}
