-- id <-> entity
-- cpp_object : Scene wrapper
require("luaclass")
require("Entity")
require("Render/RenderDefines")
require("Render/RenderGraph")
require("Render/ShaderDesc")


Scene = class("Scene")

function Scene:init(path)
    -- SceneWrapper constructor
    self.cpp_object = SPW.SceneWrapper(path)
    self.ent_map = {}
end

function Scene:addEntity(name)
    -- create a new entity in cpp side
    -- then get the entity cpp wrapper obj
    -- SceneWrapper.createEntity
    local cpp_entity = self.cpp_object:createEntity(name)
    -- cpp_entity : SPW:EntityWrapper
    local en = Entity.new(name, cpp_entity)
    self.ent_map[en.id] = en
    return en
end

function Scene:initScene()
    -- SceneWrapper.initScene
    self.cpp_object:initial()
end

function Scene:beforeUpdate()
    -- SceneWrapper.beforeUpdate
    self.cpp_object:beforeUpdate()
end

function Scene:onUpdate(duration)
    -- SceneWrapper.onUpdate
    self.cpp_object:onUpdate(duration)
end

function Scene:afterUpdate()
    -- SceneWrapper.afterUpdate
    self.cpp_object:afterUpdate()
end

function Scene:onStop()
    -- SceneWrapper.onStop
    self.cpp_object:onStop()
end

function Scene:getEntityFromID(id)
    if self.ent_map[id] ~= nil then
        return self.ent_map[id]
    end
    -- try to get the entity from cpp_object
    local cpp_entity = self.cpp_object:getEntityByID(id)
    -- check if the entity is valid
    if cpp_entity ~= nil and cpp_entity:isValid() then
        local en = Entity.new(cpp_entity:getName(), cpp_entity)
        self.ent_map[en.id] = en
        return en
    end
    -- return nil if not found
    return nil
end

function Scene:removeEntity(en)
    -- en : @Entity
    -- SceneWrapper.removeEntity() by id
    self.cpp_object:remove(en.id)
    table.remove(self.ent_map, en.id)
end

function Scene:forEach(component_type, callback)
    for _, ent in pairs(self.ent_map) do
        if ent.hasComponent(component_type) then
            callback(ent)
        end
    end
end

function Scene:createRenderGraph(name)
    local cpp_graph = self.cpp_object:createRenderGraph(name)
    local graph = RenderGraph.new(cpp_graph, name)
    return graph
end

function Scene:registerShader(shader)
    self.cpp_object:registerShader(shader.cpp_object)
end

function Scene:getGraphID(graph_name)
    return self.cpp_object:getGraphID(graph_name)
end

function Scene:getRenderNodeID(node_name)
    return self.cpp_object:getRenderNodeID(node_name)
end

function Scene:getRenderPort(port_name)
    return self.cpp_object:getRenderPort(port_name)
end

function Scene:getShaderID(shader_name)
    return self.cpp_object:getShaderID(shader_name)
end

function Scene:setSkyBox(cam_id, right, left, top, bottom, front, back)
    self.cpp_object:setSkyBox(cam_id, right, left, top, bottom, front, back)
end