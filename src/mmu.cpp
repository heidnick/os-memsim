#include "mmu.h"
#include <iomanip>

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;

    if (proc != NULL)
    {
        int pos;
        for(int i = 0; i < proc->variables.size(); i++)
        {
            if(proc->variables[i]->virtual_address == var->virtual_address)
            {
                pos = i;
            }
        }
        proc->variables.insert(proc->variables.begin() + pos, var);
    }
}

void Mmu::modifyVariableToProcess(Variable *var, uint32_t new_size, uint32_t new_address) {
    var->size = new_size;
    var->virtual_address = new_address;

}

void Mmu::deleteFreeSpace(uint32_t pid, std::string var_name, uint32_t address) {
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }
    for (i=0; i<proc->variables.size(); i++) {
        if (proc->variables[i]->name == var_name && proc->variables[i]->size == address) {
            proc->variables.erase(proc->variables.begin() + i);
        }
    }
}


void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            if (_processes[i]->variables[j]->type != FreeSpace) {
                
                std::cout << std::setw(5) << _processes[i]->pid << " | " << std::left << std::setw(13) << _processes[i]->variables[j]->name << " |";
                std::cout << "   0x";
                std::cout << std::right << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << _processes[i]->variables[j]->virtual_address << " |" << std::dec;
                std::cout << std::setfill(' ') << std::setw(11) << _processes[i]->variables[j]->size<< std::endl;
            }
        }
    }
}

Process* Mmu::getProcess(uint32_t pid) {
    int i;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            return _processes[i];
        }
    }
    return NULL;
}

std::vector<Process*> Mmu::getProcesses() {
    return _processes;
}

void Mmu::deleteProcess(uint32_t pid) {
    bool found = 0;
    for (int i=0; i<_processes.size(); i++) {
        if (_processes[i]->pid == pid) {
            _processes.erase(_processes.begin() + i);
            found = 1;
        }
    }
    if (!found) {
        std::cout << "error: process not found" << std::endl;
    }
}

void Mmu::deleteVariable(uint32_t pid, std::string var_name) {
    bool pid_found, var_found = 0;
    for (int i=0; i<_processes.size(); i++) {
        if (_processes[i]->pid == pid) {
            pid_found = 1;
            for (int j=0; j<_processes[i]->variables.size(); j++) {
                if (_processes[i]->variables[j]->name == var_name) {
                    var_found = 1;
                    _processes[i]->variables.erase(_processes[i]->variables.begin() + j);
                }
            }
        }
    }
    if (!pid_found) {
        std::cout << "error: process not found" << std::endl;
    }else if (!var_found) {
        std::cout << "error: variable not found" << std::endl;
    }
}


