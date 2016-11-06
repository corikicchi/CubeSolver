#ifndef _TIMER_H_
#define _TIMER_H_

#include "../solverthread.h"

#include <cstdint>
#include <time.h>

// タイムアウトを判定するためのクラス
class CTimer
{
public:
	// p_timeOut : time out length [ms]
    CTimer(int64_t p_timeOut) :m_timeOut(p_timeOut * (int64_t)1E6)
	{
	}
    ~CTimer()
	{
	}
	void start()
	{
		timespec_get(&ts, TIME_UTC);	// システム時間を取得
		m_startTime = ts.tv_sec * (int64_t)1E9 + ts.tv_nsec;	// ナノ秒単位のシステム時間として保存
	}
	bool isTimeOut()
	{
		timespec_get(&ts, TIME_UTC);	// システム時間を取得
		int64_t nowTime = ts.tv_sec * (int64_t)1E9 + ts.tv_nsec;	// ナノ秒単位のシステム時間として保存
		
		if (nowTime > m_startTime + m_timeOut) {
            //std::cout << "Time out." << std::endl;
            //emit SolverThread::notifySolverMessage("Time out.");
			return true;
		}
		else {
			return false;
		}
	}

private:
	struct timespec ts;
	// 内部時間はナノ秒単位
	int64_t m_timeOut;	// [ms]
	int64_t m_startTime;	// [ms]
};

#endif _TIMER_H_
