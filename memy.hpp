#pragma once

#include <unistd.h>

#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/vm_map.h>

namespace memy {
    class Process;
};

class memy::Process 
{
public:
    Process(pid_t pid);
    ~Process();

    kern_return_t CreateTask();
    bool FindBaseAddress(mach_vm_address_t* address);
    bool FindStackPointer(mach_vm_address_t* address);
    mach_vm_size_t ReadMemory(mach_vm_address_t address, mach_vm_size_t size, void* data);
    void WriteMemory(mach_vm_address_t address, void* data, mach_vm_size_t size);

private:

    pid_t pid;
    mach_port_t task;
};