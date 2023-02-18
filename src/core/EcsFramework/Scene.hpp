//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.h"
#include "entt.hpp"
#include "Utils/Timer.hpp"

#include "EcsFramework/Entity/Entity.hpp"
#include "System/SystemI.h"
#include "Component/ComponentI.h"
#include "Component/BasicComponent/IDComponent.h"
#include "Component/BasicComponent/NameComponent.h"
#include "Utils/UUID.hpp"

#include "Utils/MacroUtils.h"

#include <vector>

namespace SPW {
#define forEach(func, ...) _forEach<__VA_ARGS__>(static_cast<SPW::Scene::ForeachFunc<__VA_ARGS__>>(func))
    // contains all entities, .eg Game world ...
    class Scene : public EventResponderI {
    public:
        explicit Scene(const std::shared_ptr<EventResponderI> &parent)
            : EventResponderI(parent) {
            registry = std::make_shared<entt::registry>();
        }
        // create scene with this line
        static std::shared_ptr<Scene> create(const std::shared_ptr<EventResponderI> &parent) {
            auto scene = std::make_shared<Scene>(parent);
            // TODO
            //scene->systems.emplace_back(system);
            //scene->systems.emplace_back(system);
            return scene;
        }

        // create new entity in scene
        std::shared_ptr<Entity> createEntity(const std::string &name,
                                              const UUID &uid)  {
            auto ent = std::make_shared<SPW::Entity>(registry);
            ent->emplace<SPW::IDComponent>(uid);
            ent->emplace<SPW::NameComponent>(name);
            return ent;
        }

        std::shared_ptr<Entity> createEntity(const std::string &name) {
            return createEntity(name, UUID::randomUUID());
        }

        // delete entity
        void deleteEntity(const std::shared_ptr<Entity> &entity) {
            registry->destroy(entity->entity);
        }

        // for each
        // iterate all entities with selected component
        //
        // usage:
        // aScene.forEach([](Type1 &, Type2 &){
        //      // your code
        // }, Type1, Type2);
        template<Component ...C>
        using ForeachFunc = std::function<void(const C& ...c)>;
        template<Component ...C>
        void _forEach(const ForeachFunc<C...> &func) {
            using TupleIndex = typename X_Build_index_tuple<std::tuple_size<std::tuple<C...>>::value>::type;
            for (auto &view : getEntitiesWith<C...>()) {
                Entity e = {view, registry};
                auto res = e.combined<C...>();
                InvokeTupleFunc_Address(res, func, TupleIndex());
            }
        }

        template<Component ...C>
        void forEachEntity(const std::function<void(const Entity &)> &func) {
            for (auto &view : getEntitiesWith<C...>()) {
                func({view, registry});
            }
        }

        virtual void initial() {
            for (auto &system : systems) {
                system->initial();
            }
        }
        virtual void beforeUpdate() {
            for (auto &system : systems) {
                system->beforeUpdate();
            }
        };
        virtual void onUpdate(TimeDuration dt) {
            for (auto &system : systems) {
                system->onUpdate(dt);
            }
        };
        virtual void afterUpdate() {
            for (auto &system : systems) {
                system->afterUpdate();
            }
        };
        virtual void onStop() {
            for (auto &system : systems) {
                system->onStop();
            }
        };

        // Event
        const char *getName() override {
            return "SPWDefaultScene";
        }

    private:
        // get entity with components
        template<Component ...C>
        auto getEntitiesWith() {
            auto res = registry->view<C...>();
            return res;
        }

        std::shared_ptr<entt::registry> registry;
        std::vector<std::shared_ptr<SystemI>> systems;
        friend Entity;
    };

}
