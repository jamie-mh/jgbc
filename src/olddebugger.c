#include "lxgbc.h"
#include "ram.h"
#include "rom.h"
#include "debugger.h"
#include "cpu.h"
#include "ppu.h"

static bool add_breakpoint(const unsigned int, gbc_debugger *);
static bool remove_breakpoint(const unsigned int, gbc_debugger *);
static gbc_breakpoint *find_breakpoint(const unsigned int, gbc_debugger *);

static debug_box *dbox_instr(gbc_system *);
static debug_box *dbox_regis(gbc_system *);
static debug_box *dbox_flags(gbc_system *);
static debug_box *dbox_info(gbc_system *);

static void print_debug(gbc_system *);
static void print_box_row(debug_box *, const int);
static void print_separator(const int);

static bool dump_ram(gbc_ram *, const char *);


// Shows the current instruction and the instructions that follow it
static debug_box *dbox_instr(gbc_system *gbc) {
    
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INSTR_ROWS);

    box->height = DBOX_INSTR_ROWS;
    box->width = DBOX_INSTR_WIDTH;

    unsigned int pointer = gbc->cpu->registers->PC;

    for(int i = 0; i < DBOX_INSTR_ROWS; i++) {

        box->rows[i] = calloc(DBOX_INSTR_WIDTH, sizeof(box->rows[i]));
        box->rows[i][0] = '\0';

        // Format the address in hexadecimal 
        char label[13];
        sprintf(label, "0x%04X: ", pointer);
        strcat(box->rows[i], label);

        // If the instruction is the current instruction add an arrow
        if(i == 0) {
            strcat(box->rows[i], "-> ");
        } else {

            // If the instruction is at a breakpoint
            if(find_breakpoint(pointer, gbc->debugger)) {
                strcat(box->rows[i], "*  ");
            } else {
                strcat(box->rows[i], "   ");
            }
        }

        if(pointer > 0xFFFF) {
            strcat(box->rows[i], "OUT OF RANGE");
            pointer++;
        }
        else if(pointer > ROM_HEADER_START + 1 && pointer < ROM_HEADER_END) {
            strcat(box->rows[i], "CARTRIDGE HEADER");
            pointer++;
        }
        else {

            unsigned char opcode = read_byte(gbc->ram, pointer);

            gbc_instruction instr = find_instr(opcode, pointer, gbc);
            unsigned char length = instr.length;

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

                char *row = malloc(sizeof(char) * (DBOX_INSTR_WIDTH + 1));
                snprintf(row, DBOX_INSTR_WIDTH + 1, instr.disassembly, operand);
                strcat(box->rows[i], row);  
                free(row);
            }
            // If there is no operand (or CB opcode which have no operand)
            else {
                strcat(box->rows[i], instr.disassembly);
            }

            pointer += instr.length;
        }
    }

    return box;
}

// Shows the content of the CPU registers
static debug_box *dbox_regis(gbc_system *gbc) {

    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_REGIS_ROWS);

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

    free(line);
    return box;
}

// Parses the F register and prints the values of each flag
static debug_box *dbox_flags(gbc_system *gbc) {

    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_FLAGS_ROWS);

    box->height = DBOX_FLAGS_ROWS;
    box->width = DBOX_FLAGS_WIDTH;

    for(int i = 0; i < 4; i++) {
        box->rows[i] = malloc(sizeof(char) * (DBOX_FLAGS_WIDTH + 1));
    } 
    
    snprintf(box->rows[0], DBOX_FLAGS_WIDTH + 1, "Z: %c", '0' + get_flag('Z', gbc->cpu->registers->F));
    snprintf(box->rows[1], DBOX_FLAGS_WIDTH + 1, "N: %c", '0' + get_flag('N', gbc->cpu->registers->F)); 
    snprintf(box->rows[2], DBOX_FLAGS_WIDTH + 1, "H: %c", '0' + get_flag('H', gbc->cpu->registers->F));
    snprintf(box->rows[3], DBOX_FLAGS_WIDTH + 1, "C: %c", '0' + get_flag('C', gbc->cpu->registers->F));
        
    return box;
}

// Shows info such as the current opcode and vertical line being rendered
static debug_box *dbox_info(gbc_system *gbc) {
   
    debug_box *box = malloc(sizeof(*box));
    box->rows = malloc(sizeof(box->rows) * DBOX_INFO_ROWS);
 
    box->height = DBOX_INFO_ROWS;
    box->width = DBOX_INFO_WIDTH;

    for(int i = 0; i < DBOX_INFO_ROWS; i++) {
        box->rows[i] = malloc(sizeof(char) * (DBOX_INFO_WIDTH + 1));
    }

    unsigned short opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    if(opcode == 0xCB) {
        opcode = 0xCB00 | read_byte(gbc->ram, gbc->cpu->registers->PC + 1);
    }

    snprintf(box->rows[0], DBOX_INFO_WIDTH + 1, "OP: %04X", opcode);
    snprintf(box->rows[1], DBOX_INFO_WIDTH + 1, "LY: %02X", read_byte(gbc->ram, LY));
    snprintf(box->rows[2], DBOX_INFO_WIDTH + 1, "IE: %02X", read_byte(gbc->ram, IE));
    snprintf(box->rows[3], DBOX_INFO_WIDTH + 1, "IF: %02X", read_byte(gbc->ram, IF));

    return box;
}

