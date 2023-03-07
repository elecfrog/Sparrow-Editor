//
// Created by ZhangHao on 2023/2/19.
//

#include "RenderSystem.h"
#include "EcsFramework/Component/ComponentI.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ModelComponent.h"
#include "EcsFramework/Component/LightComponent.hpp"
#include "glm/fwd.hpp"
#include "Render/Light.h"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <string>

void SPW::RenderSystem::initial()
{
    renderBackEnd->Init();
    renderBackEnd->loadShaderLib("./resources/shaders/baselib");
}

void SPW::RenderSystem::beforeUpdate() {
    // clear buffer

    frameBuffer->bind();
    renderBackEnd->DepthTest(true);

    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderBackEnd->SetClearColor(glm::vec4(0.5));
    renderBackEnd->Clear();
}

void SPW::RenderSystem::onUpdate(SPW::TimeDuration dt) {

}

void SPW::RenderSystem::afterUpdate(){
    // get all normal cameras and UI cameras
    RenderCamera uiCamera;

    ComponentGroup<SPW::IDComponent,
        SPW::CameraComponent,
        SPW::TransformComponent> cameraGroup;

    ComponentGroup<SPW::IDComponent, 
                    SPW::TransformComponent, 
                    SPW::LightComponent> lightGroup;

    locatedScene.lock()->forEachEntityInGroup(cameraGroup,
        [this, &uiCamera, &cameraGroup](const Entity &en){
            RenderCamera camera = en.combinedInGroup(cameraGroup);
            if (std::get<1>(camera)->getType() != UIOrthoType)
                // render models on this camera
                renderModelsWithCamera(camera);
            else
                // pick uiCamera out
                // UI should be rendered in final step
                uiCamera = camera;
        });

    frameBuffer->unbind();

    // RenderPass n-1, PostProcessing
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::DepthTest, false));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::SetClearColor,glm::vec4(0.5)));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::Clear));
    postProcessPass.pushCommand(SPW::RenderCommand(&SPW::RenderBackEndI::drawInTexture,PostProcessingEffects::FXAA));
    postProcessPass.executeWithAPI(renderBackEnd);

    // RenderPass n, render in Game GUI
    //renderModelsWithCamera(uiCamera);
}


