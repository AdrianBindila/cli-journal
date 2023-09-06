#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <windows.h>

#define DEFAULT_PATH "./journal/" //default directory for entries
#define DEFAULT_PATH_DIR "./journal/*"
#define MAX_TITLE_LEN 65
#define MAX_FILE_NAME_LEN 33
#define MAX_ENTRY_LEN 5
#define MAX_LINE_LEN 2048

void addNewEntry(struct tm tm) {
    char timebuf[128];
    char format[32];

    strftime(timebuf, sizeof(timebuf), "%A, %B %d, %Y %I:%M%p", &tm);

    printf("Specify a file name! Max. %d characters!\n", MAX_FILE_NAME_LEN - 1);
    char filename[MAX_FILE_NAME_LEN];
    fgets(filename, sizeof(filename), stdin);
    filename[strlen(filename) - 1] = 0;//trim newline

    char path[128] = DEFAULT_PATH;
    strcat(path, filename);
    FILE *file = fopen(path, "w");

    if (file == NULL && errno == ENOENT) {//if directory doesn't exist, create one
        CreateDirectory(DEFAULT_PATH, NULL);
        file = fopen(path, "w");
    }

    if (file == NULL) {
        perror("Error opening file!");
        return;
    }

    printf("Write a title for the journal entry! Max. %d characters!\n", MAX_TITLE_LEN - 1);

    char title[MAX_TITLE_LEN];
    fgets(title, sizeof(title), stdin);
    fputs(title, file);
    fputs("=====================================\n", file);
    fputs(timebuf, file);
    fputs("\n=====================================\n", file);

    char line[MAX_LINE_LEN];
    printf("Happy writing! To finish writing, type *quit* on a new line (* included)!\n");
    printf("%s\n", timebuf);

    while (1) {
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, "*quit*\n") == 0) {
            fclose(file);
            break;
        }

        fputs(line, file);
    }
}

void readFile(const char *filename) {
    char path[127] = DEFAULT_PATH;
    strcat(path, filename);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file!");
        return;
    }

    char line[MAX_LINE_LEN];

    while (fgets(line, MAX_LINE_LEN, file) != NULL) {
        printf("%s", line);
    }
    fclose(file);
    getchar();
}

void listEntries() {
    //check for existing entries and list them 5 each per page
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    hFind = FindFirstFile(DEFAULT_PATH_DIR, &findData);
    BOOL isEnd = hFind == INVALID_HANDLE_VALUE;
    if (isEnd) {
        printf("FindFirstFile failed (%lu)\n", GetLastError());
        return;
    }

    //read directory entries first then list them as you please
    char fileList[128][MAX_FILE_NAME_LEN];
    int l = 0;
    while (!isEnd) {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            //store the file names
            strcpy(fileList[l++], findData.cFileName);
        }
        isEnd = !FindNextFile(hFind, &findData);
    }
    int offset = 0;
    printf("Select one of the previous journal entries to read them!\n");
    while (1) {
        printf("=============Entries=============\n");
        for (int i = 0; i < MAX_ENTRY_LEN && i + offset < l; i++) {
            printf("%d) %s\n", i + 1, fileList[i + offset]);
        }
        printf("================================\n");
        if (offset + MAX_ENTRY_LEN < l)
            printf("n) Next\n");

        if (offset - MAX_ENTRY_LEN >= 0)
            printf("p) Previous\n");

        printf("q) Quit!\n");

        char c = (char) getchar();
        getc(stdin);//clear stdin buffer

        switch (c) {
            case 'n':
                offset += MAX_ENTRY_LEN;
                break;
            case 'p':
                offset -= MAX_ENTRY_LEN;
                break;
            case 'q':
                FindClose(hFind);
                return;
            default:
                if (c >= '1' && c <= '5') {
                    readFile(fileList[c - '1' + offset]);
                } else {
                    printf("Please insert a valid option!");
                }
                break;
        }
    }
}

int main() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char timebuf[128];
    strftime(timebuf, sizeof(timebuf), "%A, %B %d, %Y %I:%M%p", &tm);
    printf("It is currently: %s\n", timebuf);

    char c;
    while (1) {
        printf("1) Start writing your journal now!\n");
        printf("2) Check previous entries!\n");
        printf("3) Quit! \n");

        c = (char) getchar();
        getc(stdin);//clear stdin buffer

        switch (c) {
            case '1':
                addNewEntry(tm);
                break;
            case '2':
                listEntries();
                break;
            case '3':
                return 0;
            default:
                break;
        }
    }
}
