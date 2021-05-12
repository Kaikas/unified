#pragma once

#include "nwnx.hpp"
#include <cstdint>

namespace Events {

class NPCEvents
{
public:
    NPCEvents();

private:
    static int32_t HandleNPCMessageHook(CNWSMessage*, CNWSCreature*, uint8_t, int32_t);
};

}
