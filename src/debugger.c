#include "lxgbc.h"
#include "debugger.h"
#include "cpu.h"
#include "ram.h"

static struct debug_box *dbox_instr(struct gbc_system **gbc, struct gbc_debugger **debugger) {
    
    // Allocate memory for the box
    struct debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INSTR_ROWS);

    // Set its properties
    box->height = DBOX_INSTR_ROWS;
    box->width = DBOX_INSTR_WIDTH;

    // Write to the row array
    unsigned int pointer = (*gbc)->registers->PC;
    int i;
    for(i = 0; i < DBOX_INSTR_ROWS; i++) {

        // Allocate memory for the row
        box->rows[i] = calloc(DBOX_INSTR_WIDTH, sizeof(box->rows[i]));
        box->rows[i][0] = '\0';

        // Show the address
        char *label = malloc(8);
        sprintf(label, "0x%04X: ", pointer);
        strcat(box->rows[i], label);
        free(label);

        // If the instruction is the current instruction add an arrow
        if(i == 0) {
            strcat(box->rows[i], "-> ");
        } else {
            // If the instruction is at a breakpoint
            if(find_breakpoint(pointer, debugger)) {
                strcat(box->rows[i], "@  ");
            } else {
                strcat(box->rows[i], "   ");
            }
        }

        // Check the we aren't outside the highest memory address
        if(pointer >= 0xFFFF) {
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

            // Read byte at the pointer and find the instruction
            struct gbc_instr instr = find_instr(read_byte(&(*gbc)->ram, pointer), gbc);

            // Read the operand and format the string
            if(instr.length > 1) {

                // Read the operand based on the length and increment the pointer
                unsigned short operand = 0;
                if(instr.length == 2) {
                    operand = read_byte(&(*gbc)->ram, pointer + 1);
                    pointer += 2;
                } else if(instr.length == 3) {
                    operand = read_short(&(*gbc)->ram, pointer + 1);
                    pointer += 3;
                }

                // Format an copy the string
                char *row = malloc(sizeof(*row) * DBOX_INSTR_WIDTH);
                sprintf(row, instr.disassembly, operand);

                strcat(box->rows[i], row);  
            }
            // If there is no operand
            else {
                // Copy the disassembly and increment the pointer
                strcat(box->rows[i], instr.disassembly);
                pointer++;
            }
        }
    }

    return box;
}

static struct debug_box *dbox_regis(struct gbc_system **gbc, struct gbc_debugger **debugger) {

    // Allocate memory for the box
    struct debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_REGIS_ROWS);

    // Set its properties
    box->height = DBOX_REGIS_ROWS;
    box->width = DBOX_REGIS_WIDTH;

    char *line = malloc(DBOX_REGIS_ROWS);

    // TODO: Replace this at some point
    sprintf(line, "A : %02X", (*gbc)->registers->A);
    box->rows[0] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[0], line);

    sprintf(line, "B : %02X", (*gbc)->registers->B);
    box->rows[1] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[1], line);

    sprintf(line, "C : %02X", (*gbc)->registers->C);
    box->rows[2] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[2], line);

    sprintf(line, "D : %02X", (*gbc)->registers->D);
    box->rows[3] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[3], line);

    sprintf(line, "E : %02X", (*gbc)->registers->E);
    box->rows[4] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[4], line);

    sprintf(line, "H : %02X", (*gbc)->registers->H);
    box->rows[5] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[5], line);

    sprintf(line, "L : %02X", (*gbc)->registers->L);
    box->rows[6] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[6], line);

    sprintf(line, "AF: %04X", (*gbc)->registers->AF);
    box->rows[7] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[7], line);

    sprintf(line, "DE: %04X", (*gbc)->registers->DE);
    box->rows[8] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[8], line);

    sprintf(line, "HL: %04X", (*gbc)->registers->HL);
    box->rows[9] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[9], line);

    sprintf(line, "PC: %04X", (*gbc)->registers->PC);
    box->rows[10] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[10], line);

    sprintf(line, "SP: %04X", (*gbc)->registers->SP);
    box->rows[11] = malloc(sizeof(char) * strlen(line));
    strcpy(box->rows[11], line);

    // Free memory
    free(line);

    return box;
}

