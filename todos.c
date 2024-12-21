#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <windows.h>

#define MAX_TODO_LENGTH 256
#define MAX_TODOS 100
#define MAX_PATH_LENGTH 512
#define DATE_LENGTH 11

typedef struct {
    char date[DATE_LENGTH];
    char items[MAX_TODOS][MAX_TODO_LENGTH];
    int count;
} TodoList;

// Function declarations
void ensure_config_dir(char* config_path);
void get_config_path(char* config_path);
void get_today_date(char* date_str);
int load_todos(TodoList* todos, const char* config_path);
void save_todos(TodoList* todos, const char* config_path);
int should_show_todos(const char* config_path);
void update_last_check(const char* config_path);
void cleanup_old_todos(TodoList* todos);
void add_todo(TodoList* todos);
void remove_todo(TodoList* todos);
void show_todos(TodoList* todos);

int main() {
    char config_path[MAX_PATH_LENGTH];
    get_config_path(config_path);
    ensure_config_dir(config_path);

    if (!should_show_todos(config_path)) {
        return 0;
    }

    TodoList todos = {0};
    get_today_date(todos.date);
    
    if (load_todos(&todos, config_path)) {
        cleanup_old_todos(&todos);
    }

    printf("\n=== Today's Todos ===\n");
    
    if (todos.count == 0) {
        char input[MAX_TODO_LENGTH];
        printf("No todos for today. Add one? (Enter to skip): ");
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0;
            if (strlen(input) > 0) {
                strncpy(todos.items[todos.count++], input, MAX_TODO_LENGTH - 1);
            }
        }
    } else {
        while (1) {
            show_todos(&todos);
            printf("\nOptions:\n1. Add new todo\n2. Remove todo\n3. Done\nChoice (1-3): ");
            
            char choice[10];
            if (fgets(choice, sizeof(choice), stdin)) {
                choice[strcspn(choice, "\n")] = 0;
                
                switch (choice[0]) {
                    case '1':
                        add_todo(&todos);
                        break;
                    case '2':
                        remove_todo(&todos);
                        break;
                    case '3':
                        goto save_and_exit;
                    default:
                        printf("Invalid choice\n");
                }
            }
        }
    }

save_and_exit:
    save_todos(&todos, config_path);
    update_last_check(config_path);
    return 0;
}

void get_config_path(char* config_path) {
    char* home_path = getenv("USERPROFILE");
    snprintf(config_path, MAX_PATH_LENGTH, "%s\\.terminal_todo", home_path);
}

void ensure_config_dir(char* config_path) {
    _mkdir(config_path);
}

void get_today_date(char* date_str) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(date_str, DATE_LENGTH, "%Y-%m-%d", t);
}

int load_todos(TodoList* todos, const char* config_path) {
    char filepath[MAX_PATH_LENGTH];
    snprintf(filepath, MAX_PATH_LENGTH, "%s\\todos.txt", config_path);
    
    FILE* file = fopen(filepath, "r");
    if (!file) return 0;

    char line[MAX_TODO_LENGTH];
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        strncpy(todos->date, line, DATE_LENGTH - 1);
        
        while (fgets(line, sizeof(line), file) && todos->count < MAX_TODOS) {
            line[strcspn(line, "\n")] = 0;
            strncpy(todos->items[todos->count++], line, MAX_TODO_LENGTH - 1);
        }
    }
    
    fclose(file);
    return 1;
}

void save_todos(TodoList* todos, const char* config_path) {
    char filepath[MAX_PATH_LENGTH];
    snprintf(filepath, MAX_PATH_LENGTH, "%s\\todos.txt", config_path);
    
    FILE* file = fopen(filepath, "w");
    if (!file) return;

    fprintf(file, "%s\n", todos->date);
    for (int i = 0; i < todos->count; i++) {
        fprintf(file, "%s\n", todos->items[i]);
    }
    
    fclose(file);
}

int should_show_todos(const char* config_path) {
    char filepath[MAX_PATH_LENGTH];
    char today[DATE_LENGTH];
    char last_check[DATE_LENGTH] = {0};
    
    snprintf(filepath, MAX_PATH_LENGTH, "%s\\last_check.txt", config_path);
    get_today_date(today);
    
    FILE* file = fopen(filepath, "r");
    if (file) {
        if (fgets(last_check, DATE_LENGTH, file)) {
            last_check[strcspn(last_check, "\n")] = 0;
        }
        fclose(file);
        return strcmp(today, last_check) != 0;
    }
    return 1;
}

void update_last_check(const char* config_path) {
    char filepath[MAX_PATH_LENGTH];
    snprintf(filepath, MAX_PATH_LENGTH, "%s\\last_check.txt", config_path);
    
    FILE* file = fopen(filepath, "w");
    if (!file) return;

    char today[DATE_LENGTH];
    get_today_date(today);
    fprintf(file, "%s", today);
    fclose(file);
}

void cleanup_old_todos(TodoList* todos) {
    char today[DATE_LENGTH];
    get_today_date(today);
    
    if (strcmp(todos->date, today) != 0) {
        // If todos are from a previous date, keep them but update the date
        strncpy(todos->date, today, DATE_LENGTH - 1);
    }
}

void add_todo(TodoList* todos) {
    if (todos->count >= MAX_TODOS) {
        printf("Todo list is full!\n");
        return;
    }

    printf("Enter new todo: ");
    char input[MAX_TODO_LENGTH];
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(todos->items[todos->count++], input, MAX_TODO_LENGTH - 1);
        }
    }
}

void remove_todo(TodoList* todos) {
    if (todos->count == 0) {
        printf("No todos to remove!\n");
        return;
    }

    printf("Enter todo number to remove: ");
    char input[10];
    if (fgets(input, sizeof(input), stdin)) {
        int index = atoi(input) - 1;
        if (index >= 0 && index < todos->count) {
            for (int i = index; i < todos->count - 1; i++) {
                strcpy(todos->items[i], todos->items[i + 1]);
            }
            todos->count--;
        } else {
            printf("Invalid todo number!\n");
        }
    }
}

void show_todos(TodoList* todos) {
    printf("\nCurrent todos:\n");
    for (int i = 0; i < todos->count; i++) {
        printf("%d. %s\n", i + 1, todos->items[i]);
    }
}
