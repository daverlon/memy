#include "memy.hpp"

#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " pid" << std::endl;
        return 1;
    }
    
    memy::Process mem((pid_t)std::stoi(argv[1]));

    mach_vm_address_t bp;
    mem.FindBaseAddress(&bp);
    // std::cout << "BP: 0x" << std::hex << bp << std::endl;

    mach_vm_address_t sp;
    mem.FindStackPointer(&sp);
    // std::cout << "SP: 0x" << std::hex << sp << std::endl;

    int x = 69;
    mem.WriteMemory(sp+0x24, &x, sizeof(int));

    int data = -1;
    mach_vm_size_t bytes_read = mem.ReadMemory(sp+0x24, (mach_vm_size_t)sizeof(int), &data);
    std::cout << bytes_read << "," << data << std::endl;

    // std::cout << *(int32_t*)data << std::endl;

    return 0;
}