// Prints a line of hyphens
static void print_separator(const int width) {
    for(int i = 0; i < width; printf("-"), i++);
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
    for(int i = 0; i < DBOX_COUNT; i += 2) {

        debug_box *box_one = boxes[i];
        debug_box *box_two = boxes[i + 1];

        print_separator(box_one->width + box_two->width + 8);
        const int height = MAX(box_one->height, box_two->height);

        for(int j = 0; j < height; j++) {

            print_box_row(box_one, j);
            print_box_row(box_two, j);

            if(j == box_one->height - 1) {
                free(box_one->rows); 
            }

            if(j == box_two->height - 1) {
                free(box_two->rows); 
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

// Prints a row of a debug box
static void print_box_row(debug_box *box, const int row) {

    if(box->height > row) {
        printf("| %-*s |", box->width, box->rows[row]);
        free(box->rows[row]);
    } else {
        printf("| %-*s |", box->width, "");
    }
}

// Handles debug commands and prints to the screen
void debug(gbc_system *gbc) {

    if(gbc->debugger->is_running) {

        gbc_breakpoint *bp = find_breakpoint(gbc->cpu->registers->PC, gbc->debugger);

        if(bp) {
            gbc->debugger->is_running = 0;
            print_debug(gbc);
            printf(CYEL "Stopped at breakpoint 0x%04X\n" CNRM, bp->address);
        } else {
            return;
        }
    }

    int command;

    while((command = getchar()) != EOF) {
        
        if(gbc->debugger->should_print) {
            print_debug(gbc);
            gbc->debugger->should_print = 0;
        }
    
        switch(command) {

            // Help command
            case 'h':
                printf("\nAvailable Commands:\nn: Run current instruction\nb: Create a breakpoint\nd: Remove a breakpoint\nl: List all breakpoints\nr: Run\np: Print debug information\ns: Read byte in memory\nw: Write byte in memory\nf: Dump RAM to a file\nq: Quit\n");
                continue;

            // Run current instruction
            case 'n':
                gbc->debugger->should_print = 1;
                return;

            // Add breakpoint
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

            // Run
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

            // Write value at address in memory
            case 'w': {

                unsigned int address;                    
                unsigned int value;

                printf("\nWrite byte\nAddress (HEX) 0x");
                scanf("%x", &address);

                if(address <= 0xFFFF) {

                    printf("Value: 0x");
                    scanf("%x", &value);

                    write_byte(gbc, address, value, 0);

                    printf(CGRN "Written %02X at %04X\n\n" CNRM, value, address);
                } else {
                    printf(CRED "Out of range\n" CNRM);
                }

                continue;
            }

            // Dump RAM to a file
            case 'f': {

                printf("Filename: ");
                char filename[RAM_DUMP_FILENAME_SIZE];
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

            // Quit
            case 'q':
                exit(0);
        }

        printf(DEBUG_PROMPT);
    }
}

// Prepares the debugger struct
void init_debugger(gbc_debugger *debugger) {
    
    debugger->breakpoint_head = NULL;
    debugger->is_debugging = false;
    debugger->is_running = false;
    debugger->should_print = false;
}

// Adds a breakpoint element to the breakpoint linked list
static bool add_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    const gbc_breakpoint *found = find_breakpoint(address, debugger);
   
    if(found == NULL) {
        
        gbc_breakpoint *new = malloc(sizeof(gbc_breakpoint));
        new->address = address;

        if(debugger->breakpoint_head == NULL) {
            new->next = NULL;
            debugger->breakpoint_head = new;
        } else {
            new->next = debugger->breakpoint_head;
            debugger->breakpoint_head = new;
        }
        
        return true;
    }
    
    return false; 
}

// Removes a breakpoint element from the breakpoint linked list
static bool remove_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    gbc_breakpoint *prev = NULL;
    gbc_breakpoint *curr= NULL;

    if(debugger->breakpoint_head != NULL) {
       
        curr = debugger->breakpoint_head;

        while(curr) {
            
            if(curr->address == address) {
                
                if(curr == debugger->breakpoint_head) {

                    if(debugger->breakpoint_head->next == NULL) {
                        debugger->breakpoint_head = NULL;
                    } else {
                        debugger->breakpoint_head = curr->next;
                    }
                } 
                else {
                    prev->next = curr->next; 
                }

                free(curr);
                return true;
            }
            
            prev = curr;
            curr = curr->next;
        }

    }

    return false;
}

// Returns a pointer to the breakpoint element with the specifed address in the breakpoint linked list provided it exists
static gbc_breakpoint *find_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    if(debugger->breakpoint_head != NULL) {

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
static bool dump_ram(gbc_ram *ram, const char *filename) {
   
    FILE *fp = NULL;
    if((fp = fopen(filename, "w")) == NULL) {
        return false; 
    }
   
    for(unsigned short pointer = 0x0; pointer < 0xFFFF; pointer++) {
        fputc(read_byte(ram, pointer), fp);
    }

    fclose(fp);
    return true;
}
