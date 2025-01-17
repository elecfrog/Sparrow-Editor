/*
 * @date   2023-04-11
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiFileExplorer.h"
#include "IO/FileSystem.h"
#include "IO/ConfigManager.h"
#include "ImGui/ImGuiManager.hpp"

namespace SPW
{
	void ImGuiFileExplorer::Draw()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;
		bool* p_open = nullptr;
		ImGui::Begin(ICON_FA_FILE "  Selected Folder",p_open,window_flags);

		if (!selected_dir.empty())
			DisplaySelectedFolder(selected_dir);

		DisplayLuaSettingMenu();

		ImGui::End();

		const char* icon = ICON_FA_FOLDER"  ";
		ImGui::Text(ICON_FA_LAPTOP_CODE" Runtime LuaLib");
		DisplayImGuiFileExplorer(icon, Config::k_EngineLualib);
		ImGui::Separator();

		ImGui::Text(ICON_FA_LAPTOP_CODE" Project");
		DisplayImGuiFileExplorer(icon, Config::k_WorkingProjectRoot);
		ImGui::Separator();

		ImGui::Text(ICON_FA_LAPTOP_CODE" Engine Resources");
		DisplayImGuiFileExplorer(icon, Config::k_EngineRoot);
		ImGui::Separator();

		DisplayImageMenu();
		DisplayJsonMenu();
		DisplayImagePanel();

	}

	void ImGuiFileExplorer::DisplayImGuiFileExplorer(const char* icon, const std::string& path)
	{
		for (const auto& entry: fs::directory_iterator(FilePath(path)))
		{
			auto& entryPath = entry.path();
			if (fs::is_directory(entryPath))
			{
				std::string folderName = entryPath.filename().string();
				bool open = false;
				//const char* icon = ICON_FA_FOLDER"  ";
				if (ImGui::TreeNode((icon + folderName).c_str()))
				{
					DisplayImGuiFileExplorer(icon, entryPath.string());
					ImGui::TreePop();
				}

				if (ImGui::IsItemClicked())
				{
					selected_dir = entryPath.string();
					open = !open;
				}
			}
			else
			{
				std::string fileName = entryPath.filename().string();
				ImGui::Text("%s", fileName.c_str());
			}
		}
	}

	void ImGuiFileExplorer::DisplaySelectedFolder(const std::string& folderPath)
	{
		// set row numbers and row width
		int numColumns = 4;
		ImGui::Columns(numColumns);

		for (const auto& entry: fs::directory_iterator(folderPath))
		{
			auto& entryPath = entry.path();
			std::string fileName = entryPath.filename().string();

			int64_t icon_id;

			if (fs::is_directory(entryPath))
			{
				icon_id = m_IconManager->GetLibIcon("folder");
				if (ImGui::ImageButton(reinterpret_cast<void*>(icon_id), m_DefalutImageSize))
				{
					// selected_dir = entryPath.string();
					// std::cout << "CLiked" << std::endl;
				}
				if (ImGui::IsItemClicked())
				{
					// std::cout << "CLiked" << std::endl;
					selected_dir = entryPath.string();
				}
			}
			else
			{
				if (entryPath.extension() == ".obj")
					icon_id = m_IconManager->GetLibIcon("obj");
				else if (entryPath.extension() == ".wav" || entryPath.extension() == ".mp3")
					icon_id = m_IconManager->GetLibIcon("music");
				else if (entryPath.extension() == ".png" || entryPath.extension() == ".jpg"
					|| entryPath.extension() == ".jpeg" || entryPath.extension() == ".dds")
					icon_id = m_IconManager->GenerateIconID(entryPath.string());
				else if (entryPath.extension() == ".anim")
					icon_id = m_IconManager->GetLibIcon("anim");
				else if (entryPath.extension() == ".cpp")
					icon_id = m_IconManager->GetLibIcon("cpp");
				else if (entryPath.extension() == ".h")
					icon_id = m_IconManager->GetLibIcon("h");
				else if (entryPath.extension() == ".gltf")
					icon_id = m_IconManager->GetLibIcon("gltf");
				else if (entryPath.extension() == ".json")
					icon_id = m_IconManager->GetLibIcon("json");
				else if (entryPath.extension() == ".lua")
					icon_id = m_IconManager->GetLibIcon("lua");
				else if (entryPath.extension() == ".mesh")
					icon_id = m_IconManager->GetLibIcon("mesh");
				else if (entryPath.extension() == ".fbx")
					icon_id = m_IconManager->GetLibIcon("fbx");
				else
					icon_id = m_IconManager->GetLibIcon("file");

				ImGuiStyle& style = ImGui::GetStyle();

				style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
				if (ImGui::ImageButton(reinterpret_cast<void*>(icon_id), m_DefalutImageSize))
				{
					// selected_dir = entryPath.string();
				}

				if (ImGui::IsItemClicked(0) && (entryPath.extension() == ".json" || entryPath.extension() == ".lua" || entryPath.extension() == ".asset"))
				{
					std::string command = "code " + entryPath.string();

					if(std::system(command.c_str()))
						std::cerr << "Error: Failed to open the file in Visual Studio Code" << std::endl;
				}

				if (ImGui::IsItemClicked(1) && (entryPath.extension() == ".json" || entryPath.extension() == ".asset"))
				{
					m_ShowJsonMenu = true;
					m_Path = entryPath.string();
				}


				if (ImGui::IsItemClicked(1) && (entryPath.extension() == ".lua" ))
				{
					SetLuaPathCallback(entryPath);
				}

				if (ImGui::IsItemClicked(1) && (entryPath.extension() == ".png" || entryPath.extension() == ".jpg" || entryPath.extension() == ".jpeg"
					||  entryPath.extension() == ".dds"))
				{
					//m_ShowImagePanel = true;

					//m_OpenGLTexture = static_cast<GLuint>(SPW::AssetManager::LoadRawImage(entryPath.string()));
					//ImTextureID imgui_texture = LoadImage(entryPath.string());
					m_OpenGLTexture = AssetManager::LoadRawImage(entryPath.string());
					m_Path = entryPath.string();
					m_ShowImageMenu = true;
				}

			}

			ImGui::Text("%s", fileName.c_str());
			ImGui::Spacing();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);

	}

	int64_t SPW::ImGuiFileExplorer::LoadImage(const std::string& file_path) {
		int width, height, channels;
		unsigned char* image_data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
		if (image_data == NULL) {
			return 0;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum format;
		switch (channels)
		{
		case 1:
			format = GL_R;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			stbi_image_free(image_data);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(image_data);
		return texture;
	}

	void SPW::ImGuiFileExplorer::DisplayImageMenu()
	{
		if (m_ShowImageMenu)
		{
			ImGui::OpenPopup("ImageMenu");
		}

		if (ImGui::BeginPopup("ImageMenu"))
		{
			if (ImGui::MenuItem("image preview"))
			{
				m_ShowImagePanel = true;
			}
			if (ImGui::MenuItem("compress image"))
			{
				auto data = AssetManager::LoadTextureData(m_Path);
				(AssetManager::CompressImage(std::move(data), m_Path));
			}
			if (ImGui::MenuItem("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			m_ShowImageMenu = false;
			ImGui::EndPopup();
		}
		
	}

	void ImGuiFileExplorer::DisplayImagePanel()
	{
		if (m_ShowImagePanel)
		{
			bool isOpen = ImGui::Begin("Image Preview", &m_ShowImagePanel);
			if (isOpen)
			{
				if (m_OpenGLTexture != 0)
				{
					ImTextureID imgui_texture = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_OpenGLTexture));
					ImGui::Image(imgui_texture, ImVec2(300, 300));
				}
			}
			else
			{
				// Close the panel and release the texture
				m_ShowImagePanel = false;

				// Delete the OpenGL texture
				if (m_OpenGLTexture != 0)
				{
					glDeleteTextures(1, &m_OpenGLTexture);
					m_OpenGLTexture = 0;
				}
			}
			ImGui::End();
		}
	}


	void SPW::ImGuiFileExplorer::DisplayJsonMenu()
	{
		if (m_ShowJsonMenu)
		{
			ImGui::OpenPopup("FileContextMenu");
		}

		if (ImGui::BeginPopup("FileContextMenu"))
		{
			if (ImGui::MenuItem("Load"))
			{
				auto asset_data = AssetManager::LoadAsset(m_Path);
				ResourceManager::getInstance()->m_AssetDataMap[asset_data.assetName] = asset_data;
			}
			if (ImGui::MenuItem("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			m_ShowJsonMenu = false;
			ImGui::EndPopup();
		}
	}


	void ImGuiFileExplorer::SetLuaPathCallback(const std::filesystem::path& path)
	{
		// std::cout << path << std::endl;
		luaPath = path.string();
		FileSystem::ResolveSlash(luaPath);

		ImGui::OpenPopup("SetLuaPath");
	}

	void ImGuiFileExplorer::DisplayLuaSettingMenu()
	{
		if (ImGui::BeginPopup("SetLuaPath"))
		{
			if (ImGui::MenuItem("Set it as default file"))
			{
				toml::table cfg = ConfigManager::GetConfigContext();

				toml::table scriptEntries = toml::table
				{
					// get file name | filepath
					{"Entry", luaPath}
				};
				cfg.insert_or_assign("DefaultScript", scriptEntries);
				ConfigManager::WriteDefaultScript(cfg);
			}

			if (ImGui::MenuItem("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}