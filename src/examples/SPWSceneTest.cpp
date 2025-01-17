#include <iostream>
#include <memory>
#include <sol/sol.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include <glad/glad.h>

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyCodes.h"
// platform
#include "Platforms/OPENGL/OpenGLBackEnd.h"
#include "Platforms/OPENGL/OpenGLxGLFWContext.hpp"
// ecs
#include "EcsFramework/Entity/Entity.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
#include "EcsFramework/Component/PhysicalComponent/Collider.h"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/System/AudioSystem/AudioSystem.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Component/MouseComponent.hpp"
#include "EcsFramework/System/ControlSystem/MouseControlSystem.hpp"
#include "EcsFramework/System/ControlSystem/KeyControlSystem.hpp"
#include "EcsFramework/System/PhysicSystem/PhysicSystem.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/System/NewRenderSystem/SPWRenderSystem.h"
#include "EcsFramework/Component/AnimationComponent/AnimationComponent.h"
#include "EcsFramework/System/AnimationSystem/AnimationSystem.h"
#include "EcsFramework/Component/PhysicalComponent/RigidActor.h"
// render
#include "Render/DefaultRenderPass.hpp"
#include "Render/RenderGraphManager.h"
#include "SimpleRender.h"
#include "Utils/UUID.hpp"
// io
#include "IO/FileSystem.h"
#include "IO/ConfigManager.h"
// asset
#include "Asset/Serializer/EntitySerializer.h"
#include "Asset/ResourceManager/ResourceManager.h"
// ui
#include "ImGui/ImGuiEvent.hpp"
#include "ImGui/ImGuiManager.hpp"

auto CreateEmptyNode(const std::shared_ptr<SPW::Scene>& scene) -> std::shared_ptr<SPW::Entity>
{
	return scene->createEntity("emptyNode");
}

void CreateAProject()
{
}

const SPW::UUID& CreateACamera(const std::shared_ptr<SPW::Scene>& scene, float width, float height, bool main = true)
{
	// add a camera entity
	auto camera = scene->createEntity("main camera");
	camera->emplace<SPW::AudioListener>();
	auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
	mainCameraTrans->position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
	cam->fov = 60;
	cam->aspect = width / height;
	cam->near = 0.01;
	cam->far = 100;

	cam->whetherMainCam = main;
	//add a key component for testing, press R to rotate
	auto cameraKey = camera->emplace<SPW::KeyComponent>();
	auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode)
	{
		auto mainCameraTrans = e.component<SPW::TransformComponent>();
		auto rotMat = glm::eulerAngleYXZ(glm::radians(mainCameraTrans->rotation.y),
			glm::radians(mainCameraTrans->rotation.x),
			glm::radians(mainCameraTrans->rotation.z));
		glm::vec4 front = { 0, 0, -1, 0 };
		front = rotMat * front;
		glm::vec3 forward = { front.x, 0, front.z };
		forward = glm::normalize(forward);
		glm::vec3 up = { 0, 1, 0 };
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		if (keycode == SPW::Key::W)
			mainCameraTrans->position += 0.01f * forward;
		if (keycode == SPW::Key::S)
			mainCameraTrans->position -= 0.01f * forward;
		if (keycode == SPW::Key::A)
			mainCameraTrans->position -= 0.01f * right;
		if (keycode == SPW::Key::D)
			mainCameraTrans->position += 0.01f * right;
		if (keycode == SPW::Key::Q)
			mainCameraTrans->position -= 0.01f * up;
		if (keycode == SPW::Key::E)
			mainCameraTrans->position += 0.01f * up;
	};
	auto mouse = camera->emplace<SPW::MouseComponent>();
	mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias,
		double y_pos_bias)
	{
		auto transform = e.component<SPW::TransformComponent>();
		transform->rotation.x -= y_pos_bias * 0.02;
		transform->rotation.y -= x_pos_bias * 0.1;
	};
	cameraKey->onKeyHeldCallBack = cb;
	cameraKey->onKeyDownCallBack = cb;
	return camera->component<SPW::IDComponent>()->getID();
}

