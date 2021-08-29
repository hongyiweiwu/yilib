// Utilities to faciliate executing functions at thread exits. Used in "condition_variable.hpp" and "future.hpp"
#pragma once

#include "pthread.h"

#include "cstdlib.hpp"

namespace std::__internal {
    struct at_thread_exit_fn_block {
        void (*callback)(void*);
        at_thread_exit_fn_block* next;

        static constinit pthread_key_t key;

        /* Adds a new at_thread_exit callback to the front of the list. The provided block should be allocated on the heap, should not automatically free itself, and should delete itself in its `callback`
         * function. The `next` field will be populated by this function. */
        static inline void push_front(at_thread_exit_fn_block* const new_block) noexcept {
            void* const curr_head = pthread_getspecific(key);
            new_block->next = static_cast<at_thread_exit_fn_block*>(curr_head);
            pthread_setspecific(key, new_block);
        }

        /* The function that will be invoked when the thread exits. This function calls every callback in the callback list, and then deletes the callback list thread key. */
        static inline void teardown(void* p) noexcept {
            at_thread_exit_fn_block* curr = static_cast<at_thread_exit_fn_block*>(p);
            while (curr != nullptr) {
                at_thread_exit_fn_block* const next = curr->next;
                curr->callback(curr);
                curr = next;
            }

            pthread_key_delete(key);
        }

        /* Initializes the callback list key, and registers the corresponding destructor function through both `pthread_key_create` (used in most cases) and `std::atexit` (used when exiting the main thread, when `pthread_key_create`
         * isn't invoked). */
        static inline void initialize_key() noexcept {
            static constinit pthread_once_t once_control = PTHREAD_ONCE_INIT;
            pthread_once(&once_control, [] {
                pthread_key_create(&key, teardown);
                std::atexit([] {
                    void* const p = pthread_getspecific(key);
                    pthread_setspecific(key, nullptr);
                    if (p != nullptr) {
                        teardown(p);
                    }
                });
            });
        }
    };
}