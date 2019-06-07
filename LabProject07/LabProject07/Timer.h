#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

class GameTimer{
private:

	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;//마지막 프레임 이후 지나간 시간
	__int64 m_nCurrentTime;//현재시간
	__int64 m_nLastTime;//마지막 프레임 시간
	__int64 m_nPerformanceFrequency;//컴퓨터의 performance Frequency
	float m_fFrameTime[MAX_SAMPLE_COUNT]; //프레임 시간을 누적하기 위한 배열
	ULONG m_nSampleCount;//누적된 프레임 횟수
	unsigned long m_nCurrentFrameRate; //현재의 프레임 레이트
	unsigned long m_nFramesPerSecond; //초당 프레임 수
	float m_fFPSTimeElapsed;//프레임 레이트 계산 소요 시간


public:

	GameTimer();
	virtual ~GameTimer();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();
};

// 프레임 레이트(Frame Rate)의 계산은 일정한 횟수 동안 각 프레임을 렌더링하는 시간을 
// 평균하여 계산한다. 이 횟수를 나타내는 상수를 정의한다. 

