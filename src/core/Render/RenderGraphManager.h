//
// Created by NANAnoo on 5/9/2023.
//
#pragma once

#include "Utils/SingletonBase.h"
#include "RenderGraph.hpp"
#include "RenderBackEndI.h"
// #include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"

namespace SPW {
    // default render graph config in sparrow engine
    const unsigned int UNKNOWN_ID = (unsigned int)(-1);

    using RenderGraphKey = std::string;
    using RenderNodeKey = std::string;
    using RenderNodeOutputKey = std::string;
    using ShaderDescKey = std::string;

    const RenderGraphKey kDefferShadingGraph = "kDefferShadingGraph";
    const RenderGraphKey kSkyBoxRenderGraph = "kSkyBoxRenderGraph";
    const RenderGraphKey kUIRenderGraph = "kUIRenderGraph";

    const RenderNodeKey kPointShadowNode = "kPointShadowNode";
    const RenderNodeKey kDirectionalShadowNode = "kDirectionalShadowNode";
    const RenderNodeKey kGBufferNode = "kGBufferNode";
    const RenderNodeKey kSkyboxNode = "kSkyboxNode";

    // render outputs in deffer shading
    const RenderNodeOutputKey kPointShadowPort = "kPointShadowPort";
    const RenderNodeOutputKey kDirectionalShadowPort = "kDirectionalShadowPort";
    const RenderNodeOutputKey kGPositionPort = "kGPositionPort";
    const RenderNodeOutputKey kGAlbedoPort = "kGAlbedoPort";
    const RenderNodeOutputKey kGNormalPort = "kGNormalPort";
    const RenderNodeOutputKey kGMetalRoughnessPort = "kGMetalRoughnessPort";
    const RenderNodeOutputKey kGDepthPort = "kGDepthPort";

    // default shaders
    const ShaderDescKey kPointShadowShader = "kPointShadowShader";
    const ShaderDescKey kDirectionalShadowShader = "kDirectionalShadowShader";
    const ShaderDescKey kAniPointShadowShader = "kAniPointShadowShader";
    const ShaderDescKey kAniDirectionalShadowShader = "kAniDirectionalShadowShader";
    const ShaderDescKey kGBufferShader = "kGBufferShader";
    const ShaderDescKey kFloorGBufferShader = "kFloorGBufferShader";
    const ShaderDescKey kAniGBufferShader = "kAniGBufferShader";
    const ShaderDescKey kDefferShadingShader = "kDefferShadingShader";
    const ShaderDescKey kSSRShader = "kSSRShader";
    const ShaderDescKey kSSRBlurShader = "kSSRBlurShader";
    const ShaderDescKey kSkyBoxShader = "kSkyBoxShader";

    class RenderGraphManager : public SingletonBase<RenderGraphManager> {
    public:
        RenderGraphManager() = default;
        std::shared_ptr<SPW::RenderGraph> createRenderGraph(const std::shared_ptr<RenderBackEndI> &backend, const RenderGraphKey &type);
        unsigned int getRenderGraph(const RenderGraphKey &type);
        unsigned int getRenderNode(const RenderNodeKey &type);
        AttachmentPort getRenderNodeOutput(const RenderNodeOutputKey &type);
        ShaderDesc getShaderDesc(const ShaderDescKey &type);

        void initializeGraph(unsigned int w, unsigned int h);
        void onFrameChanged(unsigned int w, unsigned int h);
        void forEachGraph(const std::function<void(const std::shared_ptr<RenderGraph> &graph)> &callback);
        void forEachShader(const std::function<void(const ShaderDesc &shader)> &callback);

        inline std::optional<std::string> FindGraphName(unsigned int searchID)
        {
            for (const auto& [k,v] : graphs) {
                if (v->graph_id == searchID) {
                    return k;
                }
            }
            return {};
        }

    	inline std::optional<std::string> FindNodeName(unsigned int searchID)
        {
            for (const auto& [k,v] : nodes) {
                if (v->pass_id == searchID) {
                    return k;
                }
            }
            return {};
        }
        // inline void AddSkyboxGraph(std::shared_ptr<RenderGraph> skyBoxGraph, std::shared_ptr<ModelToScreenNode> skyBoxNode)
        // {
        //     graphs.insert({ kSkyboxShadingGraph, skyBoxGraph });
        //     nodes.insert({ kSkyboxNode, skyBoxNode });
        // }
        inline std::unordered_map<RenderGraphKey, std::shared_ptr<RenderGraph>>
    	GetRenderGraphs() const { return graphs; }
        inline std::unordered_map<RenderNodeKey, std::shared_ptr<RenderPassNodeI>>
        GetNodes() const { return nodes; }

#define GET_RENDER_GRAPH(key) SPW::RenderGraphManager::getInstance()->getRenderGraph(key)
#define GET_RENDER_NODE(key) SPW::RenderGraphManager::getInstance()->getRenderNode(key)
#define GET_RENDER_OUTPUT(key) SPW::RenderGraphManager::getInstance()->getRenderNodeOutput(key)
#define GET_SHADER_DESC(key) SPW::RenderGraphManager::getInstance()->getShaderDesc(key)
    private:
        std::unordered_map<RenderGraphKey, std::shared_ptr<RenderGraph>> graphs;
        std::unordered_map<RenderNodeKey, std::shared_ptr<RenderPassNodeI>> nodes;
        std::unordered_map<RenderNodeOutputKey, AttachmentPort> all_ports;
        std::unordered_map<ShaderDescKey, ShaderDesc> shaders;
    };
}