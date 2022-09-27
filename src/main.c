#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include "csv_parse.h"

#define TODO_FILE       "/.todo.csv" //TODO ~/.todo.csv


char *file; //TODO this could cause an error, but it shouldn't

void saveTodo(csv_data_t csv) {
    int f = open(file, O_WRONLY | O_TRUNC | O_CREAT);
    for(int j = 0; j < csv.cols; j++) {
        write(f, csv.data[0][j], strlen(csv.data[0][j]));
        if(j+1 != csv.cols) write(f, ",", 1);
    }
    close(f);
}

void removeTodo(csv_data_t *csv, int task_num) {
	task_num--;
    for(int j = task_num; j < csv->cols; j++) {
    	if(csv->cols == j+1) {
    		csv->data[0][j] = NULL;
		    csv->cols--;
		    break;
    	} else {
	        if(csv->data[0][j+1] == NULL) {
	            memset(csv->data[0][j], '\0', strlen(csv->data[0][j]));
	        } else {
	            csv->data[0][j] = realloc(csv->data[0][j], strlen(csv->data[0][j+1])+1);
	            strcpy(csv->data[0][j], csv->data[0][j+1]);
	        }
        }
    }
}

void addTodo(csv_data_t *csv, char *value) {
    csv->data[0][csv->cols] = malloc( strlen(value)+1);
    strcpy(csv->data[0][csv->cols], value);
    csv->cols++;
}

void listTodo(csv_data_t csv) {
    for(int j = 0; j < csv.cols; j++) {
        printf("%d. %s\n", j+1, csv.data[0][j]);
    }
}

int main(int argc, char *argv[]) {
    file = malloc(4096);
    memset(file, '\0', 4096);
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    strcat(file, pw->pw_dir);
    strcat(file, TODO_FILE);
    FILE *f = fopen(file, "a+");
    fclose(f);
    
    csv_data_t csv = parse(file);

    if(argc == 1) {
        if(csv.rows != 0) listTodo(csv);
        else printf("Nothing To Do\n");
    } else if(strcmp(argv[1], "add") == 0 || strcmp(argv[1], "a") == 0) {
        addTodo(&csv, argv[2]);
        saveTodo(csv);
    } else if(strcmp(argv[1], "remove") == 0 || strcmp(argv[1], "rm") == 0 || strcmp(argv[1], "r") == 0) {
        int remove = atoi(argv[2]);
        removeTodo(&csv, remove);
        saveTodo(csv);
    }
	return 0;
}
