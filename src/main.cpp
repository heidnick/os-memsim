#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"
#include <sstream>
#include <vector>

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool createCalled);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {

        std::string delimeter = " ";
        size_t pos = 0;
        std::string token;
        std::vector<std::string> cmd_line;

        while ((pos = command.find(delimeter)) != std::string::npos) {
            token = command.substr(0, pos);
            cmd_line.push_back(token);
            command.erase(0, pos + delimeter.length());
        }
        cmd_line.push_back(command);

        if (cmd_line[0] == "create") {
            createProcess(stoi(cmd_line[1]), stoi(cmd_line[2]), mmu, page_table);
        }else if (cmd_line[0] == "allocate") {
            //FreeSpace, Char, Short, Int, Float, Long, Double 
            DataType type;
            if (cmd_line[3] == "freespace") {
                type = FreeSpace;
            }else if (cmd_line[3] == "short") {
                type = Short;
            }else if (cmd_line[3] == "char") {
                type = Char;
            }else if (cmd_line[3] == "float") {
                type = Float;
            }else if (cmd_line[3] == "long") {
                type = Long;
            }else if (cmd_line[3] == "double") {
                type = Double;
            }else if (cmd_line[3] == "int") {
                type = Int;
            }else {
                std::cout << "Error: DataType: " << cmd_line[3] << " invalid." << std::endl;
            }
            allocateVariable(stoi(cmd_line[1]), cmd_line[2], type, stoi(cmd_line[4]), mmu, page_table, false);
        }else if (cmd_line[0] == "set") {
            setVariable(stoi(cmd_line[1]), cmd_line[2], stoi(cmd_line[3]), &cmd_line, mmu, page_table, memory);
        }else if (cmd_line[0] == "free") {
            freeVariable(stoi(cmd_line[1]), cmd_line[2], mmu, page_table);  
        }else if (cmd_line[0] == "terminate") {
           terminateProcess(stoi(cmd_line[1]), mmu, page_table);
        }else if (cmd_line[0] == "print"){
            if(cmd_line[1] == "page")
            {
                page_table->print();
            }
        }else {
            std::cout << "Error: Command: " << cmd_line[0] << " invalid." << std::endl;
        }

        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }

    // Clean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - create new process in the MMU
    uint32_t pid = mmu->createProcess();
    //   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(pid, "<TEXT>", Char, text_size, mmu, page_table, true);
    allocateVariable(pid, "<GLOBALS>", Char, data_size, mmu, page_table, true);
    allocateVariable(pid, "<STACK>", Char, 65536, mmu, page_table, true);
    //   - print pid
    std::cout << pid << std::endl;
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool createCalled)
{
    //page_table->addEntry(1024, 9);

    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    Process *proc = mmu->getProcess(pid);

    /*for (int i=0; i<page_table.size(); i++) {
        if (page_table[i]->)
    }*/

    for (int i=0; i<proc->variables.size(); i++) {
        if (proc->variables[i]->type == FreeSpace) {
            int size = num_elements;
            if (type == Short) {
                size = size * 2;
            }else if (type == Int || type == Float) {
                size = size * 4;
            }else if (type == Long || type == Double) {
                size = size * 8;
            }
            //std::cout << "size: " << size << std::endl;
            if (proc->variables[i]->size >= size) {
                
                //size is dependent on type
                int current_vadd = proc->variables[i]->virtual_address;
                mmu->addVariableToProcess(pid, var_name, type, size, proc->variables[i]->virtual_address);
                int new_size = proc->variables[i]->size - size;
                if (new_size == 0) {
                    mmu->deleteFreeSpace(pid, proc->variables[i]->name, proc->variables[i]->virtual_address);
                }else {
                    mmu->modifyVariableToProcess(pid, proc->variables[i]->name, new_size, proc->variables[i]->virtual_address + size);
                }
                if (!createCalled) {
                    std::cout << current_vadd << std::endl;
                }
            }
        }
    }
    
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}
