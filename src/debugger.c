#include "lxgbc.h"
#include "ram.h"
#include "debugger.h"
#include "cpu.h"
#include "ppu.h"

static char add_breakpoint(const unsigned int, gbc_debugger *);
static char remove_breakpoint(const unsigned int, gbc_debugger *);
static gbc_breakpoint *find_breakpoint(const unsigned int, gbc_debugger *);
static char dump_ram(gbc_ram *, const char *);
static void print_debug(gbc_system *);


// Shows the current instruction and the instructions that follow it
static debug_box *dbox_instr(gbc_system *gbc) {
    
    // Allocate memory for the box
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INSTR_ROWS);

    // Set its properties
    box->height = DBOX_INSTR_ROWS;
    box->width = DBOX_INSTR_WIDTH;

    // Write to the row array
    unsigned int pointer = gbc->cpu->registers->PC;
    int i;
    for(i = 0; i < DBOX_INSTR_ROWS; i++) {

        // Allocate memory for the row
        box->rows[i] = calloc(DBOX_INSTR_WIDTH, sizeof(box->rows[i]));
        box->rows[i][0] = '\0';

        // Show the address
        char *label = malloc(8 + sizeof(int));
        sprintf(label, "0x%04X: ", pointer);
        strcat(box->rows[i], label);
        free(label);

        // If the instruction is the current instruction add an arrow
        if(i == 0) {
            strcat(box->rows[i], "-> ");
        } else {
            // If the instruction is at a breakpoint
            if(find_breakpoint(pointer, gbc->debugger)) {
                strcat(box->rows[i], "@  ");
            } else {
                strcat(box->rows[i], "   ");
            }
        }

        // Check the we aren't outside the highest memory address
        if(pointer > 0xFFFF) {
            strcat(box->rows[i], "OUT OF RANGE");
            pointer++;
        }
        // Avoid the cartridge header area, there are no instructions        
        else if(pointer >= 0x104 && pointer <= 0x14E) {
            strcat(box->rows[i], "CARTRIDGE HEADER");
            pointer++;
        }
        // Process opcodes as usual if not
        else {

            unsigned char opcode = read_byte(gbc->ram, pointer);

            // Find the instruction
            gbc_instruction instr = find_instr(opcode, pointer, gbc);
            unsigned char length = instr.length;

            // Read the operand and format the string
            if(length > 1 && opcode != 0xCB) {

                // Read the operand based on the length and increment the pointer
                unsigned short operand = 0;
                if(length == 2) {
                    operand = read_byte(gbc->ram, pointer + 1);

                    // If the operand is signed, get the two's complement
                    if(instr.signed_operand) {
                        operand = (~(operand - 1)) & 0x00FF;
                    }

                } else if(length == 3) {
                    operand = read_short(gbc->ram, pointer + 1);
                }

                // Format an copy the string
                char *row = malloc(sizeof(*row) * DBOX_INSTR_WIDTH);
                sprintf(row, instr.disassembly, operand);
                strcat(box->rows[i], row);  
            }
            // If there is no operand (or CB opcode which has no operand)
            else {

                // Copy the disassembly and increment the pointer
                strcat(box->rows[i], instr.disassembly);
            }

            pointer += instr.length;
        }
    }

    return box;
}

