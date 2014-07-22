#include "BaseScene.h"

BaseScene::BaseScene()
{
	_state = STATE_INIT;
}

BaseScene::~BaseScene()
{
}

void BaseScene::NextState()
{
	_state = (STATE)(((int)_state + 1) % (int)STATE_NUM);
}

BaseScene::STATE BaseScene::GetState()
{
	return _state;
}