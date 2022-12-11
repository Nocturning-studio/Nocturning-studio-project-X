#include "stdafx.h"
#include "xrThreadNew.h"

void XrThreadNew::GenCycleValues(int Count)
{
	/*
	switch (ThreadsCount)
	{
	case 2:
		GenCycleValues2(Count);
		break;
	case 4:
		if (Count>3) 	GenCycleValues4(Count);
		else			GenCycleValues2(Count);
		break;
	case 8:
		if (Count>7) 		GenCycleValues8(Count);
		else
		{
			if (Count > 3) 	GenCycleValues4(Count);
			else			GenCycleValues2(Count);
		}
		break;
	case 16:
		if (Count>15) 			GenCycleValues16(Count);
		else
		{
			if (Count > 7) 		GenCycleValues8(Count);
			else
			{
				if (Count>3) 	GenCycleValues4(Count);
				else			GenCycleValues2(Count);
			}
		}
		break;
	default:
		break;
	}

	*/

	if (Count>15) 			GenCycleValues16(Count);
	else
	{
		if (Count > 7) 		GenCycleValues8(Count);
		else
		{
			if (Count>3) 	GenCycleValues4(Count);
			else			GenCycleValues2(Count);
		}
	}

}

void XrThreadNew::GenCycleValues2(int Count)
{
	ThreadsCount = 2;

	CycleValues[0] = Count / 2;
};

void XrThreadNew::GenCycleValues4(int Count)
{
	ThreadsCount = 8;

	CycleValues[0] = Count / 4;
	CycleValues[1] = Count / 2;
	CycleValues[2] = CycleValues[0] + CycleValues[1];
};

void XrThreadNew::GenCycleValues8(int Count)
{
	ThreadsCount = 8;

	CycleValues[0] = Count / 8;
	CycleValues[1] = Count / 4;
	CycleValues[2] = CycleValues[0] + CycleValues[1];
	CycleValues[3] = Count / 2;
	CycleValues[4] = CycleValues[0]	+ CycleValues[3];
	CycleValues[5] = CycleValues[0]	+ CycleValues[4];
	CycleValues[6] = CycleValues[0]	+ CycleValues[5];
};

void XrThreadNew::GenCycleValues16(int Count)
{
	ThreadsCount = 16;

	CycleValues[0]	=	Count / 16;
	CycleValues[1]	=	Count / 8;
	CycleValues[2]	=	CycleValues[0] + CycleValues[1];
	CycleValues[3]	=	Count / 4;
	CycleValues[4]	=	CycleValues[0] + CycleValues[3];
	CycleValues[5]	=	CycleValues[0] + CycleValues[4];
	CycleValues[6]	=	CycleValues[0] + CycleValues[5];
	CycleValues[7]	=	Count / 2;
	CycleValues[8]	=	CycleValues[7] + CycleValues[0];
	CycleValues[9]	=	CycleValues[7] + CycleValues[1];
	CycleValues[10]	=	CycleValues[7] + CycleValues[2];
	CycleValues[11] =	CycleValues[7] + CycleValues[3];
	CycleValues[12] =	CycleValues[7] + CycleValues[4];
	CycleValues[13] =	CycleValues[7] + CycleValues[5];
	CycleValues[14] =	CycleValues[7] + CycleValues[6];
};

XrThreadNew::XrThreadNew()
{
	ProcCores = std::thread::hardware_concurrency();
	ThreadsCount = 16;
}

int XrThreadNew::GetProcCores()		{ return ProcCores;		}
int XrThreadNew::GetThreadsCount()	{ return ThreadsCount;	}

void XrThreadNew::FillValues(int Mas[14]) 
{
	for (int i = 0; i < 15; i++) Mas[i] = CycleValues[i];
}