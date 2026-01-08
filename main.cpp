#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "src/allocator/memory_manager.cpp"
#include "src/allocator/buddy_manager.cpp"
#include "src/allocator/first_fit.cpp"
#include "src/allocator/best_fit.cpp" 
#include "src/allocator/worst_fit.cpp"

#include "src/cache/fifo_policy.h"
#include "src/cache/lru_policy.h"
#include "include/cache_hierarchy.h"

#include "src/vm/fifo_page_policy.h"
#include "src/vm/lru_page_policy.h"
#include "include/vm/page_table.h"

#include "include/memory_system.h"

using namespace std;

void print_help() {
    cout << "\n=== COMMANDS ===" << endl;
    cout << "--- Memory Allocator ---" << endl;
    cout << "  init <size>             : Reset memory size (Clears all data)" << endl;
    cout << "  malloc <size>           : Allocate block (Physical RAM)" << endl;
    cout << "  free <id>               : Free block ID" << endl;
    cout << "  set allocator <mode>    : Switch (buddy, first, best, worst)" << endl;
    cout << "  dump                    : Show allocator memory map" << endl;
    cout << "  stats allocator         : Show Memory Utilization & Frag" << endl;

    cout << "\n--- CPU & Virtual Memory ---" << endl;
    cout << "  access <v_addr>         : Read Virtual Address (VA -> PA -> Cache)" << endl;
    cout << "  vm policy <fifo|lru>    : Set Page Replacement Policy" << endl;
    
    cout << "\n--- Cache System ---" << endl;
    cout << "  cache policy <lvl> <pol>: Set Cache Policy (L1/L2/L3, fifo/lru)" << endl;
    cout << "  stats system            : Show VM & Cache Stats" << endl;
    
    cout << "\n--- General ---" << endl;
    cout << "  exit                    : Quit" << endl;
}

int main() {
    size_t ram_size = 4096;
    size_t page_size = 64;

    FirstFit ff;
    BestFit bf;
    WorstFit wf;

    MemoryManager* list_mem = new MemoryManager(ram_size);
    list_mem->set_strategy(&ff);
    
    BuddyManager* buddy_mem = new BuddyManager(ram_size);
    bool use_buddy = false;

    FIFO_PagePolicy vm_fifo;
    LRU_PagePolicy vm_lru;
    PageTable* vm = new PageTable(page_size, ram_size, &vm_fifo); 

    LRU_Policy cache_lru;
    FIFO_Policy cache_fifo;
    CacheLevel l1(128, 64, 1, &cache_lru, 1);     
    CacheLevel l2(256, 64, 2, &cache_lru, 10);    
    CacheLevel l3(512, 64, 4, &cache_lru, 100);   
    CacheHierarchy cache_sys(&l1, &l2, &l3, 200);

    MemorySystem* sys = new MemorySystem(list_mem, &cache_sys, vm);

    map<int, void*> allocations;
    int next_id = 1;

    cout << "=== FINAL SYSTEM SIMULATOR ===" << endl;
    cout << "RAM: " << ram_size << "B | Page Size: " << page_size << "B" << endl;
    print_help();

    string line;
    while (true) {
        cout << "\n> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") break;
        else if (cmd == "help") print_help();

        else if (cmd == "init") {
            size_t new_size;
            if (ss >> new_size) {
                ram_size = new_size;
                
                delete sys;
                delete vm;
                delete buddy_mem;
                delete list_mem;
                allocations.clear();
                next_id = 1;

                list_mem = new MemoryManager(ram_size);
                list_mem->set_strategy(&ff);
                buddy_mem = new BuddyManager(ram_size);
                vm = new PageTable(page_size, ram_size, &vm_fifo);
                sys = new MemorySystem(list_mem, &cache_sys, vm);

                use_buddy = false;
                cout << "System initialized with " << ram_size << " bytes." << endl;
            } else {
                cout << "Usage: init <size>" << endl;
            }
        }
        
        else if (cmd == "malloc") {
            size_t size;
            if (ss >> size) {
                sys = new MemorySystem(list_mem, &cache_sys, vm); 
                delete sys; sys = new MemorySystem(list_mem, &cache_sys, vm);

                void* ptr = use_buddy ? buddy_mem->alloc(size) : list_mem->my_malloc(size);
                if (ptr) {
                    allocations[next_id++] = ptr;
                    cout << "Allocated ID " << next_id-1 << " @ " << ptr << endl;
                } else cout << "Allocation Failed." << endl;
            }
        }
        else if (cmd == "free") {
            int id;
            if (ss >> id && allocations.count(id)) {
                if (use_buddy) buddy_mem->free(allocations[id]);
                else list_mem->my_free(allocations[id]);
                allocations.erase(id);
                cout << "Freed ID " << id << endl;
            }
        }
        else if (cmd == "set") {
            string sub, mode; ss >> sub >> mode;
            if (sub == "allocator") {
                if (mode == "buddy") { 
                    use_buddy = true; 
                    cout << "Switched to Buddy System." << endl; 
                }
                else {
                    use_buddy = false;
                    if (mode == "first") { list_mem->set_strategy(&ff); cout << "Switched to First Fit." << endl; }
                    else if (mode == "best") { list_mem->set_strategy(&bf); cout << "Switched to Best Fit." << endl; }
                    else if (mode == "worst") { list_mem->set_strategy(&wf); cout << "Switched to Worst Fit." << endl; }
                    else cout << "Unknown mode. Use: buddy, first, best, worst." << endl;
                }
            }
        }
        else if (cmd == "dump") {
            if (use_buddy) buddy_mem->dump_memory();
            else list_mem->dump_memory();
        }

        else if (cmd == "access") {
            uint32_t addr;
            if (ss >> addr) sys->access(addr);
        }
        else if (cmd == "vm") {
            string sub, pol; ss >> sub >> pol;
            if (sub == "policy") {
                if (pol == "fifo") { vm->set_policy(&vm_fifo); cout << "VM Policy: FIFO" << endl; }
                else if (pol == "lru") { vm->set_policy(&vm_lru); cout << "VM Policy: LRU" << endl; }
            }
        }

        else if (cmd == "cache") {
            string sub, lvl, pol; ss >> sub >> lvl >> pol;
            if (sub == "policy") {
                ReplacementPolicy* p = (pol == "fifo") ? (ReplacementPolicy*)&cache_fifo : (ReplacementPolicy*)&cache_lru;
                if (lvl == "L1") l1.set_policy(p);
                else if (lvl == "L2") l2.set_policy(p);
                else if (lvl == "L3") l3.set_policy(p);
                cout << lvl << " policy set to " << pol << endl;
            }
        }

        else if (cmd == "stats") {
            string sub;
            if (ss >> sub) {
                if (sub == "allocator") {
                    cout << "\n--- ALLOCATOR STATISTICS ---" << endl;
                    if (use_buddy) buddy_mem->print_stats();
                    else list_mem->print_stats();
                } 
                else if (sub == "system") {
                    sys->print_all_stats();
                }
                else cout << "Usage: stats <allocator|system>" << endl;
            } else {
                cout << "Usage: stats <allocator|system>" << endl;
            }
        }
    }
    
    delete sys;
    delete vm;
    delete buddy_mem;
    delete list_mem;
    
    return 0;
}