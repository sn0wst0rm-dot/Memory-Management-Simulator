#include "first_fit.h"

BlockHeader* FirstFit::find_block(BlockHeader* head, size_t size) {
    BlockHeader* current = head;
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}