#pragma once

#include "render/CharacterData.h"
#include "player/PlayerCharacter.h"
#include "render/colors.h"

class PlayerCharAdapter : public CharacterData {
public:
    PlayerCharAdapter(std::shared_ptr<PlayerCharacter> pc);

    std::string getID() const override;
    std::string getEmotionalTag() const override;
    std::string getFaction() const override;
    Vec3 getPoseHint() const override;
    void setColor(const Vec3& color);

private:
    std::shared_ptr<PlayerCharacter> character;
    Vec3 color;
};