// Shows the content of the CPU registers
static debug_box *dbox_regis(gbc_system *gbc) {

    // Allocate memory for the box
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_REGIS_ROWS);

    // Set its properties
    box->height = DBOX_REGIS_ROWS;
    box->width = DBOX_REGIS_WIDTH;

    char *line = malloc(DBOX_REGIS_ROWS);

    // TODO: Replace this at some point
    sprintf(line, "A : %02X", gbc->cpu->registers->A);
    box->rows[0] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[0], line);

    sprintf(line, "B : %02X", gbc->cpu->registers->B);
    box->rows[1] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[1], line);

    sprintf(line, "C : %02X", gbc->cpu->registers->C);
    box->rows[2] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[2], line);

    sprintf(line, "D : %02X", gbc->cpu->registers->D);
    box->rows[3] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[3], line);

    sprintf(line, "E : %02X", gbc->cpu->registers->E);
    box->rows[4] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[4], line);

    sprintf(line, "H : %02X", gbc->cpu->registers->H);
    box->rows[5] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[5], line);

    sprintf(line, "L : %02X", gbc->cpu->registers->L);
    box->rows[6] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[6], line);

    sprintf(line, "AF: %04X", gbc->cpu->registers->AF);
    box->rows[7] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[7], line);

    sprintf(line, "BC: %04X", gbc->cpu->registers->BC);
    box->rows[8] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[8], line);

    sprintf(line, "DE: %04X", gbc->cpu->registers->DE);
    box->rows[9] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[9], line);

    sprintf(line, "HL: %04X", gbc->cpu->registers->HL);
    box->rows[10] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[10], line);

    sprintf(line, "SP: %04X", gbc->cpu->registers->SP);
    box->rows[11] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[11], line);
    
    sprintf(line, "PC: %04X", gbc->cpu->registers->PC);
    box->rows[12] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[12], line);

    sprintf(line, "IME: %d", gbc->cpu->registers->IME);
    box->rows[13] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[13], line);

    sprintf(line, "HALT: %d", gbc->cpu->is_halted);
    box->rows[14] = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(box->rows[14], line);

    // Free memory
    free(line);

    return box;
}

// Parses the F register and prints the values of each flag
static debug_box *dbox_flags(gbc_system *gbc) {

    // Allocate memory for the box
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_FLAGS_ROWS);

    // Set its properties
    box->height = DBOX_FLAGS_ROWS;
    box->width = DBOX_FLAGS_WIDTH;

    // Allocate memory for 4 rows
    int i;
    for(i = 0; i < 4; i++) {
        box->rows[i] = malloc(sizeof(char) * (DBOX_FLAGS_WIDTH + 1));
    } 
    
    // Create the rows
    sprintf(box->rows[0], "Z: %d", (gbc->cpu->registers->F >> 7) & 1);
    sprintf(box->rows[1], "N: %d", (gbc->cpu->registers->F >> 6) & 1); 
    sprintf(box->rows[2], "H: %d", (gbc->cpu->registers->F >> 5) & 1);
    sprintf(box->rows[3], "C: %d", (gbc->cpu->registers->F >> 4) & 1);
        
    return box;
}

// Shows varied info
static debug_box *dbox_info(gbc_system *gbc) {
   
    // Allocate memory for the box
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INFO_ROWS);
 
    // Set its properties
    box->height = DBOX_INFO_ROWS;
    box->width = DBOX_INFO_WIDTH;

    // Allocate memory for the rows
    int i;
    for(i = 0; i < DBOX_INFO_ROWS; i++) {
        box->rows[i] = malloc(sizeof(char) * (DBOX_INFO_WIDTH + 1));
    }

    unsigned short opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    // CB prefix
    if(opcode == 0xCB) {
        opcode = 0xCB00 | read_byte(gbc->ram, gbc->cpu->registers->PC + 1);
    }

    // Write some information
    strcpy(box->rows[0], "OPCODE:");
    sprintf(box->rows[1], "-> %04X", opcode);
    strcpy(box->rows[2], "LY:");
    sprintf(box->rows[3], "-> %02X", read_byte(gbc->ram, LY));

    return box;
}

// Prints a line of hyphens
static void print_separator(const int width) {
    int i = 0; 
    for(; i < width; printf("-"), i++);
    printf("\n");
}

