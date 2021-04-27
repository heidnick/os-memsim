#include "pagetable.h"

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
        address = entry.at(page_number) + page_offset;
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
        std::cout << currpid << " |       " << currframe << " |       " << _table[keys[i]] << std::endl;
        //printf("%.14s | %.14s | %.13s", currpid, currframe, _table[keys[i]]);
    }
}
