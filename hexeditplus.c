#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Variables as required
char debug_mode = 0;
char file_name[128];
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count = 0;

// Function Prototypes
void toggle_debug_mode();
void set_file_name();
void set_unit_size();
void load_into_memory();
void toggle_display_mode();
void memory_display();
void save_into_file();
void memory_modify();
void quit();

// Menu Structure
struct fun_desc {
    char *name;
    void (*fun)();
};

// Menu Array
struct fun_desc menu[] = {
    { "Toggle Debug Mode", toggle_debug_mode },
    { "Set File Name", set_file_name },
    { "Set Unit Size", set_unit_size },
    { "Load Into Memory", load_into_memory },
    { "Toggle Display Mode", toggle_display_mode },
    { "Memory Display", memory_display },
    { "Save Into File", save_into_file },
    { "Memory Modify", memory_modify },
    { "Quit", quit },
    { NULL, NULL }
};

// Implementations

void toggle_debug_mode() {
    if (debug_mode == 0) {
        debug_mode = 1;
        fprintf(stderr, "Debug flag now on\n");
    } else {
        debug_mode = 0;
        fprintf(stderr, "Debug flag now off\n");
    }
}

void set_file_name() {
    printf("Please enter <file name>\n");
    // Getting input safely
    if (fgets(file_name, sizeof(file_name), stdin) != NULL) {
        // Remove trailing newline
        file_name[strcspn(file_name, "\n")] = 0;
        if (debug_mode) {
            fprintf(stderr, "Debug: file name set to '%s'\n", file_name);
        }
    }
}

void set_unit_size() {
    printf("Please enter unit size (1, 2, or 4):\n");
    int input;
    scanf("%d", &input);
    // Clear buffer residue
    getchar(); 

    if (input == 1 || input == 2 || input == 4) {
        unit_size = input;
        if (debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", unit_size);
        }
    } else {
        printf("Invalid unit size\n");
    }
}

void quit() {
    if (debug_mode) {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

// Stubs for future tasks
void load_into_memory() { printf("Not implemented yet\n"); }
void toggle_display_mode() { printf("Not implemented yet\n"); }
void memory_display() { printf("Not implemented yet\n"); }
void save_into_file() { printf("Not implemented yet\n"); }
void memory_modify() { printf("Not implemented yet\n"); }

int main(int argc, char **argv) {
    int choice;
    int bounds = 0;
    
    // Calculate menu size
    while(menu[bounds].name != NULL) {
        bounds++;
    }

    while (1) {
        if (debug_mode) {
            fprintf(stderr, "unit_size: %d, file_name: %s, mem_count: %zu\n", 
                    unit_size, file_name, mem_count);
        }

        printf("Choose action:\n");
        for (int i = 0; i < bounds; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }

        if (scanf("%d", &choice) != 1) {
             // Handle non-integer input to prevent infinite loop
             getchar(); 
             exit(1);
        }
        // Consume the newline after the number
        getchar();

        if (choice >= 0 && choice < bounds) {
            menu[choice].fun();
        } else {
            printf("Unknown option\n");
        }
        printf("\n");
    }
    return 0;
}