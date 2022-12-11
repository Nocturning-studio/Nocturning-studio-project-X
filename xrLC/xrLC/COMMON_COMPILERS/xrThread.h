#pragma once
#include <thread>

class ENGINE_API CThread
{
	static void			startup(void* P);
public:
	volatile u32		thID;
	volatile float		thProgress;
	volatile BOOL		thCompleted;
	volatile BOOL		thMessages;
	volatile BOOL		thMonitor;
	volatile float		thPerformance;
	volatile BOOL		thDestroyOnComplete;

	CThread				(u32 _ID)	
	{
		thID				= _ID;
		thProgress			= 0;
		thCompleted			= FALSE;
		thMessages			= TRUE;
		thMonitor			= FALSE;
		thDestroyOnComplete	= TRUE;
	}
	virtual				~CThread(){}
	void				Start	()
	{
		thread_spawn	(startup,"worker-thread",1024*1024,this);
	}
	virtual		void	Execute	()	= 0;
};

class ENGINE_API CThreadManager
{
	xr_vector<CThread*>	threads;
public:
	void				start	(CThread*	T);
	void				wait	(u32		sleep_time=1000);
};

IC void Gen_thread_intervals(u32 max_threads,u32 num_items, u32 &threads, u32 &stride)
{
	R_ASSERT(num_items);

	if (max_threads <= num_items)
	{
		stride = num_items / max_threads;
		threads = max_threads;

		Msg("max threads: %i num items: %i threads: %i stride %i",max_threads,num_items, threads, stride);
		return;
	}
	threads = num_items;
	stride = 1;
	Msg("max threads: %i num items: %i threads: %i stride %i", max_threads, num_items, threads, stride);

}