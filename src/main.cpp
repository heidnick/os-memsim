#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"
#include <sstream>
#include <vector>
#include <stdint.h>

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool createCalled);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void printVarName(void *memory, int physical_address, DataType type);


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
        DataType type;
        if (cmd_line[0] == "create") {
            createProcess(stoi(cmd_line[1]), stoi(cmd_line[2]), mmu, page_table);
        }else if (cmd_line[0] == "allocate") {
            //FreeSpace, Char, Short, Int, Float, Long, Double 
            
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
            int offset = stoi(cmd_line[3]);
            int count = 0;
            //std::cout << cmd_line.size() << std::endl;
            for (int i = 4; i < cmd_line.size(); i++) {
                if(type == Char){
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), (char*)cmd_line[i].c_str(), mmu, page_table, memory);
                }
                else if(type == Float){
                    float value = stof(cmd_line[i]);
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), &value, mmu, page_table, memory);
                }
                else if(type == Int){
                    int value = stoi(cmd_line[i]);
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), &value, mmu, page_table, memory);
                }
                else if(type == Double){
                    double value = stod(cmd_line[i]);
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), &value, mmu, page_table, memory);
                }
                else if(type == Short){
                    short value = stoi(cmd_line[i]);
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), &value, mmu, page_table, memory);
                }
                else if(type == Long){
                    long value = stol(cmd_line[i]);
                    setVariable(stoi(cmd_line[1]), cmd_line[2], (uint32_t)(offset + count), &value, mmu, page_table, memory);
                }
                count++;
            }
        }else if (cmd_line[0] == "free") {
            freeVariable(stoi(cmd_line[1]), cmd_line[2], mmu, page_table);  
        }else if (cmd_line[0] == "terminate") {
           terminateProcess(stoi(cmd_line[1]), mmu, page_table);
        }else if (cmd_line[0] == "print"){
            if(cmd_line[1] == "page")
            {
                page_table->print();
            }
            else if(cmd_line[1] == "mmu") {
                mmu->print();
            }else if (cmd_line[1] == "processes") {
                std::vector<Process*> _processes = mmu->getProcesses();
                for (int i=0; i<_processes.size(); i++) {
                    std::cout << _processes[i]->pid << std::endl;
                }
            }else {
                std::string d_limeter = ":";
                size_t pos = 0;
                int temp_pid;
                std::string var_name;
                pos = cmd_line[1].find(d_limeter);
                if (pos != std::string::npos) {
                    temp_pid = stoi(cmd_line[1].substr(0, pos));
                    var_name = cmd_line[1].substr(pos+1);
                
                    //now we have the pid, just print the variable
                    //std::cout << temp_pid << var_name << std::endl;
                    std::vector<Process*> _processes = mmu->getProcesses();
                    bool found = 0;
                    for (int i=0; i<_processes.size(); i++) {
                        if (_processes[i]->pid == temp_pid) {
                            for (int j=0; j<_processes[i]->variables.size(); j++) {
                                if (_processes[i]->variables[j]->name == var_name) {
                                    found = 1;
                                    type = _processes[i]->variables[j]->type;
                                    int type_size = 1;
                                    if (type == Short) {
                                        type_size = 2;
                                    }else if (type == Int || type == Float) {
                                        type_size = 4;
                                    }else if (type == Long || type == Double) {
                                        type_size = 8;
                                    }
                                    int size = _processes[i]->variables[j]->size/type_size;
                                    bool done, noComma = 0;
                                    int k = 0;
                                    while (k<4 && !done) {
                                        int physical_address = page_table->getPhysicalAddress(temp_pid, _processes[i]->variables[j]->virtual_address + k);
                                        //std::cout << memory << " " << k << std::endl;
                                        printVarName(memory, physical_address, type);
                                        k = k + 1;
                                        if (k>=size) {
                                            done = 1;
                                        }
                                        else
                                        {
                                            std::cout << ", ";
                                        }
                                    }
                                    if (!done)  {
                                        std::cout << "... [" << size << " items]" << std::endl;
                                    }else {
                                        std::cout << std::endl;
                                    }
                                }
                            }
                        }
                    }


                }else {
                    std::cout << "error: command not recognized" << std::endl;
                }
                
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

    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    Process *proc = mmu->getProcess(pid);
    /*::map<std::string, int> p_table = page_table->getPageTable();
    for (int i=0; i<p_table.size(); i++) {
        if (p_table[i])
    }*/
    //std::vector<std::string> keys = page_table->sortedKeys();
    int size = num_elements;
    int type_size = 1;
    if (type == Short) {
        type_size = 2;
    }else if (type == Int || type == Float) {
        type_size = 4;
    }else if (type == Long || type == Double) {
        type_size = 8;
    }
    size = size * type_size;

    int variable_size = proc->variables.size();
    for (int i=0; i<variable_size; i++) {
        if (proc->variables[i]->type == FreeSpace) {
            int current_vadd = proc->variables[i]->virtual_address;
            int offset = (type_size -  (current_vadd % type_size)) % type_size;
            bool added_offset = false;
            //if there would be cross page conflicts
            if( size + (current_vadd % page_table->getPageSize()) > page_table->getPageSize() && offset != 0)
            {
                //increase the size, then check if the free space can still handle the offset
                size = size + offset;
                if(proc->variables[i]->size >= size)
                {
                    mmu->addVariableToProcess(pid, "<FREE_SPACE>", FreeSpace, offset, current_vadd);
                    current_vadd = current_vadd + offset;
                    mmu->modifyVariableToProcess(proc->variables[i+1], proc->variables[i+1]->size - offset, current_vadd);          
                    i = i + 1;
                    added_offset = true;
                    
                }
            }
            if (proc->variables[i]->size >= size) {
                //size is dependent on type
                if(added_offset){
                    size = size - offset;
                }
                int starting_page = (int)current_vadd/page_table->getPageSize();
                int ending_page = (int)(current_vadd + size - 1) / page_table->getPageSize();
                mmu->addVariableToProcess(pid, var_name, type, size, current_vadd);

                if (current_vadd == 0) {
                    for (int j = starting_page; j<ending_page + 1; j++) {
                        page_table->addEntry(pid, j);
                    }
                }else {
                    if (current_vadd % page_table->getPageSize() != 0) {
                        starting_page++;
                    }
                    for (int j = starting_page; j<ending_page + 1; j++) {
                        page_table->addEntry(pid, j);
                    }
                }

                int new_size = proc->variables[i+1]->size - size;
                if (new_size == 0) {
                    mmu->deleteFreeSpace(pid, proc->variables[i+1]->name, current_vadd);
                }else {
                    mmu->modifyVariableToProcess( proc->variables[i+1], new_size, current_vadd + size);
                }
                if (!createCalled) {
                    std::cout << current_vadd << std::endl;
                }
            }
            //in case the free space wasnt enough to fit the variable when additional offset was needed, reset to original size
            size = size - offset;
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
    Process *proc = mmu->getProcess(pid);
    bool found = 0;
    int physical_address;
    int size = 0;
    DataType type;
    
    for (int i=0; i<proc->variables.size(); i++) {
        if (var_name == proc->variables[i]->name) {
            type = proc->variables[i]->type;
            physical_address = page_table->getPhysicalAddress(pid, proc->variables[i]->virtual_address + (offset * sizeof(type)));
            found = 1;
            
        }
    }
    if (!found) {
        std::cout << "error: variable not found" << std::endl;
    }else {
        if(type == Char)
        {
            memcpy(((char*)memory)+physical_address, value, 1);
        }
        if(type == Float)
        {
            memcpy(((float*)memory)+physical_address, value, 4);
        }
        if(type == Int)
        {
            memcpy(((int*)memory)+physical_address, value, 4);
        }
        if(type == Long)
        {
            memcpy(((long*)memory)+physical_address, value, 8);
        }
        if(type == Short)
        {
            memcpy(((short*)memory)+physical_address, value, 2);
        }
        if(type == Double)
        {
            memcpy(((double*)memory)+physical_address, value, 8);
        }
        //printf("%p\n", value);
        //std::cout << "size: " << sizeof(value) << " type: " << type << " value: " << (int*)value << std::endl;
        //std::cout << ((float*)memory)[physical_address] << std::endl;
        //std::cout << ((char*)memory)[physical_address] << std::endl;

    }
}

void printVarName(void *memory, int physical_address, DataType type) {
        //std::cout << "physical address: " << physical_address << std::endl;
        if(type == Char){
            std::cout << ((char*)memory)[physical_address];
        }
        else if(type == Float){
            std::cout << ((float*)memory)[physical_address];
        }
        else if(type == Int){
            std::cout << ((int*)memory)[physical_address];
        }
        else if(type == Short){
            std::cout << ((short*)memory)[physical_address];
        }
        else if(type == Long){
            std::cout << ((long*)memory)[physical_address];
        }
        else if(type == Double){
            std::cout << ((double*)memory)[physical_address];
        }
        //std::cout << *static_cast<std::string*>(ptr) << std::endl;
        //printf("%s\n", (char*)static_cast<std::string*>(ptr));
        //std::string *test = static_cast<std::string*>(ptr);
        //std::cout << "test " << *test << std::endl;
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
    int v_add;
    bool found = 0;
    Process *proc = mmu->getProcess(pid);
    for (int j=0; j<proc->variables.size(); j++) {
        if (proc->variables[j]->name == var_name) {
            found = 1;
            v_add = proc->variables[j]->virtual_address;
        }
    }
    int page_number = 0;
    int page_offset = 0;
    mmu->deleteVariable(pid, var_name);
    if (found) {
        page_offset = v_add % page_table->getPageSize();
        page_number = (v_add - page_offset) / page_table->getPageSize();
        std::string pid_string = std::to_string(pid);
        std::string page_string = std::to_string(page_number);
        page_table->freePage(pid_string,page_string);
    }

}  

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
    mmu->deleteProcess(pid);
    std::string pid_string = std::to_string(pid);
    page_table->freePages(pid_string);
}