// Prints all the selected dboxes to the screen
static void print_debug(gbc_system *gbc) {

    // Create an array of boxes
    // A box is a formatted debug table in the console window
    debug_box *boxes[DBOX_COUNT];
    boxes[0] = dbox_instr(gbc);
    boxes[1] = dbox_regis(gbc);
    boxes[2] = dbox_flags(gbc);
    boxes[3] = dbox_info(gbc);

    // Render the various boxes in columns of 2
    int i;
    for(i = 0; i < DBOX_COUNT; i += 2) {

        debug_box *box_one = boxes[i];
        debug_box *box_two = boxes[i + 1];

        print_separator(box_one->width + box_two->width + 8);
        int height = MAX(box_one->height, box_two->height);

        int j;
        for(j = 0; j < height; j++) {

            // If there is a row in the first column
            if(box_one->height > j) {
                printf("| %-*s |", box_one->width, box_one->rows[j]);
                free(box_one->rows[j]);
            } else {
                printf("| %-*s |", box_one->width, "");
            }

            // If there is a row in the second column
            if(box_two->height > j) {
                printf("| %-*s |", box_two->width, box_two->rows[j]);
                free(box_two->rows[j]);
            } else {
                printf("| %-*s |", box_two->width, "");
            }

            if(j < height) {
                printf("\n");
            }
        }

        print_separator(box_one->width + box_two->width + 8);

        free(box_one);
        free(box_two);
    }
}

// Handles debug commands and prints to the screen
void debug(gbc_system *gbc) {

    // If the emulator is running
    if(gbc->debugger->is_running) {

        gbc_breakpoint *bp = find_breakpoint(gbc->cpu->registers->PC, gbc->debugger);

        // If there is a breakpoint at this instruction
        if(bp) {
            gbc->debugger->is_running = 0;
            print_debug(gbc);
            printf(CYEL "Stopped at breakpoint 0x%04X\n" CNRM, bp->address);
        } else {
            return;
        }
    }

    int command;

    // Get a command from the user
    while((command = getchar()) != EOF) {
        
        if(gbc->debugger->should_print) {
            print_debug(gbc);
            gbc->debugger->should_print = 0;
        }
    
        switch(command) {

            // Help command
            case 'h':
                printf("\nAvailable Commands:\nn: Run current instruction\nb: Create a breakpoint\nd: Remove a breakpoint\nl: List all breakpoints\nr: Run\np: Print debug information\ns: Read byte in memory\nf: Dump RAM to a file\nq: Quit\n");
                continue;

            // Run current instruction
            case 'n':
                gbc->debugger->should_print = 1;
                return;

            // Add breakpoint command
            case 'b': {
                unsigned int address = 0x101;
                printf("\nNew breakpoint\nAddress (HEX) 0x");
                scanf("%x", &address);

                if(address <= 0xFFFF) {
                    
                    // Add the breakpoint 
                    if(add_breakpoint(address, gbc->debugger)) {
                        printf(CGRN "Breakpoint added at 0x%04X\n" CNRM, address);
                    } else {
                        printf(CRED "Breakpoint already exists\n" CNRM);
                    }
                } else {
                    printf(CRED "Out of range\n" CNRM);
                }

                continue;
            }

            // Remove breakpoint
            case 'd': {
                unsigned int address;
                printf("\nRemove breakpoint\nAddress (HEX) 0x");
                scanf("%x", &address);

                // Remove the breakpoint 
                if(remove_breakpoint(address, gbc->debugger)) {
                    printf(CGRN "Breakpoint at 0x%04X removed\n" CNRM, address);
                } else {
                    printf(CRED "Breakpoint does not exist\n" CNRM);
                }
                continue;
            }

            // List breakpoints
            case 'l': {

                gbc_breakpoint *item = gbc->debugger->breakpoint_head;
                int i = 1;
                while(item) {
                    printf("%d: 0x%04X\n", i, item->address);
                    item = item->next;
                    i++;
                }
                continue;
            }

            // Run command
            case 'r':
                gbc->debugger->is_running = 1;
                return;
            
            // Print debug information
            case 'p':
                print_debug(gbc);
                continue;

            // Read value at address in memory
            case 's': {
                unsigned int address;
                printf("\nRead byte\nAddress (HEX) 0x");
                scanf("%x", &address);

                if(address <= 0xFFFF) {
                    printf("Result: 0x%02X\n\n", read_byte(gbc->ram, address));
                } else {
                    printf(CRED "Out of range\n" CNRM);
                }

                continue;
            }

            // Dump RAM to a file
            case 'f': {
                printf("Filename: ");
                char *filename = malloc(sizeof(*filename) * RAM_DUMP_FILENAME_SIZE);
                getchar();
                fgets(filename, RAM_DUMP_FILENAME_SIZE, stdin);
                filename[strlen(filename) - 1] = '\0';

                if(dump_ram(gbc->ram, filename)) {
                    printf(CGRN "RAM dumped successfully to \"%s\"\n" CNRM, filename);    
                } else {
                    printf(CRED "RAM dump failed!\n" CNRM);
                }
                continue;
            }

            // Quit command
            case 'q':
                exit(0);
        }

        printf(DEBUG_PROMPT);
    }
}

