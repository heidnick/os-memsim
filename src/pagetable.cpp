#include "pagetable.h"
#include <iomanip>

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    //std::cout<< "page num: " << page_number << std::endl;
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    int frame = 0; 
    // Find free frame
    // TODO: implement this!
    std::map<std::string, int>::iterator it;
    bool num_list[67108864/_page_size];
    for (it = _table.begin(); it != _table.end(); it++)
    {
        num_list[it->second] = true;
    }
    int i=0;
    bool done = false;
    while (!done){
        if (!num_list[i]) {
            frame = i;
            done = true;
        }
        i++;
    }
    _table[entry] = frame;
    
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    int page_number = 0;
    int page_offset = 0;

    page_offset = virtual_address % _page_size; 
    page_number = (virtual_address - page_offset) / _page_size;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
        address = _table.at(entry) * _page_size + page_offset;
    }

    return address;
}

int PageTable::getPageSize() {
    return _page_size;
}

std::map<std::string, int> PageTable::getPageTable() {
    return _table;
}

std::vector<std::string> PageTable::getSortedKeys() {
    return sortedKeys();
}


void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages
        std::string currkey = keys[i];
        std::size_t index = currkey.find("|");
        std::string currpid = currkey.substr(0, index);
        std::string currframe = currkey.substr(index+1, currkey.size());
        std::cout << std::setw(5) << currpid << " |" << std::setw(12) << currframe << " |" << std::setw(13) << _table[keys[i]] << std::endl;
    }
}

void PageTable::freePages(std::string pid) {
    
    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end();)
    {
        std::string d_limeter = "|";
        std::string it_pid;
        int pos = it->first.find(d_limeter);
        if (pos != std::string::npos) {
            it_pid = it->first.substr(0, pos);
        }
        if (it_pid == pid) {
            _table.erase(it++);
        }else {
            it++;
        }
    }
}

void PageTable::freePage(std::string pid, std::string page) {
    int count = 0;
    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        std::string d_limeter = "|";
        std::string it_pid;
        std::string it_page;
        int pos = it->first.find(d_limeter);
        if (pos != std::string::npos) {
            it_pid = it->first.substr(0, pos);
            it_page = it->first.substr(pos+1);
        }
        if (it_page == page) {
            count++;
        }
    }
    /*if (count == 1) {

    }*/

}