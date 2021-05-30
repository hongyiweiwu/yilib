#include "any.hpp"
#include "typeinfo.hpp"
#include "utility.hpp"

namespace std {
    const char* bad_any_cast::what() const noexcept { return "std::bad_any_cast"; }

    any::any(const any& other) : state(other.state), typeinfo(other.typeinfo), destroyer(other.destroyer) {
        switch (other.state) {
            case __State::HEAP:
                new (&storage) __heap_obj_storage(other.storage.heap_obj);
                break;
            
            case __State::STACK:
                new (&storage) __stack_obj_storage(other.storage.stack_obj);
                break;

            default:
                break;
        }
    }

    any::any(any&& other) noexcept : state(other.state), typeinfo(other.typeinfo), destroyer(other.destroyer) {
        switch (other.state) {
            case __State::HEAP:
                new (&storage) __heap_obj_storage(move(other.storage.heap_obj));
                break;
            
            case __State::STACK:
                new (&storage) __stack_obj_storage(move(other.storage.stack_obj));
                break;

            default:
                break;
        }
    }

    any::~any() { reset(); }

    any& any::operator=(const any& rhs) { 
        any(rhs).swap(*this);
        return *this;
    }

    any& any::operator=(any&& rhs) noexcept {
        any(move(rhs)).swap(*this);
        return *this;
    }

    void any::reset() noexcept {
        switch (state) {
            case __State::HEAP:
                destroyer(*this);
                storage.heap_obj.~__heap_obj_storage();
                goto shared_destroy;
            
            case __State::STACK:
                destroyer(*this);
                storage.stack_obj.~__stack_obj_storage();

shared_destroy:
                state = __State::NONE;
                storage.no_obj = nullptr;
                typeinfo = nullptr;
                destroyer = nullptr;

            case __State::NONE:
                return;
        }
    }

    void any::swap(any& rhs) noexcept {
        switch (state) {
            case __State::NONE: 
                switch (rhs.state) {
                    case __State::NONE: break;
                    case __State::HEAP:
                        new (&storage.heap_obj) __heap_obj_storage(move(rhs.storage.heap_obj));
                        rhs.storage.no_obj = nullptr;
                        break;

                    case __State::STACK:
                        new (&storage.stack_obj) __stack_obj_storage(move(rhs.storage.stack_obj));
                        rhs.storage.no_obj = nullptr;
                        break;
                }
                break;

            case __State::HEAP:
                switch (rhs.state) {
                    case __State::NONE:
                        new (&rhs.storage.heap_obj) __heap_obj_storage(move(storage.heap_obj));
                        storage.no_obj = nullptr;
                        break;

                    case __State::HEAP: {
                        auto temp = move(rhs.storage.heap_obj);
                        new (&rhs.storage.heap_obj) __heap_obj_storage(move(storage.heap_obj));
                        new (&storage.heap_obj) __heap_obj_storage(move(temp));
                        break;
                    }

                    case __State::STACK: {
                        auto temp = move(rhs.storage.stack_obj);
                        new (&rhs.storage.heap_obj) __heap_obj_storage(move(storage.heap_obj));
                        new (&storage.stack_obj) __stack_obj_storage(move(temp));
                        break;
                    }
                }
                break;
            
            case __State::STACK: 
                switch (rhs.state) {
                    case __State::NONE:
                        new (&rhs.storage.stack_obj) __stack_obj_storage(move(storage.stack_obj));
                        storage.no_obj = nullptr;
                        break;

                    case __State::HEAP: {
                        auto temp = move(rhs.storage.heap_obj);
                        new (&rhs.storage.stack_obj) __stack_obj_storage(move(storage.stack_obj));
                        new (&storage.heap_obj) __heap_obj_storage(move(temp));
                        break;
                    }

                    case __State::STACK:
                        auto temp = move(rhs.storage.stack_obj);
                        new (&rhs.storage.stack_obj) __stack_obj_storage(move(storage.stack_obj));
                        new (&storage.stack_obj) __stack_obj_storage(move(temp));
                        break;
                }
                break;
        }

        std::swap(state, rhs.state);
        std::swap(typeinfo, rhs.typeinfo);
        std::swap(destroyer, rhs.destroyer);
    }

    bool any::has_value() const noexcept { return state != __State::NONE; }
    const type_info& any::type() const noexcept { return has_value() ? *typeinfo : typeid(void); }

    void swap(any& x, any& y) noexcept { x.swap(y); }
}