// Prepares the debugger struct
void init_debugger(gbc_debugger *debugger) {
    
    // Set the defaults
    debugger->breakpoint_head = NULL;
    debugger->is_debugging = 0;
    debugger->is_running = 0;
    debugger->should_print = 0;
}

// Adds a breakpoint element to the breakpoint linked list
static char add_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    // Check for duplicate breakpoints
    gbc_breakpoint *found = find_breakpoint(address, debugger);
   
    if(found == NULL) {
        
        // Create a new breakpoint element
        gbc_breakpoint *new = malloc(sizeof(gbc_breakpoint));
        new->address = address;

        // If there are no breakpoints, set it as the head
        if(debugger->breakpoint_head == NULL) {
            new->next = NULL;
            debugger->breakpoint_head = new;
        } else {
            // Add the new breakpoint to the front of the linked list
            new->next = debugger->breakpoint_head;
            debugger->breakpoint_head = new;
        }
        
        return 1;
    }
    
    return 0; 
}

// Removes a breakpoint element from the breakpoint linked list
static char remove_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    // Remove the item from the linked list
    gbc_breakpoint *prev = NULL;
    gbc_breakpoint *curr= NULL;

    if(debugger->breakpoint_head != NULL) {
       
        // Go through all the breakpoints
        curr = debugger->breakpoint_head;
        while(curr) {
            
            // If the item matches 
            if(curr->address == address) {
                
                // If we are removing the head
                if(curr == debugger->breakpoint_head) {

                    // If there is only one
                    if(debugger->breakpoint_head->next == NULL) {
                        debugger->breakpoint_head = NULL;
                    } else {
                        debugger->breakpoint_head = curr->next;
                    }
                } 
                // If there is more than one 
                else {
                    prev->next = curr->next; 
                }

                free(curr);
                return 1;
            }
            
            prev = curr;
            curr = curr->next;
        }

    }

    return 0;
}

// Returns a pointer to the breakpoint element with the specifed address in the breakpoint linked list provided it exists
static gbc_breakpoint *find_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    if(debugger->breakpoint_head != NULL) {

        // Iterate through the breakpoints and find one with the correct address
        gbc_breakpoint *item = debugger->breakpoint_head;
        while(item) {
            
            if(item->address == address) {
                return item;
            }

            item = item->next;
        }
    }

    return NULL; 
}

// Dumps the contents of the RAM into a binary file
static char dump_ram(gbc_ram *ram, const char *filename) {
   
    // Open the file for writing
    FILE *fp = NULL;
    if((fp = fopen(filename, "w")) == NULL) {
        return 0; 
    }
   
    // Dump the ram to a file
    unsigned short pointer = 0x0;
    for(; pointer < 0xFFFF; pointer++) {
        
        // TODO: Remove this
        if((pointer >= 0xA000 && pointer <= 0xBFFF) ||
           (pointer >= 0xFE00)) {
            fputc(0, fp); 
        } else {
            fputc(read_byte(ram, pointer), fp);
        }
    }
    fclose(fp);

    return 1;
}
