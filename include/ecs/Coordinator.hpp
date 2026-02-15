#pragma once

#include "Entity.hpp"
#include "Component.hpp"
#include "System.hpp"
#include "Signature.hpp"

namespace SOECS
{
    class Coordinator
    {
    public:
        void Init()
        {
            // Create pointers to each manager
            component_manager_ = std::make_unique<ComponentManager>();
            entity_manager_ = std::make_unique<EntityManager>();
            system_manager_ = std::make_unique<SystemManager>();
        }

        // Entity methods
        EntityId CreateEntity()
        {
            return entity_manager_->CreateEntity();
        }

        void DestroyEntity(EntityId entity)
        {
            entity_manager_->DestroyEntity(entity);

            component_manager_->EntityDestroyed(entity);

            system_manager_->EntityDestroyed(entity);
        }

        // Component methods
        template <typename T>
        void RegisterComponent()
        {
            component_manager_->RegisterComponent<T>();
        }

        template <typename T>
        void AddComponent(EntityId entity, T component)
        {
            component_manager_->AddComponent<T>(entity, component);

            auto signature = entity_manager_->GetSignature(entity);
            signature.set(component_manager_->GetComponentType<T>(), true);
            entity_manager_->SetSignature(entity, signature);

            system_manager_->EntitySignatureChanged(entity, signature);
        }

        template <typename T>
        void RemoveComponent(EntityId entity)
        {
            component_manager_->RemoveComponent<T>(entity);

            auto signature = entity_manager_->GetSignature(entity);
            signature.set(component_manager_->GetComponentType<T>(), false);
            entity_manager_->SetSignature(entity, signature);

            system_manager_->EntitySignatureChanged(entity, signature);
        }

        template <typename T>
        T &GetComponent(EntityId entity)
        {
            return component_manager_->GetComponent<T>(entity);
        }

        template <typename T>
        ComponentTypeId GetComponentType()
        {
            return component_manager_->GetComponentType<T>();
        }

        // System methods
        template <typename T>
        std::shared_ptr<T> RegisterSystem()
        {
            return system_manager_->RegisterSystem<T>();
        }

        template <typename T>
        void SetSystemSignature(Signature signature)
        {
            system_manager_->SetSignature<T>(signature);
        }

    private:
        std::unique_ptr<ComponentManager> component_manager_;
        std::unique_ptr<EntityManager> entity_manager_;
        std::unique_ptr<SystemManager> system_manager_;
    };
}