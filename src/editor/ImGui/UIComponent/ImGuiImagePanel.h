#include "ImGuiPanel.h"
#include "ImGui/UIComponent/ImGuiPanel.h"

namespace SPW {

	class ImGuiImagePanel : public ImGuiPanel
	{
	public:
		ImGuiImagePanel(int64_t tid)
			: ImGuiPanel("Image Panel")
			, texture_id(tid)
		{	}

	protected:

		void Begin() override
		{
			ImGui::Begin(ImGuiPanel::m_title.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);
		}

		void Draw() override
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetCursorPos(ImVec2(0, 0));

			ImGui::Image
			(
				(void*)texture_id,
				windowSize,
				ImVec2(0.0, 1.0),
				ImVec2(1.0, 0.0),
				ImVec4(1, 1, 1, 1),
				ImVec4(0, 0, 0, 0.f)
			);
		}

	private:
		int64_t texture_id;
	};

}
