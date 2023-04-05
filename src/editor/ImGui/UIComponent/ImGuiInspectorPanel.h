/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "stb_image.h"

namespace SPW {

class ImGuiInspectorPanel : public ImGuiPanel {
public:
  ImGuiInspectorPanel(std::string title, bool *open = nullptr)
      : ImGuiPanel(std::move(title), open)
	{}

  void SetSelectedGameObject(const Entity& e) {m_Entity = &e;}

  //Icon
  ImTextureID sound_texture_id = generateTextureID("./resources/texture/container.jpg");

protected:
    //texture properties
    float image_width = 0.2f;
    float image_height = 0.2f;
    int width, height, nrChannels;
    ImVec2 image_size = ImVec2(20, 20);

    //
    GLuint sound_id;

    ImTextureID generateTextureID(const char* path){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        glGenTextures(1, &sound_id);
        glBindTexture(GL_TEXTURE_2D, sound_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        ImTextureID my_texture_id = (void*)(intptr_t)sound_id;

        return my_texture_id;
   };

	void Draw() override
	{
        if (m_Entity != nullptr) 
        {
            ImGui::Text("Name:"); ImGui::SameLine();
            std::string name = m_Entity->component<SPW::NameComponent>()->getName();
            char nameBuffer[256];
            strcpy(nameBuffer, m_Entity->component<SPW::NameComponent>()->getName().c_str());
            ImGui::InputText("*", nameBuffer, 256);
            if (ImGui::Button("Update Name"))
            {
                name.assign(nameBuffer);
                m_Entity->component<SPW::NameComponent>()->updateName(name);
                std::cout << "Update name to " << m_Entity->component<SPW::NameComponent>()->getName() << std::endl;
            }
            if (m_Entity->has<SPW::TransformComponent>()) 
                DrawTransformComponent(m_Entity->component<SPW::TransformComponent>());
            if (m_Entity->has<SPW::CameraComponent>())
                DrawCameraComponent(m_Entity->component<SPW::CameraComponent>());
            if (m_Entity->has<SPW::PointLightComponent>())
                DrawPointLightComponent(m_Entity->component<SPW::PointLightComponent>());
            if (m_Entity->has<SPW::DirectionalLightComponent>())
                DrawDirectionalLightComponent(m_Entity->component<SPW::DirectionalLightComponent>());
            //TODO：音效
            if(m_Entity->has<SPW::AudioComponent>())
                DrawAudioComponent(m_Entity->component<SPW::AudioComponent>());
            if(m_Entity->has<SPW::AudioListener>())
                DrawAudioListener(m_Entity->component<SPW::AudioListener>());
            //TODO: Key
            if(m_Entity->has<SPW::KeyComponent>())
                DrawKeyComponent(m_Entity->component<SPW::KeyComponent>());

            ImGui::Button("Add Component", ImVec2(200, 20));
        }
	}

private:

    void DrawAxis(SPW::TransformComponent* component)
    {
      //TODO::Draw axis
  }
    void DrawTransformComponent(SPW::TransformComponent* component) const
    {
        ImGui::PushID("Transform"); 
        if (ImGui::TreeNode("Transform")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("Transform", ImVec2(0, 90), true)) {
                // draw component properties
                ImGui::InputFloat3("Position", glm::value_ptr(component->position));
                ImGui::InputFloat3("Rotation", glm::value_ptr(component->rotation));
                ImGui::InputFloat3("Scale", glm::value_ptr(component->scale));
    
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    
    void DrawCameraComponent(SPW::CameraComponent* component) const
    {
        ImGui::PushID("Camera");
        if (ImGui::TreeNode("Camera")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("Camera", ImVec2(0, 120), true))
            {
                if(component->getType() == CameraType::PerspectiveType)
                {
                    // draw component properties
                    ImGui::InputFloat("FOV", &component->fov);
                    ImGui::InputFloat("Aspect", &component->aspect);
                    ImGui::InputFloat("Near", &component->near);
                    ImGui::InputFloat("Far", &component->far);
                }
                else if(component->getType() == CameraType::OrthoType)
                {
                    // draw component properties
                    ImGui::InputFloat("Left", &component->left);
                    ImGui::InputFloat("Right", &component->right);
                    ImGui::InputFloat("Bottom", &component->bottom);
                    ImGui::InputFloat("Top", &component->top);
                }
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
    
    void DrawPointLightComponent(SPW::PointLightComponent* component) const
    {
        ImGui::PushID("Point Light");
        if (ImGui::TreeNode("Point Light")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("Point Light", ImVec2(0, 180), true))
            {
                // draw component properties
                ImGui::InputFloat("Constant", &component->constant);
                ImGui::InputFloat("Linear", &component->linear);
                ImGui::InputFloat("Quadratic", &component->quadratic);
                ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
                ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
                ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
    
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

	void DrawDirectionalLightComponent(SPW::DirectionalLightComponent* component) const
    {
        ImGui::PushID("Dictional Light");
        if (ImGui::TreeNode("Dictional Light")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("Dictional Light", ImVec2(0, 90), true))
            {
                // draw component properties
                ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
                ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
                ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
    
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void DrawAudioComponent(SPW::AudioComponent* component) const
    {
        ImGui::PushID("Audio Source");
        if (ImGui::TreeNode("Audio Source")) /* TODO: add icon*/
        {
            int i = 1,j = 1;
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            for(auto &a : component->allSounds){
                std::string clipNum = "Audio Clip " + std::to_string(i++);
                std::string AudioSourceNum = "Audio Source " + std::to_string(j++);
                if (ImGui::TreeNode(AudioSourceNum.c_str())){
                    ImGui::InputText(clipNum.c_str(),const_cast<char*>(a.first.c_str()),256);
                    ImGui::Checkbox("is 3D",&a.second->is3D);
                    ImGui::Checkbox("is Loop",&a.second->isLoop);
                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::TreePop();
                }

            }

            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void DrawAudioListener(SPW::AudioListener* component) const
    {
        ImGui::PushID("Audio");
        if (ImGui::TreeNode("Audio")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("AudioSource", ImVec2(0, 90), true)) {
                // draw component properties
                ImGui::InputInt("Listener ID",&component->Listener_id);

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void DrawKeyComponent(SPW::KeyComponent* component) const
    {
        ImGui::PushID("Event System");
        if (ImGui::TreeNode("Event System")) /* TODO: add icon*/
        {
            glBindTexture(GL_TEXTURE_2D,sound_id);
            ImGui::Image(sound_texture_id, image_size); // Icon
            if (ImGui::BeginChild("Event System", ImVec2(0, 90), true)) {
                // draw component properties
                ImGui::Checkbox("Key Enable",&component->keyEnable);

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    const SPW::Entity* m_Entity = nullptr;
};

}
