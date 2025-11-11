#pragma once

#include "entity.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class EntityRegistry {

public:
    // List of all entities
    std::vector<std::unique_ptr<Entity>> entities;
    // Lookup map for named access
    std::unordered_map<std::string, Entity*> registry;

    EntityRegistry() = default;
    ~EntityRegistry() = default;

    template <typename T, typename... Args>
    T* spawn(const std::string& name, Args&&... args)
    {
        auto e = std::make_unique<T>(name, std::forward<Args>(args)...);
        T* ptr = e.get();
        registry[name] = ptr;
        entities.push_back(std::move(e));
        return ptr;
    }

    Entity* get(const std::string& name)
    {
        auto it = registry.find(name);

        if (it != registry.end()) {
            return it->second;
        } else {
            // std::cerr << "[EntityRegistry] ERROR: Entity '" << name << "' not found!\n";
            TraceLog(LOG_ERROR, "[EntityRegistry] Entity '%s' not found!", name.c_str());
            assert(false && "Entity not found in registry!");
            return nullptr;
        }
    }

    std::vector<std::unique_ptr<Entity>>& get_all() { return entities; }

    void purge_dead()
    {
        entities.erase(std::remove_if(entities.begin(), entities.end(),
                           [&](const std::unique_ptr<Entity>& e) {
                               return !e->is_alive;
                           }),
            entities.end());
    }

    // Entity* skull = entity_registry.get("skull");
    // chest = entity_registry.spawn<Entity>("chest", 10, 5, eZone::WORLD);
};
