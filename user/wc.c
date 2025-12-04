#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUF_SIZE 512

int flag_l = 0;
int flag_w = 0;
int flag_c = 0;
int flag_L = 0;

void wc_file(char *filename, int fd, int *total_lines, int *total_words, int *total_chars, int *total_longest) {
    char buf[BUF_SIZE];
    int n;

    int lines = 0;
    int words = 0;
    int chars = 0;
    int longest = 0;
    int current_len = 0;
    int in_word = 0;

    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            char c = buf[i];
            chars++;

            // Line counting and longest line tracking
            if (c == '\n') {
                lines++;
                if (current_len > longest) longest = current_len;
                current_len = 0;
            } else {
                current_len++;
            }

            // Word counting
            if (c == ' ' || c == '\n' || c == '\t') {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }
    }

    // Handle last line if file does not end with newline
    if (current_len > 0) {
        lines++; // last line
        if (current_len > longest) longest = current_len;
    }

    // Update totals
    *total_lines += lines;
    *total_words += words;
    *total_chars += chars;
    if (longest > *total_longest) *total_longest = longest;

    // Print output for this file
    if (!flag_l && !flag_w && !flag_c && !flag_L) {
        // Default: lines words chars filename
        printf("%d %d %d %s\n", lines, words, chars, filename);
    } else {
        if (flag_l) printf("%d ", lines);
        if (flag_w) printf("%d ", words);
        if (flag_c) printf("%d ", chars);
        if (flag_L) printf("%d ", longest);
        printf("%s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    // Help
    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: wc [-lwcL] [files...]\n");
        exit(0);
    }

    int i = 1;

    // Parse flags
    while (i < argc && argv[i][0] == '-') {
        char *p = argv[i] + 1;
        while (*p) {
            if (*p == 'l') flag_l = 1;
            else if (*p == 'w') flag_w = 1;
            else if (*p == 'c') flag_c = 1;
            else if (*p == 'L') flag_L = 1;
            else {
                printf("Invalid flag: %c\n", *p);
                exit(0);
            }
            p++;
        }
        i++;
    }

    // No files provided
    if (i == argc) {
        printf("No files given\n");
        exit(0);
    }

    int total_lines = 0, total_words = 0, total_chars = 0, total_longest = 0;
    int file_count = 0;

    // Process each file
    for (; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            printf("Cannot open %s\n", argv[i]);
            continue;
        }

        wc_file(argv[i], fd, &total_lines, &total_words, &total_chars, &total_longest);
        close(fd);
        file_count++;
    }

    // Print totals if more than one file
    if (file_count > 1) {
        if (!flag_l && !flag_w && !flag_c && !flag_L) {
            printf("%d %d %d total\n", total_lines, total_words, total_chars);
        } else {
            if (flag_l) printf("%d ", total_lines);
            if (flag_w) printf("%d ", total_words);
            if (flag_c) printf("%d ", total_chars);
            if (flag_L) printf("%d ", total_longest);
            printf("total\n");
        }
    }

    exit(0);
}
