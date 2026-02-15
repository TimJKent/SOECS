#pragma once

#include "Entity.hpp"
#include "Signature.hpp"

#include <queue>
#include <stdexcept>
#include <array>
#include <cstdint>

namespace SOECS
{
    typedef int32_t EntityId;

    const EntityId INVALID_ENTITY_ID = -1;
    const EntityId MAX_ENTITIES = 512;

    class EntityManager
    {
    public:
        EntityManager()
        {
            for (EntityId id = 0; id < MAX_ENTITIES; ++id)
            {
                available_entity_ids_.push(id);
            }
        }

        EntityId CreateEntity()
        {
            if (available_entity_ids_.empty())
            {
                throw std::runtime_error("Maximum number of entities reached");
            }

            EntityId new_id = available_entity_ids_.front();
            available_entity_ids_.pop();
            return new_id;
        }

        void DestroyEntity(EntityId entity_id)
        {
            if (entity_id > MAX_ENTITIES)
            {
                throw std::invalid_argument("Invalid EntityId");
            }

            entity_signatures_[entity_id].reset();
            available_entity_ids_.push(entity_id);
        }

        void SetSignature(EntityId entity_id, Signature signature)
        {
            if (entity_id > MAX_ENTITIES)
            {
                throw std::invalid_argument("Invalid EntityId");
            }

            entity_signatures_[entity_id] = signature;
        }

        Signature GetSignature(EntityId entity_id) const
        {
            if (entity_id > MAX_ENTITIES)
            {
                throw std::invalid_argument("Invalid EntityId");
            }

            return entity_signatures_[entity_id];
        }

        std::queue<EntityId> available_entity_ids_{};
        std::array<Signature, MAX_ENTITIES> entity_signatures_{};
    };
}
