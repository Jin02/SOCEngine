#include "BaseScene.h"

BaseScene::BaseScene()
{
	state = STATE_INIT;
}

void BaseScene::NextState()
{
	state = (STATE)(((int)state + 1) % (int)STATE_NUM);
}

BaseScene::STATE BaseScene::GetState()
{
	return state;
}