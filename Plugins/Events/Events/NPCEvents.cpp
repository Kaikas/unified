#include "Events/NPCEvents.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CGameObjectArray.hpp"
#include "API/CNetLayer.hpp"
#include "API/CNetLayerPlayerInfo.hpp"
#include "API/Functions.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSMessage.hpp"
#include "API/Constants.hpp"
#include "API/Globals.hpp"
#include "Events.hpp"
#include <vector>


namespace Events {

using namespace NWNXLib;
using namespace NWNXLib::API;
using namespace NWNXLib::API::Constants;
using namespace NWNXLib::Platform;

static Hooks::Hook s_HandleNPCMessageHook;

NPCEvents::NPCEvents()
{
    Events::InitOnFirstSubscribe("NWNX_ON_NPC_.*", []() {
        s_HandleNPCMessageHook = Hooks::HookFunction(
                Functions::_ZN15CFactionManager23GetNPCFactionReputationEii,
                (void*)&HandleNPCMessageHook, Hooks::Order::Early);
    });
}

int32_t NPCEvents::HandleNPCMessageHook(CNWSMessage *thisPtr, CNWSCreature *cNPC, uint8_t nMinor, int32_t bGroup)
{
    int32_t retVal;
    std::string event = "NWNX_ON_NPC_";
    ObjectID oidNPC = cNPC ? cNPC->m_oidLastPerception: OBJECT_INVALID;

    auto DefaultSignalEvent = [&]() -> void {
        if (Events::SignalEvent(event + "_BEFORE", oidNPC))
        {
            retVal = s_HandleNPCMessageHook->CallOriginal<int32_t>(thisPtr, cNPC, nMinor, bGroup);
        }
        else
        {
            retVal  = false;
        }
        Events::SignalEvent(event + "_AFTER", oidNPC);
    };

    switch (nMinor)
    {
        case NPCMinor::ScratchAss:
        {
            event += "SCRATCH_ASS";
            int32_t offset = 0;

            std::string area = Utils::ObjectIDToString(Utils::PeekMessage<ObjectID>(thisPtr, 0) & 0x7FFFFFFF); offset += sizeof(ObjectID);
            std::string object = Utils::ObjectIDToString(Globals::AppManager()->m_pServerExoApp->GetObjectArray()->m_nNextObjectArrayID[0]);
            int32_t objectType;

            switch (nMinor)
            {
                case NPCMinor::ScratchAss:
                    objectType = ObjectType::Creature;
                    break;
                default:
                    break;
            }

            auto resref = Utils::PeekMessage<CResRef>(thisPtr, offset);

            auto PushAndSignal = [&](const std::string& ev) -> bool {
                Events::PushEventData("OBJECT", object);
                Events::PushEventData("OBJECT_TYPE", std::to_string(objectType));
                Events::PushEventData("RESREF", resref.GetResRefStr());
                return Events::SignalEvent(ev, oidNPC);
            };

            if (PushAndSignal(event + "_BEFORE"))
            {
                retVal = s_HandleNPCMessageHook->CallOriginal<int32_t>(thisPtr, cNPC, nMinor, bGroup);
            }
            else
            {
                retVal = false;
            }

            PushAndSignal(event +"_AFTER");
            break;
        }
        default:
            retVal = s_HandleNPCMessageHook->CallOriginal<int32_t>(thisPtr, cNPC, nMinor, bGroup);
            break;
    }

    return retVal;
}

}
