#pragma once

class BaseScene
{
public:
	enum State{ Init = 0, Loop, End, Num };

private:
	State _state;

public:
	BaseScene();
	~BaseScene();

public:
	void NextState();
	State GetState();

public:
	virtual void Initialize() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;
};

