#include "safe.h"
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// 待处理队列
JSafeFSeq< deque<string*> > g_HandleDeque;
// 结果列表
JSafeSeq< list<string*> > g_ResList;
// 输入结束标志
JSafeBool g_bPutOver(false);
// 当前运行的工作线程数
JSafeInt g_nCurTCnt(0);
// 过滤字符串
string g_sFilter;
// 线程数
const u_int g_uTCnt = 3;

inline u_int usleep_r(u_int usec)
{
	// usleep的线程安全版本
	struct timespec rqtp;
	struct timespec rem;

	memset(&rem, 0, sizeof(rem));
	rqtp.tv_sec = usec / 1000000;
	rqtp.tv_nsec = usec % 1000000 * 1000;
	nanosleep(&rqtp, &rem);
	return rem.tv_sec * 1000000 + rem.tv_nsec / 1000;
}

// 工作线程的主函数
void* Work(void* pParam)
{
	++g_nCurTCnt;

	string* pIn = NULL;

	while(1)
	{
		while(g_HandleDeque.pop_front(pIn))
		{
			if(pIn->find(g_sFilter) != string::npos)
			{
				g_ResList.push_back(pIn);
			}
			else
			{
				delete pIn;
			}
		}
		if(g_bPutOver)
		{
			break;
		}
		else
		{
			usleep_r(1);
		}
	}

	--g_nCurTCnt;
	return NULL;
}

int main(int argc, char** argv)
{
	string sUsage = string(argv[0]) + " filter filepath";
	if(argc != 3)
	{
		cout << sUsage << endl;
		_exit(1);
	}

	g_sFilter = argv[1];
	ifstream f(argv[2]);
	if(!f)
	{
		cout << argv[2] << " can't open!" << endl;
		_exit(1);
	}

	pthread_t tI[g_uTCnt];

	for(int i = 0; i < g_uTCnt; i++)
	{
		if(0 != pthread_create(&tI[i], NULL, Work, NULL))
		{
			cout << "Congratulations!" << endl;
			_exit(1);
		}
	}

	string* pLine = new string;
	while(getline(f, *pLine))
	{
		g_HandleDeque.push_back(pLine);
		pLine = new string;
	}
	delete pLine;
	
	// 结束设置
	g_bPutOver = JSafeBool(true);

	while(1)
	{
		list<string*> l;
		if(!g_ResList.empty())
		{
			g_ResList.swap(l);
			for(list<string*>::iterator it = l.begin(); it != l.end(); )
			{
				cout << **it << endl;
				delete *it;
				it = l.erase(it);
			}
		}
		else if(g_HandleDeque.empty() && g_nCurTCnt == 0)
		{
			break;
		}
		else
		{
			usleep_r(1);
		}
	}

	return 0;
}
