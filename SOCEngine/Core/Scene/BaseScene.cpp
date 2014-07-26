#include "BaseScene.h"

BaseScene::BaseScene()
{
	_state = State::Init;
}

BaseScene::~BaseScene()
{
}

void BaseScene::NextState()
{
	_state = (State)(((int)_state + 1) % (int)State::Num);
}

BaseScene::State BaseScene::GetState()
{
	return _state;
}