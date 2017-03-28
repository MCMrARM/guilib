#pragma once

#include <string>
#include <vector>
#include <functional>

namespace guilib {

class StyleRule {

private:

    struct Description {
        size_t refs = 0;

        struct Entry {
            size_t size;
            size_t offset;
            std::function<void (void*)> constructFunc;
            std::function<void (void*, void*)> moveFunc;
            std::function<void (void*)> destructFunc;
        };

        std::vector<Entry> entries;
        size_t totalSize = 0;

        void acquireRef() { refs++; }
        void releaseRef();
    };
    Description* description = nullptr;
    void* data = nullptr;

    void updateDescription(Description* description);

    static void destroyData(Description* description, void* data);

    static Description* currentDescription;

    /**
     * Creates a new global Description that will be used for all future StyleRule instances. The instances that were
     * already created are not affected.
     * @return a pointer to the current description
     */
    static Description* createNewDescription();

    /**
     * Returns the current global Description instance. If no instance exists when this function is called, an empty one
     * will be created.
     * @return a pointer to a valid description instance
     */
    static Description* getDescription();

public:

    StyleRule();

    ~StyleRule();

    /**
     * This is a internal class used to automagically register style declarations.
     * @tparam T
     */
    template <typename T>
    struct RegistrationHandle {

        size_t id;

        RegistrationHandle() {
            Description* desc = createNewDescription();
            id = desc->entries.size();
            desc->entries.push_back({ sizeof(T), desc->totalSize, [](void* ptr) {
                new ((T*) ptr)T();
            }, [](void* newPtr, void* oldPtr) {
                new ((T*) newPtr)T((T&&) (*(T*) oldPtr));
                ((T*) oldPtr)->~T();
            }, [](void* ptr) {
                ((T*) ptr)->~T();
            } });
            desc->totalSize += sizeof(T);
        }
        // TODO: Handle destruction propertly?

    };

    template <typename T>
    T& as() {
        if (T::Registration.id > description->entries.size()) {
            if (T::Registration.id > currentDescription->entries.size()) {
                throw std::runtime_error("The specified style rule type is recognized.");
            } else {
                updateDescription(currentDescription);
            }
        }
        return *((T*) ((unsigned char*) data + description->entries[T::Registration.id].offset));
    }

};

}