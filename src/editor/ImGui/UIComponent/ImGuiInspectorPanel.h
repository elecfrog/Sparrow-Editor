/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/ComponentTypes.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "stb_image.h"

#include "ImGui/ImGuiIconManager.hpp"
#include "ImGui/ImGuiMessageBox/ImGuiMessageBox.h"
#include "Asset/ResourceManager/ResourceManager.h"
#include "ImGui/IconsFontAwesome6.h"
#include "ImGui/ImGuiFileDialog.h"
#include "IO/FileSystem.h"

namespace SPW
{
	class ImGuiInspectorPanel : public ImGuiPanel
	{
	public:
		ImGuiInspectorPanel(std::string title, ImGuiIconManager* iconManager, bool* open = nullptr)
			: ImGuiPanel(ICON_FA_CIRCLE_EXCLAMATION"  "+std::move(title), open,ImGuiWindowFlags_NoMove )
			, m_IconManager(iconManager)
// 			, m_FileDialog(std::make_shared<ImGuiFileDialog>())
		{
			// = ;
		}

		void SetActiveScene(const std::shared_ptr<Scene>& scene) { scene_ptr = scene; }

		void SetNoneSelectedGameObject() { m_Entity = nullptr; }

		void SetSelectedGameObject(const Entity& e);

		// 		bool deleteEntity = false;

	protected:
		void Draw() override;

	private:
		void DrawTransformComponent(TransformComponent* component);
		void DrawMeshComponent(MeshComponent* component);
		void DrawCameraComponent(CameraComponent* component) const;
		void DrawLightComponent() const;
		void DrawAudioComponent(AudioComponent* component) const;
		void DrawAudioListener(AudioListener* component) const;
		void DrawKeyComponent(KeyComponent* component) const;
		void DrawAnimationComponent(AnimationComponent* component) const;
		void DrawHierarchyNode(AnimationComponent* component, const HierarchyNode& node) const;
		void Loop();

//		void SaveEntityCallback();

	private:
		std::shared_ptr<Entity> m_Entity = nullptr;
		std::shared_ptr<Scene> scene_ptr = nullptr;
		std::unordered_map<ComponentType, bool> componentStatus;

		ImGuiIconManager* m_IconManager;
		ImVec2 k_DefalutImageSize = ImVec2(20, 20);
		std::unique_ptr<ImGuiMessageBox> msgBox_Inspector;

// 		std::shared_ptr<ImGuiFileDialog>		m_FileDialog;


		bool show_selectingMesh = false;
		bool show_naming = false;
		bool show_addcomponent = false;
		char m_PendingName[256] = "";



		// std::string convertToString(char* array)
		// {
		// 	std::stringstream ss;
		// 	ss << array;
		// 	return ss.str();
		// }
	};

}
