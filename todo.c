#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#define MAX_TASKS 100
#define MAX_TASK_LENGTH 256
#define MAX_PATH 260

typedef struct {
    char description[MAX_TASK_LENGTH];
    int keep_for_tomorrow;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int count;
    time_t last_run_date;
} TodoList;

char config_path[MAX_PATH];
char data_path[MAX_PATH];

void initialize_paths() {
    char* appdata = getenv("APPDATA");
    if (appdata == NULL) {
        printf("Error: Cannot get APPDATA directory\n");
        exit(1);
    }
    
    snprintf(config_path, MAX_PATH, "%s\\doit", appdata);
    _mkdir(config_path);
    
    snprintf(data_path, MAX_PATH, "%s\\tasks.dat", config_path);
}

void save_todo_list(TodoList* list) {
    FILE* file = fopen(data_path, "wb");
    if (file == NULL) {
        printf("Error: Cannot save todo list\n");
        return;
    }
    
    fwrite(list, sizeof(TodoList), 1, file);
    fclose(file);
}

int load_todo_list(TodoList* list) {
    FILE* file = fopen(data_path, "rb");
    if (file == NULL) {
        list->count = 0;
        list->last_run_date = 0;
        return 0;
    }
    
    fread(list, sizeof(TodoList), 1, file);
    fclose(file);
    return 1;
}

void print_tasks(TodoList* list) {
    if (list->count == 0) {
        printf("No tasks for today!\n");
        return;
    }
    
    for (int i = 0; i < list->count; i++) {
        printf("%d. %s %s\n", 
            i + 1, 
            list->tasks[i].description,
            list->tasks[i].keep_for_tomorrow ? "[Keep for tomorrow]" : "");
    }
    printf("\n");
}

void add_task(TodoList* list) {
    if (list->count >= MAX_TASKS) {
        printf("Maximum number of tasks reached!\n");
        return;
    }
    
    printf("Enter new task: ");
    char input[MAX_TASK_LENGTH];
    fgets(input, MAX_TASK_LENGTH, stdin);
    input[strcspn(input, "\n")] = 0;  // Remove newline
    
    if (strlen(input) > 0) {
        strncpy(list->tasks[list->count].description, input, MAX_TASK_LENGTH - 1);
        list->tasks[list->count].keep_for_tomorrow = 0;
        list->count++;
        save_todo_list(list);
    }
}

void mark_task_for_tomorrow(TodoList* list) {
    print_tasks(list);
    if (list->count == 0) return;
    
    printf("Enter task number to mark for tomorrow (1-%d): ", list->count);
    int task_num;
    if (scanf("%d", &task_num) == 1 && task_num >= 1 && task_num <= list->count) {
        list->tasks[task_num - 1].keep_for_tomorrow = 1;
        printf("Task marked for tomorrow!\n");
        save_todo_list(list);
    } else {
        printf("Invalid task number!\n");
        while (getchar() != '\n');  // Clear input buffer
    }
}

void process_new_day(TodoList* list) {
    TodoList new_list = {0};
    int new_count = 0;
    
    // Keep only tasks marked for tomorrow
    for (int i = 0; i < list->count; i++) {
        if (list->tasks[i].keep_for_tomorrow) {
            new_list.tasks[new_count] = list->tasks[i];
            new_list.tasks[new_count].keep_for_tomorrow = 0;  // Reset the flag
            new_count++;
        }
    }
    
    new_list.count = new_count;
    new_list.last_run_date = time(NULL);
    *list = new_list;
    save_todo_list(list);
}

int is_new_day(time_t last_run) {
    time_t now = time(NULL);
    struct tm* last_tm = localtime(&last_run);
    struct tm* now_tm = localtime(&now);
    
    return last_tm->tm_year != now_tm->tm_year ||
           last_tm->tm_mon != now_tm->tm_mon ||
           last_tm->tm_mday != now_tm->tm_mday;
}

void show_menu() {
    printf("1. Add new task\n");
    printf("2. Mark task for tomorrow\n");
    printf("3. Exit\n");
    printf("Choose an option: ");
}

int main(int argc, char* argv[]) {
    initialize_paths();
    TodoList list;
    load_todo_list(&list);
    
    // Check if it's a new day
    if (is_new_day(list.last_run_date)) {
        process_new_day(&list);
        printf("Welcome to a new day!\n");
    } else if (argc == 1) {  // No arguments provided
        printf("Already ran today. Use todo ls to show menu again.\n");
        return 0;
    }
    
    printf("=== DOIT ===\n");
    // Show menu if it's first run of the day or --force is used
    if (is_new_day(list.last_run_date) || (argc > 1 && strcmp(argv[1], "ls") == 0)) {
        int choice;
        do {
            print_tasks(&list);
            show_menu();
            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n');  // Clear input buffer
                continue;
            }
            while (getchar() != '\n');  // Clear input buffer
            
            switch (choice) {
                case 1:
                    add_task(&list);
                    break;
                case 2:
                    mark_task_for_tomorrow(&list);
                    break;
                case 3:
                    break;
                default:
                    printf("Invalid choice!\n");
            }
        } while (choice != 3);
    }
    
    return 0;
}
