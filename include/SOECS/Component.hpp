#pragma once

#include "Constants.hpp"
#include "Entity.hpp"

#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <array>

namespace SOECS
{

    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityId entity) = 0;
    };

    template <typename T>
    class ComponentArray : public IComponentArray
    {
    public:
        void InsertData(EntityId entity, T component)
        {
            if (entity_to_index_.find(entity) != entity_to_index_.end())
            {
                throw std::runtime_error("Component added to same entity more than once.");
            }

            size_t newIndex = size_;
            entity_to_index_[entity] = newIndex;
            index_to_entity_[newIndex] = entity;
            component_array_[newIndex] = component;
            ++size_;
        }

        void RemoveData(EntityId entity)
        {
            if (entity_to_index_.find(entity) == entity_to_index_.end())
            {
                throw std::runtime_error("Removing non-existent component.");
            }

            size_t indexOfRemovedEntity = entity_to_index_[entity];
            size_t indexOfLastElement = size_ - 1;
            component_array_[indexOfRemovedEntity] = component_array_[indexOfLastElement];

            EntityId entityOfLastElement = index_to_entity_[indexOfLastElement];
            entity_to_index_[entityOfLastElement] = indexOfRemovedEntity;
            index_to_entity_[indexOfRemovedEntity] = entityOfLastElement;

            entity_to_index_.erase(entity);
            index_to_entity_.erase(indexOfLastElement);

            --size_;
        }

        T &GetData(EntityId entity)
        {
            if (entity_to_index_.find(entity) == entity_to_index_.end())
            {
                throw std::runtime_error("Retrieving non-existent component.");
            }

            return component_array_[entity_to_index_[entity]];
        }

        void EntityDestroyed(EntityId entity) override
        {
            if (entity_to_index_.find(entity) != entity_to_index_.end())
            {
                RemoveData(entity);
            }
        }

    private:
        std::array<T, MAX_ENTITIES> component_array_;
        std::unordered_map<EntityId, size_t> entity_to_index_;
        std::unordered_map<size_t, EntityId> index_to_entity_;
        size_t size_ = 0;
    };

    class ComponentManager
    {
    public:
        template <typename T>
        void RegisterComponent()
        {
            const char *typeName = typeid(T).name();
            if (component_type_ids_.find(typeName) != component_type_ids_.end())
            {
                throw std::runtime_error("Component type already registered.");
            }

            component_type_ids_.insert({typeName, next_component_type_id_});
            component_arrays_.insert({typeName, std::make_shared<ComponentArray<T>>()});
            ++next_component_type_id_;
        }

        template <typename T>
        ComponentTypeId GetComponentTypeId()
        {
            const char *typeName = typeid(T).name();
            if (component_type_ids_.find(typeName) == component_type_ids_.end())
            {
                throw std::runtime_error("Component type not registered.");
            }
            return component_type_ids_[typeName];
        }

        template <typename T>
        void AddComponent(EntityId entity, T component)
        {
            GetComponentArray<T>()->InsertData(entity, component);
        }

        template <typename T>
        void RemoveComponent(EntityId entity)
        {
            GetComponentArray<T>()->RemoveData(entity);
        }

        template <typename T>
        T &GetComponent(EntityId entity)
        {
            return GetComponentArray<T>()->GetData(entity);
        }

        void EntityDestroyed(EntityId entity)
        {
            for (auto const &pair : component_arrays_)
            {
                auto const &component = pair.second;
                component->EntityDestroyed(entity);
            }
        }

    private:
        std::unordered_map<const char *, ComponentTypeId> component_type_ids_{};
        std::unordered_map<const char *, std::shared_ptr<IComponentArray>> component_arrays_{};
        ComponentTypeId next_component_type_id_{};

        template <typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray()
        {
            const char *typeName = typeid(T).name();

            if (component_type_ids_.find(typeName) == component_type_ids_.end())
            {
                throw std::runtime_error("Component not registered before use.");
            }

            return std::static_pointer_cast<ComponentArray<T>>(component_arrays_[typeName]);
        }
    };
}
