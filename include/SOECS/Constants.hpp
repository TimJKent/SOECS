#pragma once

#include <cstdint>

namespace SOECS
{
    typedef int8_t ComponentTypeId;
    typedef int32_t EntityId;

    const ComponentTypeId INVALID_COMPONENT_TYPE_ID = -1;
    const ComponentTypeId MAX_COMPONENTS = 32;

    const EntityId INVALID_ENTITY_ID = -1;
    const EntityId MAX_ENTITIES = 512;
}
