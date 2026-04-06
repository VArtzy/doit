#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#define MAX_TASKS 50
#define MAX_TASK_LENGTH 256
#define MAX_PATH 260

// Structure to store task information
typedef struct {
    char description[MAX_TASK_LENGTH];
    int keep_for_tomorrow;
} Task;

// Global variables
Task tasks[MAX_TASKS];
int task_count = 0;
char data_dir[MAX_PATH];
char last_run_file[MAX_PATH];
char tasks_file[MAX_PATH];

// Function declarations
void initialize_paths(void);
int should_run_today(void);
void update_last_run(void);
void load_tasks(void);
void save_tasks(void);
void add_task(void);
void list_tasks(void);
void mark_for_tomorrow(void);
void cleanup_tasks(void);
void create_directory_if_not_exists(const char* path);

int main(int argc) {  
    initialize_paths();

    // Check if program should run today
    int run = should_run_today();
    if (argc == 1 && !run) {
        return 0;
    }
    
    // Update last run timestamp
    update_last_run();
    
    // Load any tasks marked for today
    load_tasks();

    if (run) {
        cleanup_tasks();
    }
    
    printf("=== DOIT ===\n");
    
    // Main program loop
    char choice;
    while (1) {
        list_tasks(); 
        printf("1. Add\n");
        printf("2. Mark\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        
        scanf(" %c", &choice);
        getchar(); // Clear newline
        
        switch (choice) {
            case '1':
                add_task();
                break;
            case '2':
                mark_for_tomorrow();
                break;
            case '3':
                save_tasks();
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
    }
    
    return 0;
}

void initialize_paths(void) {
    char* appdata = getenv("APPDATA");
    if (appdata == NULL) {
        fprintf(stderr, "Could not get APPDATA directory\n");
        exit(1);
    }
    
    snprintf(data_dir, MAX_PATH, "%s\\doit", appdata);
    snprintf(last_run_file, MAX_PATH, "%s\\last_run.txt", data_dir);
    snprintf(tasks_file, MAX_PATH, "%s\\tasks.txt", data_dir);
    
    create_directory_if_not_exists(data_dir);
}

void create_directory_if_not_exists(const char* path) {
    if (_mkdir(path) != 0 && errno != EEXIST) {
        fprintf(stderr, "Error creating directory: %s\n", path);
        exit(1);
    }
}

int should_run_today(void) {
    FILE* fp;
    char last_date[11] = {0};
    time_t now;
    struct tm* tm_info;
    char today[11];
    
    time(&now);
    tm_info = localtime(&now);
    strftime(today, 11, "%Y-%m-%d", tm_info);
    
    // Check if last_run file exists and read last run date
    if ((fp = fopen(last_run_file, "r")) != NULL) {
        fgets(last_date, 11, fp);
        fclose(fp);
        
        // Compare dates
        if (strcmp(last_date, today) == 0) {
            return 0; // Already run today
        }
    }
    
    return 1; // Should run today
}

void update_last_run(void) {
    FILE* fp;
    time_t now;
    struct tm* tm_info;
    char today[11];
    
    time(&now);
    tm_info = localtime(&now);
    strftime(today, 11, "%Y-%m-%d", tm_info);
    
    fp = fopen(last_run_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error updating last run file\n");
        return;
    }
    
    fprintf(fp, "%s", today);
    fclose(fp);
}

void load_tasks(void) {
    FILE* fp;
    task_count = 0;
    
    fp = fopen(tasks_file, "r");
    if (fp == NULL) return;
    
    while (fgets(tasks[task_count].description, MAX_TASK_LENGTH, fp) != NULL && task_count < MAX_TASKS) {
        // Remove newline if present
        size_t len = strlen(tasks[task_count].description);
        if (len > 0 && tasks[task_count].description[len-1] == '\n') {
            tasks[task_count].description[len-1] = '\0';
        }

        if (strstr(tasks[task_count].description, "tomorrow") != NULL) {
            tasks[task_count].keep_for_tomorrow = 1;
        } else {
            tasks[task_count].keep_for_tomorrow = 0;
        }
        task_count++;
    }
    
    fclose(fp);
}

void save_tasks(void) {
    FILE* fp;
    int i;
    
    fp = fopen(tasks_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error saving tasks\n");
        return;
    }
    
    for (i = 0; i < task_count; i++) {
        if (tasks[i].keep_for_tomorrow) {
            if (strstr(tasks[i].description, "tomorrow") == NULL) {
                fprintf(fp, "%s -> tomorrow\n", tasks[i].description);
            } else {
                fprintf(fp, "%s\n", tasks[i].description);
            }
        } else {
            fprintf(fp, "%s\n", tasks[i].description);
        }
    }
    
    fclose(fp);
}

void add_task(void) {
    if (task_count >= MAX_TASKS) {
        printf("Maximum number of tasks reached!\n");
        return;
    }
    
    printf("Enter task description: ");
    fgets(tasks[task_count].description, MAX_TASK_LENGTH, stdin);
    
    // Remove newline if present
    size_t len = strlen(tasks[task_count].description);
    if (len > 0 && tasks[task_count].description[len-1] == '\n') {
        tasks[task_count].description[len-1] = '\0';
    }
    
    tasks[task_count].keep_for_tomorrow = 0;
    task_count++;
    
    printf("Task added successfully!\n");
}

void list_tasks(void) {
    if (task_count == 0) {
        printf("No tasks for today!\n\n");
        return;
    }
    
    for (int i = 0; i < task_count; i++) {
        printf("%d. %s\n", i + 1, tasks[i].description);
    }
    printf("\n");
}

void mark_for_tomorrow(void) {
    int task_num;
    
    list_tasks();
    
    if (task_count == 0) return;
    
    printf("\nEnter task number to keep for tomorrow: ");
    scanf("%d", &task_num);
    getchar(); // Clear newline
    
    if (task_num < 1 || task_num > task_count) {
        printf("Invalid task number!\n");
        return;
    }
    
    tasks[task_num - 1].keep_for_tomorrow = 1;
    printf("Task marked for tomorrow!\n");
}

void cleanup_tasks(void) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].keep_for_tomorrow) {
            tasks[i].keep_for_tomorrow = 0;
            tasks[i].description[strlen(tasks[i].description) - 12] = '\0';
        } else {
            for (int j = i; j < task_count - 1; j++) {
                strcpy(tasks[j].description, tasks[j + 1].description);
                tasks[j].keep_for_tomorrow = tasks[j + 1].keep_for_tomorrow;
            }
            task_count--;
            i--;
        }
    }

    save_tasks();
}
