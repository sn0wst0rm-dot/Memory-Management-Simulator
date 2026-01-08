#include "worst_fit.h"

BlockHeader* WorstFit::find_block(BlockHeader* head, size_t size) {
    BlockHeader* best_block = nullptr;
    BlockHeader* current = head;

    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            if (best_block == nullptr || current->size > best_block->size) {
                best_block = current;
            }
        }
        current = current->next;
    }
    return best_block;
}