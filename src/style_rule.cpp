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
    if (this->description != nullptr)
        destroyDescription();
    this->description = description;
    description->acquireRef();
    data = malloc(description->totalSize);
    for (auto const& ent : description->entries)
        ent.constructFunc((void*) ((char*) data + ent.offset));
}

void StyleRule::destroyDescription() {
    for (auto const& ent : description->entries)
        ent.destructFunc((void*) ((char*) data + ent.offset));
    description->releaseRef();
    description = nullptr;
}

StyleRule::StyleRule() {
    updateDescription(currentDescription);
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