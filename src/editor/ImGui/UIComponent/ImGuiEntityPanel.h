/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiInspectorPanel.h"
#include "ImGuiPanel.h"

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
			: ImGuiPanel(title, open)
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
				if (ImGui::MenuItem(item.name.c_str()))
				{
					item.callback();
				}
			}

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
