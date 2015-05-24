#include "SimpleSoundPlayer.h"

using namespace Sound;

SimpleSoundPlayer::SimpleSoundPlayer(void)
{
	_graphBuilder = NULL;
	_mediaControl = NULL; _mediaPosition = NULL;
	_basicAudio = NULL;

	_sndLoop		= false;
	_isPlay		= false;
	_isSoundLoad	= false;

	CoInitialize(NULL);
}

SimpleSoundPlayer::~SimpleSoundPlayer(void)
{
	Destroy();
	CoUninitialize();
}

std::auto_ptr<SimpleSoundPlayer> SimpleSoundPlayer::Create(const std::string& filePath)
{
	std::auto_ptr<SimpleSoundPlayer> soundPlayer(new SimpleSoundPlayer);
	soundPlayer->Init(filePath);
	return soundPlayer;
}

bool SimpleSoundPlayer::Init(const std::string& filePath)
{
	Destroy();

	if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_graphBuilder)))
		return false;

	std::wstring unicode(filePath.begin(), filePath.end());
	if(FAILED(_graphBuilder->RenderFile(unicode.c_str(), NULL)))
	{ Destroy(); return false; }

	if(FAILED(_graphBuilder->QueryInterface(IID_IMediaControl, (void**)&_mediaControl)))
	{ Destroy(); return false; }

	if(FAILED(_graphBuilder->QueryInterface(IID_IMediaPosition, (void**)&_mediaPosition)))
	{ Destroy(); return false; }

	if(FAILED(_graphBuilder->QueryInterface(IID_IBasicAudio, (void**)&_basicAudio)))
	{ Destroy(); return false; }

	_isSoundLoad = true;

	return true;
}

void SimpleSoundPlayer::Destroy()
{
	_isSoundLoad = false;

	SAFE_RELEASE(_basicAudio);
	SAFE_RELEASE(_mediaPosition);
	SAFE_RELEASE(_mediaControl);
	SAFE_RELEASE(_graphBuilder);
}

bool SimpleSoundPlayer::Play(bool loopMode)
{
	if( _isSoundLoad == false )
		return false;

	_mediaControl->Run();

	return (_isPlay = true);
}

void SimpleSoundPlayer::Pause()
{
	_mediaControl->Pause();
}

void SimpleSoundPlayer::Stop()
{
	if( _isSoundLoad == false )
		return;

	_mediaControl->StopWhenReady();
	_mediaPosition->put_CurrentPosition(0);
	_isPlay = false;
}

REFTIME SimpleSoundPlayer::GetPosition()
{
	REFTIME temp = 0; _mediaPosition->get_CurrentPosition(&temp);
	return temp;
}

void SimpleSoundPlayer::SetPosition(REFTIME pos)
{
	if(_mediaPosition==NULL)
		return;

	_mediaPosition->put_CurrentPosition(pos);
}

REFTIME SimpleSoundPlayer::GetDuration()
{
	REFTIME temp = 0; _mediaPosition->get_Duration(&temp);
	unsigned ms = (unsigned)(temp / 1000000);
	return ms;
}

REFTIME SimpleSoundPlayer::GetEndTime()
{
	REFTIME temp = 0; _mediaPosition->get_StopTime(&temp);
	return temp;
}

int SimpleSoundPlayer::GetVolume()
{
	LONG temp = 0; _basicAudio->get_Volume(&temp);
	return temp;
}

void SimpleSoundPlayer::SetVolume(int vol)
{
	_basicAudio->put_Volume(vol);
}

int SimpleSoundPlayer::GetBalance()
{
	LONG temp = 0; _basicAudio->get_Balance(&temp);
	return temp;
}

void SimpleSoundPlayer::SetBalance(int bal)
{
	_basicAudio->put_Balance(bal);
}

bool SimpleSoundPlayer::GetisSoundEnd()
{
	if( this->GetEndTime() == this->GetPosition() )
	{
		_isPlay = false;
		return true;
	}

	return false;
}