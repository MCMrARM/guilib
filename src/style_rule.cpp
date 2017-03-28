#include <guilib/style_rule.h>

using namespace guilib;

StyleRule::Description* StyleRule::currentDescription;

void StyleRule::Description::releaseRef() {
    if (refs <= 1) {
        delete this;
        return;
    }
    --refs;
}

void StyleRule::updateDescription(Description* description) {
    Description* oldDescription = this->description;
    void* oldData = this->data;
    this->description = description;

    description->acquireRef();
    data = malloc(description->totalSize);
    for (size_t i = 0; i < description->entries.size(); i++) {
        auto& ent = description->entries[i];
        if (i < oldDescription->entries.size())
            ent.moveFunc((void*) ((char*) data + ent.offset), (void*) ((char*) oldData + ent.offset));
        else
            ent.constructFunc((void*) ((char*) data + ent.offset));
    }

    if (oldData != nullptr)
        destroyData(oldDescription, oldData);
}

void StyleRule::destroyData(Description* description, void* data) {
    for (auto const& ent : description->entries)
        ent.destructFunc((void*) ((char*) data + ent.offset));
    free(data);
    description->releaseRef();
}

StyleRule::StyleRule() {
    if (currentDescription->totalSize > 0) {
        description = currentDescription;
        description->acquireRef();
        data = malloc(description->totalSize);
        for (auto const& ent : description->entries)
            ent.constructFunc((void*) ((char*) data + ent.offset));
    }
}

StyleRule::~StyleRule() {
    if (data != nullptr)
        destroyData(description, data);
}

StyleRule::Description* StyleRule::createNewDescription() {
    Description* newDescription = new Description();
    newDescription->acquireRef();
    if (currentDescription != nullptr) {
        newDescription->entries = currentDescription->entries;
        newDescription->totalSize = currentDescription->totalSize;

        currentDescription->releaseRef(); // this will destroy the object if needed
    }
    currentDescription = newDescription;
    return newDescription;
}

StyleRule::Description* StyleRule::getDescription() {
    if (currentDescription == nullptr)
        return createNewDescription();
    return currentDescription;
}