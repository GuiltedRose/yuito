#pragma once
#include <string>
#include "math/Vec3.h"

class CharacterData {
public:
    virtual ~CharacterData() = default;

    virtual std::string getID() const = 0;
    virtual std::string getEmotionalTag() const = 0;
    virtual std::string getFaction() const = 0;

    virtual Vec3 getPoseHint() const = 0; // Optional for idle variation
};
