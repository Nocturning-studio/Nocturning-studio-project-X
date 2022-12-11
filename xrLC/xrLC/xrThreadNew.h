#pragma once

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <thread>
#include <mutex>
//std::mutex mtx;

	
class XrThreadNew
{
private:
	int ProcCores;
	int ThreadsCount;	
	int CycleValues[15];
private:
	void GenCycleValues2  (int Count);
	void GenCycleValues4  (int Count);
	void GenCycleValues8  (int Count);
	void GenCycleValues16 (int Count);
public:
	XrThreadNew();
	void GenCycleValues(int Count);
	int  GetProcCores();
	int	 GetThreadsCount();
	void FillValues(int mas[15]);
};
