#include "memy.hpp"

#include <iostream>

memy::Process::Process(pid_t pid) {
    this->pid = pid;

    kern_return_t kr;

    kr = this->CreateTask();
    if (kr != KERN_SUCCESS) {
        std::cerr << "Error: Failed to create task: ";
        std::cerr << mach_error_string(kr) << std::endl;
        return;
    }

    kr = this->FindBaseAddress();
    if (kr != KERN_SUCCESS) {
        std::cerr << "Error: Failed to find base address: ";
        std::cerr << mach_error_string(kr) << std::endl;
        return;
    }

    std::cout << "Base address: 0x" << std::hex << this->baseAddress << std::dec << std::endl;
    // std::cout << "Base address: " << this->baseAddress << std::endl;
}

memy::Process::~Process() {

}

kern_return_t memy::Process::CreateTask() {
    return task_for_pid(mach_task_self(), this->pid, &this->task);
}

kern_return_t memy::Process::FindBaseAddress() {
    mach_vm_address_t vmoffset;
    vm_map_size_t vmsize;
    uint32_t nesting_depth = 0;
    struct vm_region_submap_info_64 vbr;
    mach_msg_type_number_t vbrcount = 16;

    kern_return_t kr = mach_vm_region_recurse(
        this->task, 
        &vmoffset, 
        &vmsize, 
        &nesting_depth, 
        (vm_region_recurse_info_64_t)&vbr, 
        &vbrcount
    );

    if (kr == KERN_SUCCESS) {
        this->baseAddress = vmoffset;
        // std::cout << "Base address: 0x" << std::hex << this->baseAddress << std::endl;
    }

    return kr;
}

mach_vm_size_t memy::Process::ReadMemory(mach_vm_address_t offset, mach_vm_size_t size, void* data) {
    
    mach_vm_size_t read_size;

    std::cout << "Task: " << this->task << std::endl;
    std::cout << "Address: 0x" << std::hex << (this->baseAddress+offset) << std::endl;
    std::cout << "Size: " << std::dec << size << std::endl;

    kern_return_t kr = mach_vm_read_overwrite(
        this->task,
        this->baseAddress+offset,
        size,
        (mach_vm_address_t)data,
        &read_size
    );

    if (kr != KERN_SUCCESS) {
        std::cerr << "Error reading memory: ";
        std::cerr << mach_error_string(kr) << std::endl;
        return 0;
    }

    return read_size;
}