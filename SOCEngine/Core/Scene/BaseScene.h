#pragma once

namespace Core
{

	class BaseScene
	{
	public:
		enum STATE{ STATE_INIT = 0, STATE_LOOP, STATE_END, STATE_NUM };

	private:
		STATE state;

	public:
		BaseScene();
		void NextState();
		STATE GetState();

	public:
		virtual void Initialize() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;
		virtual void Destroy() = 0;
	};

}