const SPW::UUID& CreateCamera2(const std::shared_ptr<SPW::Scene>& scene, float width, float height)
{
	// add a camera entity
	auto camera = scene->createEntity("not main camera");
	camera->emplace<SPW::AudioListener>();
	auto mainCameraTrans = camera->emplace<SPW::TransformComponent>();
	mainCameraTrans->position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	auto cam = camera->emplace<SPW::CameraComponent>(SPW::PerspectiveType);
	cam->fov = 60;
	cam->aspect = width / height;
	cam->near = 0.01;
	cam->far = 50;

	cam->whetherMainCam = false;
	//add a key component for testing, press R to rotate
	auto cameraKey = camera->emplace<SPW::KeyComponent>();
	auto cb = [](const SPW::Entity& e, SPW::KeyCode keycode)
	{
		auto mainCameraTrans = e.component<SPW::TransformComponent>();
		auto rotMat = glm::eulerAngleYXZ(glm::radians(mainCameraTrans->rotation.y),
			glm::radians(mainCameraTrans->rotation.x),
			glm::radians(mainCameraTrans->rotation.z));
		glm::vec4 front = { 0, 0, -1, 0 };
		front = rotMat * front;
		glm::vec3 forward = { front.x, 0, front.z };
		forward = glm::normalize(forward);
		glm::vec3 up = { 0, 1, 0 };
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		if (keycode == SPW::Key::W)
			mainCameraTrans->position += 0.01f * forward;
		if (keycode == SPW::Key::S)
			mainCameraTrans->position -= 0.01f * forward;
		if (keycode == SPW::Key::A)
			mainCameraTrans->position -= 0.01f * right;
		if (keycode == SPW::Key::D)
			mainCameraTrans->position += 0.01f * right;
		if (keycode == SPW::Key::Q)
			mainCameraTrans->position -= 0.01f * up;
		if (keycode == SPW::Key::E)
			mainCameraTrans->position += 0.01f * up;
	};
	// auto mouse = camera->emplace<SPW::MouseComponent>();
	// mouse->cursorMovementCallBack = [](const SPW::Entity& e, double x_pos, double y_pos, double x_pos_bias,
	//                                    double y_pos_bias)
	// {
	// 	auto transform = e.component<SPW::TransformComponent>();
	// 	transform->rotation.x -= y_pos_bias * 0.02;
	// 	transform->rotation.y -= x_pos_bias * 0.1;
	// };
	cameraKey->onKeyHeldCallBack = cb;
	cameraKey->onKeyDownCallBack = cb;
	return camera->component<SPW::IDComponent>()->getID();
}

std::shared_ptr<SPW::Entity> createPlight(const std::shared_ptr<SPW::Scene>& scene, glm::vec3 position, glm::vec3 color)
{
	auto light = scene->createEntity("light");
	auto lightTrans = light->emplace<SPW::TransformComponent>();
	auto lightCom = light->emplace<SPW::PointLightComponent>();
	lightCom->ambient = color;
	lightTrans->position = position;
	return light;
}

std::shared_ptr<SPW::Entity> createDirectionalLight(const std::shared_ptr<SPW::Scene>& scene, glm::vec3 rotation, glm::vec3 color)
{
	auto light = scene->createEntity("light");
	auto lightTrans = light->emplace<SPW::TransformComponent>();
	auto lightCom = light->emplace<SPW::DirectionalLightComponent>();
	lightCom->ambient = color;
	lightTrans->rotation = rotation;
	return light;
}

struct NoOpDeleter
{
	template <typename T>
	void operator()(T*) const noexcept
	{
	}
};

// test usage
class Transformer : public SPW::WindowEventResponder
{
public:
	explicit Transformer(const std::shared_ptr<SPW::EventResponderI>& parent)
		: SPW::WindowEventResponder(parent)
	{
	}

	bool onWindowResize(int w, int h) override
	{
		std::cout << "window resize" << "(" << w << ", " << h << ")" << std::endl;
		bool should_update = false;
		if (w < 500)
		{
			w = 500;
			should_update = true;
		}
		if (h < 400)
		{
			h = 400;
			should_update = true;
		}
		if (should_update && !window.expired())
			window.lock()->setSize(w, h);
		// set projection
		// TODO: add a responder to each camera
		scene.lock()->forEach([=](SPW::CameraComponent* cam) {
			cam->aspect = float(w) / float(h);
			if (cam->getType() == SPW::UIOrthoType) {
				cam->right = w;
				cam->top = h;
			}
			}, SPW::CameraComponent);
		return true;
	}

	// windows size is not equal to frame buffer size
	bool onFrameResize(int w, int h) override
	{
		glViewport(0, 0, w, h);
		std::cout << "frame resize" << "(" << w << ", " << h << ")" << std::endl;
		// set projection
		return false;
	}

	const char* getName() final
	{
		return "Transformer";
	}

	std::weak_ptr<SPW::WindowI> window;
	std::weak_ptr<SPW::Scene> scene;
};

// #define SAVE_SKYBOX
#define LOAD_ASSET

class SPWTestApp : public SPW::AppDelegateI
{
public:
	explicit SPWTestApp(std::shared_ptr<SPW::EventResponderI>& app, const char* name)
		: SPW::AppDelegateI(app), _name(name)
	{
	}

