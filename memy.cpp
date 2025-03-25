#include "memy.hpp"

#include <iostream>

memy::Process::Process(pid_t pid) {
    this->pid = pid;

    if (kern_return_t kr = this->CreateTask() != KERN_SUCCESS)
    {
        std::cerr << "Failed to create task: ";
        std::cerr << mach_error_string(kr) << std::endl;
        return;
    }
}

memy::Process::~Process() {

}

kern_return_t memy::Process::CreateTask() {
    return task_for_pid(mach_task_self(), this->pid, &this->task);
}

bool memy::Process::FindBaseAddress(mach_vm_address_t* address) {
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
        *address = vmoffset;
        // std::cout << "Base address: 0x" << std::hex << this->baseAddress << std::endl;
        return true;
    }

    return false;
}

bool memy::Process::FindThreadState(arm_thread_state64_t* state) {
    thread_act_port_array_t threads;
    mach_msg_type_number_t thread_count;

    kern_return_t kr = task_threads(this->task, &threads, &thread_count);
    if (kr != KERN_SUCCESS) {
        std::cerr << "Failed to get threads: " << mach_error_string(kr);
        return false;
    }

    for (mach_msg_type_number_t i = 0; i < thread_count; i++) {
        thread_suspend(threads[i]);
    }

    bool found = false;
    arm_thread_state64_t temp_state;
    for (mach_msg_type_number_t i = 0; i < thread_count; i++) {
        mach_msg_type_number_t count = ARM_THREAD_STATE64_COUNT;
        kr = thread_get_state(
            threads[i], 
            ARM_THREAD_STATE64,
            (thread_state_t)&temp_state,
            &count
        );
        if (kr == KERN_SUCCESS) {
            *state = temp_state;
            found = true;
            break;
        }
    }
    
    for (mach_msg_type_number_t i = 0; i < thread_count; i++) {
        thread_resume(threads[i]);
    }

    for (mach_msg_type_number_t i = 0; i < thread_count; i++) {
        mach_port_deallocate(mach_task_self(), threads[i]);
    }
    vm_deallocate(mach_task_self(), (mach_vm_address_t)threads, sizeof(thread_act_t) * thread_count);

    return found;
}

mach_vm_size_t memy::Process::ReadMemory(mach_vm_address_t address, mach_vm_size_t size, void* data) {
    mach_vm_size_t read_size;
    kern_return_t kr = mach_vm_read_overwrite(
        this->task,
        address,
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

void memy::Process::WriteMemory(mach_vm_address_t address, void* data, mach_vm_size_t size) {
    kern_return_t kr = mach_vm_write(this->task, 
        address,
        (vm_offset_t)(data),
        (mach_msg_type_number_t)size
    );
    if (kr != KERN_SUCCESS) {
        std::cerr << "Error writing memory: " << mach_error_string(kr) << std::endl;
        return;
    }
} 