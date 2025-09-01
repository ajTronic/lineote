#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

#define PROMPT "what's your line for today?\n\n"

#define LINEOTE_FILE_NAME ".lineote_messages"

#define RESET       "\x1b[0m"   // reset color to default
#define BLACK       "\x1b[30m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define WHITE       "\x1b[37m"
#define GREY        "\x1b[90m" 

#define UNDERLINE "\x1b[4m"
#define ITALIC "\x1b[3m"

char* get_lineote_file_path() {
	char* homedir = getpwuid(getuid())->pw_dir;
	char* lineote_file_path = malloc(sizeof(char) * 256);
	snprintf(lineote_file_path, 256, "%s/" LINEOTE_FILE_NAME, homedir);
	return lineote_file_path;
}

void init_lineote(char* path) {
	FILE *fptr = fopen(path, "w"); // creates an empty file at the path
	fclose(fptr);
}

char* read_lineote(char* path) {
	FILE *fptr = fopen(path, "r");

	char* content = malloc(sizeof(char) * 1024);

	size_t read_size = fread(content, 1, 1024, fptr);
    content[read_size] = '\0'; // Null-terminate

	fclose(fptr);

	return content;
}

void write_to_lineote(char* path, char* date, char* message) {
	FILE *fptr = fopen(path, "a"); // open in 'a' (append) mode

	fprintf(fptr, "%s %s", date, message);

	fclose(fptr);
}

char* get_formatted_date() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char* result = malloc(13 * sizeof(char));
	snprintf(result, 13, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	return result;
}

char* prompt_for_message() {
	char* date = get_formatted_date();
	printf("\non " RED UNDERLINE "%s" RESET ":\n", date);
	free(date);

	printf(YELLOW "> " RESET);

	char* message = malloc(sizeof(char) * 1024);
	printf(ITALIC);
	fgets(message, 1024, stdin);
	printf(RESET);

	printf("\n");

	return message;
}

char* get_last_n_lines(char* str, int n) {
	int len = strlen(str);
	int line_count = 0;

	for (int i = len; i > 0; i--) {
		// char is newline but not the last newline
		if (str[i] == '\n' && i < len - 1) {
			line_count++;

			if (line_count >= n) {
				return &str[i+1]; // +1 to not include an extra \n
			}
		}
	}

	return str;
}

void print_formatted_entries(char* entries) {
	char* line;
	char* remaining = entries;
	
	while ((line = strtok_r(remaining, "\n", &remaining))) { // for each line
		char date[20];
		char message[1024];
		sscanf(line, "%s %[^\n]s", date, message);
		printf(RED UNDERLINE "%s" RESET ITALIC " %s\n" RESET, date, message);
	}
}

int main(int argc, char* argv[]) {
	enum Action {
		NEW_ENTRY,
		VIEW_ENTRIES,
		VIEW_LAST,
		UNKNOWN,
	};

	enum Action action;

	if (argc == 1) action = NEW_ENTRY;
	else if (strcmp(argv[1], "all") == 0)  action = VIEW_ENTRIES; 
	else if (strcmp(argv[1], "last") == 0)  action = VIEW_LAST; 
	else action = UNKNOWN;

	if (action == UNKNOWN) {
		printf(RED "Unknown command: %s\n" RESET, argv[1]);
		return -1;
	}

	char* path = get_lineote_file_path();

	if (access(path, F_OK) == -1) { // lineote file doesnt exist
		init_lineote(path);
	}

	if (action == NEW_ENTRY) {
		char* date = get_formatted_date();
		char* message = prompt_for_message();
		write_to_lineote(path, date, message);
		free(date);
		free(message);
	} else {
		char* lineote = read_lineote(path);

		if (action == VIEW_ENTRIES) {
			print_formatted_entries(lineote);
		} else if (action == VIEW_LAST) {
			if (argc >= 3) {
				char* end;
				long num_lines = strtol(argv[2], &end, 10);
				if (argv[2] == end) { // not a valid number
					printf(RED "Invalid number of last lines: %s\n" RESET, argv[2]); // print after the last newline
					return -1;
				} else {
					print_formatted_entries(get_last_n_lines(lineote, num_lines));
				}
			} else {
				print_formatted_entries(get_last_n_lines(lineote, 1));
			}
		}
	}

	free(path);

	return 0;
}
