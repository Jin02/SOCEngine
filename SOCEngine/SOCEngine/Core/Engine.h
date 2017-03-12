#pragma once

#include "Common.h"
#include "IScene.h"
#include <memory>

#include <chrono>

namespace Core
{
	class Engine final
	{
	public:
		Engine();
		Engine(IScene* scene);

		DISALLOW_ASSIGN(Engine);
		DISALLOW_COPY_CONSTRUCTOR(Engine);

		// Scene
		void RunScene();
		void ChangeScene(IScene* scene);

		// System
		void Initialize();
		void Render();
		void Destroy();

	private:
		static NullScene							_nullScene;
		IScene*										_scene;

		std::chrono::system_clock::time_point		_prevTime;
		double										_lag;
	};
}