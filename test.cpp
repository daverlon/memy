#include "memy.hpp"

#include <iostream>

#define x(type,val) type val; &val

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " pid" << std::endl;
        return 1;
    }
    
    memy::Process mem((pid_t)std::stoi(argv[1]));

    arm_thread_state64_t state;
    mem.FindThreadState(&state);

    mach_vm_address_t bp;
    mem.FindBaseAddress(&bp);
    // std::cout << "BP: 0x" << std::hex << bp << std::endl;

    mach_vm_address_t sp = state.__sp;
    // std::cout << "SP: 0x" << std::hex << sp << std::endl;
    
    // int x = 69;
    // mem.WriteMemory(sp+0x24, &x, sizeof(int));

    int data = -1;
    mach_vm_size_t bytes_read = mem.ReadMemory(sp+0x28, (mach_vm_size_t)sizeof(int), &data);
    std::cout << bytes_read << "," << data << std::endl;

    // std::cout << *(int32_t*)data << std::endl;

    while (1) {
        mem.FindThreadState(&state);
        std::cout << state.__pc << std::endl;
        sleep(1);
    }


    return 0;
}