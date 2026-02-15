#pragma once

#include "Entity.hpp"
#include "Signature.hpp"
#include <unordered_map>
#include <set>

namespace SOECS
{
    class System
    {
    public:
        std::set<EntityId> entities;
    };

    class SystemManager
    {
    public:
        template <typename T>
        std::shared_ptr<T> RegisterSystem()
        {
            const char *typeName = typeid(T).name();

            if (systems_.find(typeName) != systems_.end())
            {
                throw std::runtime_error("System already registered.");
            }

            auto system = std::make_shared<T>();
            systems_.insert({typeName, system});
            return system;
        }

        template <typename T>
        void SetSignature(Signature signature)
        {
            const char *typeName = typeid(T).name();

            if (systems_.find(typeName) == systems_.end())
            {
                throw std::runtime_error("System not registered.");
            }

            signatures_.insert({typeName, signature});
        }

        void EntityDestroyed(EntityId entity)
        {
            for (auto const &pair : systems_)
            {
                auto const &system = pair.second;
                system->entities.erase(entity);
            }
        }

        void EntitySignatureChanged(EntityId entity, Signature entitySignature)
        {
            // Notify each system that an entity's signature changed
            for (auto const &pair : systems_)
            {
                auto const &type = pair.first;
                auto const &system = pair.second;
                auto const &systemSignature = signatures_[type];

                // Entity signature matches system signature - insert into set
                if ((entitySignature & systemSignature) == systemSignature)
                {
                    system->entities.insert(entity);
                }
                // Entity signature does not match system signature - erase from set
                else
                {
                    system->entities.erase(entity);
                }
            }
        }

    private:
        std::unordered_map<const char *, Signature> signatures_;
        std::unordered_map<const char *, std::shared_ptr<System>> systems_;
    };
}