static struct debug_box *dbox_flags(struct gbc_system **gbc, struct gbc_debugger **debugger) {

    // Allocate memory for the box
    struct debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_FLAGS_ROWS);

    // Set its properties
    box->height = DBOX_FLAGS_ROWS;
    box->width = DBOX_FLAGS_WIDTH;

    // Allocate memory for 4 rows
    int i;
    for(i = 0; i < 4; i++) {
        box->rows[i] = malloc(sizeof(char) * DBOX_FLAGS_WIDTH);
    } 
    
    // Create the rows
    sprintf(box->rows[0], "Z: %d", ((*gbc)->registers->F >> 7) & 1);
    sprintf(box->rows[1], "N: %d", ((*gbc)->registers->F >> 6) & 1); 
    sprintf(box->rows[2], "H: %d", ((*gbc)->registers->F >> 5) & 1);
    sprintf(box->rows[3], "C: %d", ((*gbc)->registers->F >> 4) & 1);
        
    return box;
}

static struct debug_box *dbox_info(struct gbc_system **gbc, struct gbc_debugger **debugger) {
   
    // Allocate memory for the box
    struct debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INFO_ROWS);
 
    // Set its properties
    box->height = DBOX_INFO_ROWS;
    box->width = DBOX_INFO_WIDTH;

    // Allocate memory for the rows
    int i;
    for(i = 0; i < DBOX_INFO_ROWS; i++) {
        box->rows[i] = malloc(sizeof(char) * DBOX_INFO_WIDTH);
    }

    // Write some information
    strcpy(box->rows[0], "OPCODE:");
    sprintf(box->rows[1], "-> %02X", read_byte(&(*gbc)->ram, (*gbc)->registers->PC));
    strcpy(box->rows[2], "INTERR:");
    sprintf(box->rows[3], "-> %d", (*gbc)->interrupts_enabled);

    return box;
}

static void print_separator(const int width) {
    int i = 0; 
    for(; i < width; printf("-"), i++);
    printf("\n");
}