void SPW::RenderSystem::renderModelsWithCamera(const RenderCamera &camera) {
    if (std::get<0>(camera) == nullptr) {
        return;
    }
    UUID currentID = std::get<0>(camera)->getID();
    auto cameraCom = std::get<1>(camera);
    auto transformCom = std::get<2>(camera);
    glm::vec3 camPos = transformCom->position;

    using ShaderModelMap = std::unordered_map<ShaderHandle, std::vector<SPW::Entity>, ShaderHash, ShaderEqual>;

    // predefine render objects
    ComponentGroup<SPW::IDComponent,
        SPW::ModelComponent,
        SPW::TransformComponent> modelGroup;

    // 1. get associated objects from camera
    std::vector<SPW::Entity> renderModels;
    locatedScene.lock()->forEachEntityInGroup(
        modelGroup,
        [&currentID, &renderModels, this](const Entity &en) {
            auto modelCom = en.component<SPW::ModelComponent>();
            // passing data to GPU
            if (modelCom->bindCameras.find(currentID) != modelCom->bindCameras.end()) {
                if (!modelCom->ready) {
                    modelCom->model->setUpModel(renderBackEnd);
                    modelCom->ready = true;
                }
                renderModels.push_back(en);
            }
    });

    // 2. calculate VP from camera
    glm::mat4x4 V, P;
    glm::mat4x4 cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, transformCom->position);
    cameraTransform = cameraTransform * glm::eulerAngleXYZ(glm::radians(transformCom->rotation.x),
                       glm::radians(transformCom->rotation.y),
                       glm::radians(transformCom->rotation.z));

    glm::vec4 eye(0, 0, 1, 1), look_at(0, 0, 0, 1), up(0, 1, 0, 0);
    V = glm::lookAt(glm::vec3(cameraTransform * eye),
                    glm::vec3(cameraTransform *look_at),
                    glm::vec3(cameraTransform * up));
    if (cameraCom->getType() == SPW::PerspectiveType) {
        P = glm::perspective(glm::radians(cameraCom->fov),
                             cameraCom->aspect,
                             cameraCom->near,
                             cameraCom->far);

    } else {
        P = glm::ortho(cameraCom->left, cameraCom->right,
                       cameraCom->bottom, cameraCom->top, cameraCom->near, cameraCom->far);
    }

    // get lights from scene
    ComponentGroup<SPW::IDComponent,
        SPW::LightComponent,
        SPW::TransformComponent> lightGroup;
    std::vector<PLight> pLights;
    std::vector<DLight> dLights;
    locatedScene.lock()->forEachEntityInGroup(lightGroup, [&lightGroup, &pLights, &dLights](const Entity &en){
        auto [id, light, trans] = en.combinedInGroup(lightGroup);
        if (light->getType() == PointLightType) {
            PLight pl = {};
            pl.position = trans->position;
            pl.ambient = light->ambient;
            pl.diffuse = light->diffuse;
            pl.specular = light->specular;
            pl.constant = light->constant;
            pl.linear = light->linear;
            pl.quadratic = light->quadratic;
            pLights.push_back(pl);
        } else {
            glm::vec4 dir = {0, 0, -1, 0};
            auto rotMat = glm::eulerAngleXYZ(glm::radians(trans->rotation.x),
                       glm::radians(trans->rotation.y),
                       glm::radians(trans->rotation.z));
            DLight dl = {};
            dl.direction = dir * rotMat;
            dl.ambient = light->ambient;
            dl.diffuse = light->diffuse;
            dl.specular = light->specular;
            dLights.push_back(dl);
        }
    });
    

    // RenderPass 1, shadow
    // sort models with program, build a map with shadow_program -> models[]
    auto renderPass = [this, &renderModels, &V, &P, &camPos, &pLights, &dLights](bool isShadow){
        ShaderModelMap programModelMap;
        for (auto &en : renderModels) {
            // get program that used in shadow rendering
            auto program =
                isShadow ? en.component<SPW::ModelComponent>()->shadowProgram :
                en.component<SPW::ModelComponent>()->modelProgram;

            if (program.isValid()) {
                // record models in current program
                programModelMap[program].push_back(en);
            }
        }

        // for each program, draw every model
        for (auto [handle, entities] : programModelMap) {
            auto shader = renderBackEnd->getShader(handle);
            shader->Bind();
            shader->SetUniformValue<glm::vec3>("camPos", camPos);
            shader->SetUniformValue<glm::mat4>("V", V);
            shader->SetUniformValue<glm::mat4>("P", P);
            // bind light
            for (unsigned int i = 0; i < pLights.size(); i++) {
                auto pl = pLights[i];
                shader->setPLight(i, pl);
            }
            for (unsigned int i = 0; i < dLights.size(); i++) {
                auto dl = dLights[i];
                shader->setDLight(i, dl);
            }
            shader->SetUniformValue("PLightCount", int(pLights.size()));
            shader->SetUniformValue("DLightCount", int(dLights.size()));
            // render every model in this shader
            for (auto &model : entities) {
                auto modelCom = model.component<ModelComponent>();
                auto transformCom = model.component<TransformComponent>();

                glm::mat4 M = glm::mat4(1.0f);
                M = glm::translate(M,transformCom->position);
                M = M * glm::eulerAngleXYZ(glm::radians(transformCom->rotation.x),
                                      glm::radians(transformCom->rotation.y),
                                      glm::radians(transformCom->rotation.z));
                M = glm::scale(M, transformCom->scale);

                shader->SetUniformValue<glm::mat4>("M", M);
                modelCom->model->Draw(renderBackEnd, handle);
            }
        }

    };

    // shadow pass
    renderPass(true);

    // model pass
    renderPass(false);
}

void SPW::RenderSystem::onStop() {

}

bool SPW::RenderSystem::onFrameResize(int w, int h) {
    std::cout << "RenderSystem frame changed" << std::endl;
    // update frame buffer here
    frameBuffer->deleteFrameBuffer();
    frameBuffer->genFrameBuffer();
    frameBuffer->bind();
    frameBuffer->AttachColorTexture(w,h,0);
    frameBuffer->AttachDepthRenderBuffer(w,h);
    frameBuffer->CheckFramebufferStatus();
    frameBuffer->unbind();
    return false;
}