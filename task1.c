#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Variables as required
char debug_mode = 0;
char file_name[128];
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count = 0;
int display_mode = 0; // 0 = Hex, 1 = Decimal

// Formatting arrays for printing
static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

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

// --- Implementations ---

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

// --- Task 1 Implementations ---

void load_into_memory() {
    // 1. Check if file name is empty
    if (strcmp(file_name, "") == 0) {
        printf("Error: File name is empty\n");
        return;
    }

    // 2. Open file for reading
    FILE *f = fopen(file_name, "rb");
    if (f == NULL) {
        printf("Error: Failed to open file\n");
        return;
    }

    unsigned int location;
    int length;

    // 3. Prompt user
    printf("Please enter <location> <length>\n");
    char input[100];
    fgets(input, 100, stdin);
    sscanf(input, "%x %d", &location, &length);

    if (debug_mode) {
        fprintf(stderr, "Debug: file_name: %s, location: %x, length: %d\n", file_name, location, length);
    }

    // 4. Load data
    fseek(f, location, SEEK_SET);
    fread(mem_buf, unit_size, length, f);
    
    printf("Loaded %d units into memory\n", length);

    fclose(f);
}

void toggle_display_mode() {
    if (display_mode == 0) {
        display_mode = 1;
        printf("Decimal display flag now on, decimal representation\n");
    } else {
        display_mode = 0;
        printf("Decimal display flag now off, hexadecimal representation\n");
    }
}

void memory_display() {
    unsigned int addr;
    int u;

    printf("Enter address and length\n");
    char input[100];
    fgets(input, 100, stdin);
    sscanf(input, "%x %d", &addr, &u);

    // Handle special case 0 (start of mem_buf) vs real virtual address
    unsigned char *ptr;
    if (addr == 0) {
        ptr = mem_buf;
    } else {
        ptr = (unsigned char *)addr;
    }

    if (display_mode == 0) {
        printf("Hexadecimal\n===========\n");
    } else {
        printf("Decimal\n=======\n");
    }

    for (int i = 0; i < u; i++) {
        // Retrieve value based on unit size
        int val = 0;
        if (unit_size == 1) {
            val = *((unsigned char*)ptr);
        } else if (unit_size == 2) {
            val = *((unsigned short*)ptr);
        } else if (unit_size == 4) {
            val = *((unsigned int*)ptr);
        }

        // Print using the appropriate format array
        if (display_mode == 0) { // Hex
            printf(hex_formats[unit_size - 1], val);
        } else { // Decimal
            printf(dec_formats[unit_size - 1], val);
        }

        ptr += unit_size;
    }
}

void save_into_file() {
    if (strcmp(file_name, "") == 0) {
        printf("Error: File name is empty\n");
        return;
    }

    // Open with r+ to allow writing without truncating (clearing) the file
    FILE *f = fopen(file_name, "r+");
    if (f == NULL) {
        printf("Error: Failed to open file for writing\n");
        return;
    }

    unsigned int source_addr;
    unsigned int target_loc;
    int length;

    printf("Please enter <source-address> <target-location> <length>\n");
    char input[100];
    fgets(input, 100, stdin);
    sscanf(input, "%x %x %d", &source_addr, &target_loc, &length);

    if (debug_mode) {
        fprintf(stderr, "Debug: source_addr: %x, target_loc: %x, length: %d\n", source_addr, target_loc, length);
    }

    // Check file size constraint
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    
    if (target_loc > file_size) {
        printf("Error: Target location exceeds file size\n");
        fclose(f);
        return;
    }

    // Determine source pointer
    unsigned char *ptr;
    if (source_addr == 0) {
        ptr = mem_buf;
    } else {
        ptr = (unsigned char *)source_addr;
    }

    // Write to file
    fseek(f, target_loc, SEEK_SET);
    fwrite(ptr, unit_size, length, f);

    fclose(f);
    printf("Saved %d units into file\n", length);
}

void memory_modify() {
    unsigned int location;
    unsigned int val;

    printf("Please enter <location> <val>\n");
    char input[100];
    fgets(input, 100, stdin);
    sscanf(input, "%x %x", &location, &val); // Note: val is hex input

    if (debug_mode) {
        fprintf(stderr, "Debug: location: %x, val: %x\n", location, val);
    }

    // Modify memory based on unit size
    if (unit_size == 1) {
        mem_buf[location] = (unsigned char)val;
    } else if (unit_size == 2) {
        *((unsigned short*)&mem_buf[location]) = (unsigned short)val;
    } else if (unit_size == 4) {
        *((unsigned int*)&mem_buf[location]) = (unsigned int)val;
    }
}

// --- Main Loop ---

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