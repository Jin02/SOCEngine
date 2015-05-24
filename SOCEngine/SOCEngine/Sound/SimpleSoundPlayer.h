#pragma once

#include <DShow.h>
#include "Common.h"
#include <string>

#pragma comment (lib, "strmiids.lib")

namespace Sound
{
	class SimpleSoundPlayer
	{
	private:
		IGraphBuilder*		_graphBuilder;
		IMediaControl*		_mediaControl;
		IMediaPosition*		_mediaPosition;
		IBasicAudio*		_basicAudio;

		bool				_sndLoop;

		bool				_isPlay;
		bool				_isSoundLoad;

	public:
		SimpleSoundPlayer(void);
		~SimpleSoundPlayer(void);

	public:
		static std::auto_ptr<SimpleSoundPlayer> Create(const std::string& filePath);
		bool Init(const std::string& filePath);
		void Destroy();

	public:
		bool Play(bool loopMode);
		void Pause();
		void Stop();

	public:
		REFTIME GetPosition();
		void SetPosition(REFTIME pos);
		REFTIME GetDuration();

		int GetVolume();
		void SetVolume(int vol);

		int GetBalance();
		void SetBalance(int bal);

		REFTIME GetEndTime();

		GET_SET_ACCESSOR(IsPlay, bool, _isPlay);
		GET_ACCESSOR(IsSoundLoaded, bool, _isSoundLoad);

		bool GetisSoundEnd();
	};
}