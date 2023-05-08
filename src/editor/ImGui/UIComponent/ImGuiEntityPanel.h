/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiInspectorPanel.h"
#include "ImGuiPanel.h"
#include "ImGui/IconsFontAwesome6.h"

namespace SPW
{
	class ImGuiEntityPanel : public ImGuiPanel
	{
	public:
		using MenuItemCallback = std::function<void()>;

		struct MenuItem
		{
			std::string ID;
			std::string name;
			MenuItemCallback callback;
		};

		ImGuiEntityPanel(const std::string& title, bool* open = nullptr)
			: ImGuiPanel(ICON_FA_LIST"  "+title, open, ImGuiWindowFlags_NoMove )
		{	}

		void UpdateMenuItemLabel(const std::string& id, const std::string& label);
		void AddMenuItem(const std::string& id, const std::string& label, MenuItemCallback callback);

		void ClearItems()
		{
			m_Items.clear();
		}

		void RemoveMenuItem(const std::string& ID)
		{
			const auto it = m_Items.find(ID);

			if (it != m_Items.end())
			{
				m_Items.erase(it);
			}
		}

		void SetActiveScene(const std::shared_ptr<Scene>& scene)
		{
			scene_ptr = scene.get();
		}

	protected:
		void Draw() override
		{
			for (const auto& item_pair : m_Items)
			{
				const auto& item = item_pair.second;
				if (ImGui::Button("x"))
				{
					scene_ptr->deleteEntity(scene_ptr->getEntityByID(item_pair.first));
					RemoveMenuItem(item_pair.first);
				}
				ImGui::SameLine();
				ImGui::BeginGroup();
				bool is_selected = false;
				ImVec2 item_min = ImGui::GetItemRectMin();
				ImVec2 item_max = ImGui::GetItemRectMax();
				ImGui::Selectable((ICON_FA_CUBE"		"+item.name).c_str(), &is_selected, ImGuiSelectableFlags_None, ImVec2(0, item_max.y - item_min.y));
				if (ImGui::IsItemClicked())
				{
					item.callback();
				}
				ImGui::EndGroup();
			}


			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Add Entity").x) * 0.5f);
			if( ImGui::Button( "Add Entity" ) )
			{
				auto new_gameObject = scene_ptr->createEntity("NewGameObject");
				new_gameObject->emplace<TransformComponent>();
			}
		}

	private:
		std::unordered_map<std::string, MenuItem> m_Items;
		Scene* scene_ptr;
	};
}
