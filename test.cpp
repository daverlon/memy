#include "memy.hpp"

#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " pid" << std::endl;
        return 1;
    }
    
    memy::Process mem((pid_t)std::stoi(argv[1]));

    // uint64_t offset = 0x100003e0c - 0x10000000;
    mach_vm_address_t offset = 0x100003e0c - 0x10000000;
    void* data;
    mem.ReadMemory(offset, (mach_vm_size_t)4, data);

    // std::cout << *(int32_t*)data << std::endl;

    return 0;
}