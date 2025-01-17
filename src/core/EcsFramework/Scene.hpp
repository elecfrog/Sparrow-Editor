//
// Created by ZhangHao on 2023/2/13.
//
#pragma once

#include "Event/Event.h"
#include "entt.hpp"
#include "Utils/Timer.hpp"

#include "EcsFramework/Entity/Entity.hpp"
#include "System/SystemI.h"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "Component/ComponentI.h"
#include "Component/BasicComponent/IDComponent.h"
#include "Component/BasicComponent/NameComponent.h"
#include "Utils/UUID.hpp"
#include "Utils/MacroUtils.h"
#include "UI/UIResponder.hpp"
#include <vector>

namespace SPW {
#define forEach(func, ...) _forEach<__VA_ARGS__>(static_cast<SPW::Scene::ForeachFunc<__VA_ARGS__>>(func))
    // contains all entities, .eg Game world ...
    class Scene : public EventResponderI {
    public:
        explicit Scene(const std::shared_ptr<EventResponderI> &parent)
            : EventResponderI(parent) {
            registry = std::make_shared<entt::registry>();
            ui_responder = std::make_shared<UIResponder>(nullptr);
        }
        // create scene with this line
        static std::shared_ptr<Scene> create(const std::shared_ptr<EventResponderI> &parent) {
            auto scene = std::make_shared<Scene>(parent);
            return scene;
        }

        void addSystem(std::shared_ptr<SystemI> &&system) {

            systems.push_back(system);
        }

        void PauseSystem(std::shared_ptr<SystemI> &&system){
            for(auto s : systems){
                if(typeid(s) == typeid(system)) s->setPausd(true);
            }
        }
        // create new entity in scene
        std::shared_ptr<Entity> createEntity(const std::string &name,
                                              const UUID &uid)  {
            auto ent = std::make_shared<SPW::Entity>(registry);
            ent->emplace<SPW::IDComponent>(uid);
            ent->emplace<SPW::NameComponent>(name);
            all_entities[uid.toString()] = ent;
            return ent;
        }

        std::shared_ptr<Entity> createEntity(const std::string &name) {
            return createEntity(name, UUID::randomUUID());
        }

        // delete entity
        void deleteEntity(const std::shared_ptr<Entity> &entity) {
            all_entities.erase(entity->getUUID().toString());
            registry->destroy(entity->entity);
        }

        void deleteEntity(const Entity* entity) {
            all_entities.erase(entity->getUUID().toString());
            registry->destroy(entity->entity);
        }

        // get entity by id
        std::shared_ptr<Entity> getEntityByID(const std::string &uuid) {
            if (all_entities.find(uuid) != all_entities.end()) {
                return all_entities[uuid];
            }
            return nullptr;
        }

        // remove entity by id
        void removeEntityByID(const std::string &uuid) {
            if (all_entities.find(uuid) != all_entities.end()) {
                deleteEntity(all_entities[uuid]);
            }
        }
        void clearAllEntity()
        {
            for (const auto& entity : all_entities)
            {
                if(entity.second->has<CameraComponent>() && entity.second->getUUID() == ResourceManager::getInstance()->activeCameraID)
					continue;
            	deleteEntity(entity.second);

            }
        }
        // for each
        // iterate components of every entity that has required components
        //
        // usage:
        // aScene.forEach([](Type1 &, Type2 &){
        //      // your code
        // }, Type1, Type2);
        template<Component ...C>
        using ForeachFunc = std::function<void(C* ...c)>;
        template<Component ...C>
        void _forEach(ForeachFunc<C...> &&func) {
            using TupleIndex = typename X_Build_index_tuple<std::tuple_size<std::tuple<C *...>>::value>::type;
            for (auto &view : getEntitiesWith<C...>()) {
                Entity e = {view, registry};
                auto res = e.combined<C...>();
                InvokeTupleFunc(res, func, TupleIndex());
            }
        }

        // for loop with a callback.
        // Components ...C: Required components in an entity
        // func: call back Entities that have required components
        template<Component ...C>
        void forEachEntity(const std::function<void(const Entity &)> &func) {
            auto views = getEntitiesWith<C...>();
            for (auto &view : views) {
                func({view, registry});
            }
        }

        // for loop with a callback.
        // ComponentGroup<C...>, required components group
        // func: call back Entities that have required components
        template<Component ...C>
        void forEachEntityInGroup(ComponentGroup<C...>, const std::function<void(const Entity &)> &func) {
            forEachEntity<C...>(func);
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

        // for UI
        unsigned int getUIRenderGraphID() {
            return m_renderSystem.lock()->uiGraph->graph_id;
        }

        unsigned int getUIRenderNodeID() {
            return m_renderSystem.lock()->uiNode->pass_id;
        }

        UUID getUIProgramID() {
            return m_renderSystem.lock()->UIProgram.uuid;
        }

        void onEvent(const std::shared_ptr<EventI> &e) override{
            if (isUIMode) {
                ui_responder->onEvent(e);
            } else {
                EventResponderI::onEvent(e);
            }
        }

        void solveEvent(const std::shared_ptr<EventI> &e) override {
            e->dispatch<WindowResizeType, WindowEvent>([this](WindowEvent *e){
                ui_responder->width = e->width;
                ui_responder->height = e->height;
                return false;
            });
        }

        std::shared_ptr<UIResponder> getUIResponder(){
            return ui_responder;
        }

        std::shared_ptr<UIResponder> initUIResponder(int width, int height){
            ui_responder->width = width;
            ui_responder->height = height;
            ui_responder->pos_x = 0;
            ui_responder->pos_y = 0;

            return ui_responder;
        }

        std::weak_ptr<SPWRenderSystem> m_renderSystem;

        std::shared_ptr<Entity> uiCamera;

        std::shared_ptr<UIResponder> ui_responder;
        bool isUIMode = false;
        std::shared_ptr<entt::registry> registry;
    private:
        // get entity with components
        template<Component ...C>
        auto getEntitiesWith() {
            auto res = registry->view<C...>();
            return res;
        }

        std::unordered_map<std::string, std::shared_ptr<Entity>> all_entities;


        std::vector<std::shared_ptr<SystemI>> systems;
        friend Entity;

    };
}