static void print_debug(struct gbc_system **gbc, struct gbc_debugger **debugger) {

    // Create an array of boxes
    // A box is a formatted debug table in the console window
    struct debug_box *boxes[DBOX_COUNT];
    boxes[0] = dbox_instr(gbc, debugger);
    boxes[1] = dbox_regis(gbc, debugger);
    boxes[2] = dbox_flags(gbc, debugger);
    boxes[3] = dbox_info(gbc, debugger);

    // Render the various boxes in columns of 2
    int i;
    for(i = 0; i < DBOX_COUNT; i += 2) {

        struct debug_box *box_one = boxes[i];
        struct debug_box *box_two = boxes[i + 1];

        print_separator(box_one->width + box_two->width + 8);
        int height = max(box_one->height, box_two->height);

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

void debug(struct gbc_system **gbc, struct gbc_debugger **debugger) {

    // If the emulator is running
    if((*debugger)->running) {

        struct breakpoint *bp = find_breakpoint((*gbc)->registers->PC, debugger);

        // If there is a breakpoint at this instruction
        if(bp) {
            (*debugger)->running = 0;
            print_debug(gbc, debugger);
            printf(CYEL "Stopped at breakpoint 0x%04X\n" CNRM, bp->address);
            remove_breakpoint(bp->address, debugger);
        } else {
            return;
        }
    }

    // If we need to skip some instructions
    if((*debugger)->skip_instr > 0) {
        (*debugger)->skip_instr--;
        return;
    }

    int command;

    // Get a command from the user
    while((command = getchar()) != EOF) {
        
        if((*debugger)->print) {
            print_debug(gbc, debugger);
            (*debugger)->print = 0;
        }
    
        switch(command) {

            // Help command
            case 'h':
                printf("\nAvailable Commands:\n(1-9): Run X instructions\nb: Create a breakpoint\nd: Remove a breakpoint\nl: List all breakpoints\nr: Run\np: Print debug information\nq: Quit\n");
                continue;

            // Number input to run for X instructions before stopping
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':

                // Subtract '0' to get the digit
                (*debugger)->skip_instr = command - '0' - 1;
                (*debugger)->print = 1;
                return;

            // Add breakpoint command
            case 'b': {
                unsigned int address;
                printf("\nNew breakpoint\nAddress (HEX) 0x");
                scanf("%x", &address);

                // Add the breakpoint 
                if(add_breakpoint(address, debugger)) {
                    printf(CGRN "Breakpoint added at 0x%04X\n" CNRM, address);
                } else {
                    printf(CRED "Breakpoint already exists\n" CNRM);
                }
                continue;
            }

            // Remove breakpoint
            case 'd': {
                unsigned int address;
                printf("\nRemove breakpoint\nAddress (HEX) 0x");
                scanf("%x", &address);

                // Remove the breakpoint 
                if(remove_breakpoint(address, debugger)) {
                    printf(CGRN "Breakpoint at 0x%04X removed\n" CNRM, address);
                } else {
                    printf(CRED "Breakpoint does not exist\n" CNRM);
                }
                continue;
            }

            // List breakpoints
            case 'l': {

                struct breakpoint *item = (*debugger)->breakpoint_head;
                int i = 1;
                while(item) {
                    printf("%d: 0x%04X\n", i, item->address);
                    item = item->next;
                    i++;
                }
                continue;
            }

            // Print debug information
            case 'p':
                print_debug(gbc, debugger);
                continue;

            // Run command
            case 'r':
                (*debugger)->running = 1;
                return;

            // Quit command
            case 'q':
                exit(0);
        }

        printf("(dbg) ");
    }
}

void init_debugger(struct gbc_debugger **debugger) {
    
    // Set the defaults
    (*debugger)->breakpoint_head = NULL;
    (*debugger)->breakpoint_count = 0;
    (*debugger)->skip_instr = 0;
    (*debugger)->running = 0;

    // Start the prompt
    printf("(dbg) ");
}

static char add_breakpoint(const unsigned short address, struct gbc_debugger **debugger) {

    // Check for duplicate breakpoints
    struct breakpoint *found = find_breakpoint(address, debugger);
   
    if(found == NULL) {
        
        // Create a new breakpoint element
        struct breakpoint *new = malloc(sizeof(*new));
        new->address = address;

        // If there are no breakpoints, set it as the head
        if((*debugger)->breakpoint_count == 0) {
            new->next = NULL;
            (*debugger)->breakpoint_head = new;
        } else {
            // Add the new breakpoint to the front of the linked list
            new->next = (*debugger)->breakpoint_head;
            (*debugger)->breakpoint_head = new;
        }
        
        (*debugger)->breakpoint_count++;
        return 1;
    }
    
    return 0; 
}

static char remove_breakpoint(const unsigned short address, struct gbc_debugger **debugger) {

    // Remove the item from the linked list
    struct breakpoint *prev = NULL;
    struct breakpoint *curr= NULL;

    if((*debugger)->breakpoint_count > 0) {
       
        // Go through all the breakpoints
        curr = (*debugger)->breakpoint_head;
        while(curr) {
            
            // If the item matches 
            if(curr->address == address) {
                
                // If we are removing the head
                if(curr == (*debugger)->breakpoint_head) {

                    // If there is only one
                    if((*debugger)->breakpoint_count == 1) {
                        (*debugger)->breakpoint_head = NULL;
                    } else {
                        (*debugger)->breakpoint_head = curr->next;
                    }
                } 
                // If there is more than one 
                else {
                    prev->next = curr->next; 
                }

                free(curr);
                (*debugger)->breakpoint_count--;
                return 1;
            }
            
            prev = curr;
            curr = curr->next;
        }

    }

    return 0;
}

static struct breakpoint *find_breakpoint(const unsigned short address, struct gbc_debugger **debugger) {

    if((*debugger)->breakpoint_count > 0) {

        // Iterate through the breakpoints and find one with the correct address
        struct breakpoint *item = (*debugger)->breakpoint_head;
        while(item) {
            
            if(item->address == address) {
                return item;
            }

            item = item->next;
        }
    }

    return NULL; 
}