	void onAppInit() final
	{
		std::shared_ptr<SPW::GlfwWindow> window = std::make_shared<SPW::GlfwWindow>();
		app->window = window;
		app->window->setSize(1280, 720);
		app->window->setTitle("Editor Test");

		// weak strong dance
		std::weak_ptr weak_window = window;
		window->onWindowCreated([weak_window, this](GLFWwindow* handle)
			{
				if (weak_window.expired())
				{
					return;
				}
				// create graphics context
				weak_window.lock()->graphicsContext = std::make_shared<SPW::OpenGLxGLFWContext>(handle);
				// initial context
				weak_window.lock()->graphicsContext->Init();

				// create render back end
				renderBackEnd = std::make_shared<SPW::OpenGLBackEnd>();

				// create scene
				scene = SPW::Scene::create(app->delegate.lock());

				// create render system
				auto rendersystem = std::make_shared<SPW::SPWRenderSystem>(scene, renderBackEnd,
					weak_window.lock()->frameWidth(),
					weak_window.lock()->frameHeight());
				// add system
				scene->m_renderSystem = rendersystem;
				scene->addSystem(std::make_shared<SPW::AudioSystem>(scene));
				scene->addSystem(rendersystem);
				scene->addSystem(std::make_shared<SPW::KeyControlSystem>(scene));
				scene->addSystem(std::make_shared<SPW::MouseControlSystem>(scene));
				scene->addSystem(std::make_shared<SPW::AnimationSystem>(scene));
				scene->addSystem(std::make_shared<SPW::PhysicSystem>(scene));

				// ------ create main render graph ----------------
				SPW::RenderGraphManager::getInstance()->createRenderGraph(renderBackEnd, SPW::kDefferShadingGraph);
				SPW::RenderGraphManager::getInstance()->forEachShader([&rendersystem](const SPW::ShaderDesc& shader) {
					rendersystem->addShaderDescriptor(shader);
					});

// 				SPW::RenderGraphManager::getInstance()->AddSkyboxGraph(rendersystem->skyBoxGraph, rendersystem->skyBoxNode);

				// --------------- create shader ---------------

				auto camera_id = CreateACamera(scene, weak_window.lock()->width(), weak_window.lock()->height());

				// --------------------------------------------------------------------------------
				SPW::ResourceManager::getInstance()->m_CameraIDMap["main"] = camera_id;
				SPW::ResourceManager::getInstance()->activeCameraID = camera_id;

				auto ptr = std::shared_ptr<EventResponderI>(app);
				m_ImguiManager = std::make_shared<SPW::ImGuiManager>(handle, scene, ptr);

				// init scene
				scene->initial();
			});
	}

	void beforeAppUpdate() final
	{
		scene->beforeUpdate();
	}

	void onAppUpdate(const SPW::TimeDuration& du) final
	{
		// physics, computation
		scene->onUpdate(du);
	}

	void afterAppUpdate() final
	{
		scene->afterUpdate();
		// ----------------------------------------------------------------------------------------
		m_ImguiManager->Render();
	}

	void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>>& events) final
	{
		// for (auto &e : events) {
		//     DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
		// }
	}

	void onAppStopped() final
	{
		sol::state state;
		state.open_libraries(sol::lib::base, sol::lib::package);
		std::string x = state["package"]["path"];
		state["package"]["path"] = x + ";./resources/scripts/lua/?.lua";
		try
		{
			if (state.script_file("./resources/scripts/lua/test.lua").valid())
			{
				sol::protected_function main_function = state["main"];
				sol::protected_function_result result = main_function();
				if (!result.valid())
				{
					std::cout << "execution error" << std::endl;
				}
			}
		}
		catch (sol::error& e)
		{
			std::cout << e.what() << std::endl;
		}
		std::cout << "app stopped" << std::endl;
		scene->onStop();
	}

	void solveEvent(const std::shared_ptr<SPW::EventI>& e) final
	{
		e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
			[this](SPW::WindowEvent* e)
			{
				// close application

				app->stop();
				return true;
			});
		SPW::EventResponderI::solveEvent(e);
	}

	const char* getName() final { return _name; }
	const char* _name;
	std::shared_ptr<SimpleRender> render;
	std::shared_ptr<SPW::Scene> scene;
	std::shared_ptr<SPW::RenderBackEndI> renderBackEnd;
	std::shared_ptr<SPW::ImGuiManager> m_ImguiManager;
};

// main entrance
int main(int argc, char** argv)
{

//	SPW::ConfigManager::WriteDefaultConfig();

	if (SPW::ConfigManager::Boost())
		std::cout << "Boost" << std::endl;
	else
		return -1;
	
	// app test
	auto appProxy =	
		SPW::Application::create<SPWTestApp>("SPWTestApp");
	return appProxy->app->run(argc, argv);
}
