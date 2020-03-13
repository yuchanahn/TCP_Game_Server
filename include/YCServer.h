#pragma once
#include <vector>
#include <map>
#include <stack>
#include <mutex>
#include <queue>
#include <functional>
#include "PROP.h"
#include <WinSock2.h>
#include <unordered_map>
#include <thread>
#include "YCStack.h"
#include "YCMempool.h"
#include "IBehavior.h"
#include "YCPacket.h"

class ThreadData
{
	int strandID;
	std::vector<std::queue<std::function<void()>>>& CurrentJobs;
	std::vector<std::queue<std::function<void()>>>& FutureJobs;
	std::vector<bool>& IsCurrentJob;
public:
	ThreadData(std::vector<std::queue<std::function<void()>>>& c_, std::vector<std::queue<std::function<void()>>>& f_, std::vector<bool>& cj_) :
		CurrentJobs(c_), FutureJobs(f_), IsCurrentJob(cj_), strandID(-1) {}

	bool GetNewStrand();
	int GetCurrentStrandID();

	PROP_G(bool, IsTasking,
		{
			if (strandID != -1)
			{
				if (CurrentJobs[strandID].empty())
				{
					IsCurrentJob[strandID] = false;
					strandID = -1;
				}
			}
			return strandID != -1;
		});
	PROP_G(std::queue<std::function<void()>>*, Strand, { return  &CurrentJobs[strandID]; });
};
class JobManager
{

private:
	std::mutex mt;
	std::condition_variable c;
private:
	std::map<std::thread::id, ThreadData*> mThreadDataMap;
	std::vector<bool> isCurrentJob;
	std::vector<std::queue<std::function<void()>>> mCurrentJobs;
	std::vector<std::queue<std::function<void()>>> mFutureJobs;
	bool Stop;

	void Init();


public:
	JobManager() : Stop(false) { Init(); }

	void StopThread();
	void add_Job(int id, std::function<void()> f);
	void add_Job_Late(int id, std::function<void()> f);
	int create_Strand_ID();
	void clear(int m_id);
	void run();
};
class Strand
{
	JobManager& jm;

	void Init();
public:
	int id;
	Strand(JobManager& jm_) : jm(jm_)
	{
		Init();
	}
	void Add(std::function<void()> job);
	void AddLate(std::function<void()> job);
};

class ClientHandle
{
public:
	SOCKET			mSock;
	SOCKADDR_IN		_addr;
};
class IOData
{
public:
	OVERLAPPED		_overlap;
	WSABUF			_wsabuf;
	char			_buffer[1024];

	enum eIOTYPE
	{
		In,
		Out
	};
	eIOTYPE mType;
};

struct YCSession_t
{
	int id;
	SOCKET socket_id;
	Strand* job;
	ClientHandle* client_handle;
	class YCUser* user;
};

// ----------------------------------------------------------------------------
//		## Server ##
// ----------------------------------------------------------------------------
// 기본적으로 상속받아서 사용하면 됨. 상속받은 뒤 세션과, 업데이트 로직을
// 커스텀 하여 사용한다.
// ----------------------------------------------------------------------------
class YCServer
{
private:
	int PORT;

	WSAData wsaData;
	HANDLE CP;
	SOCKET mServerSock;
	SOCKADDR_IN	sAddr;

	std::vector<std::thread> IOCP_Thread;
	std::vector<std::thread> workerThreads;

	YCStack<int> mFree_Sessions_ID_Stack;
	std::vector<YCSession_t> mSessions;
	std::unordered_map<SOCKET, int> Sessions_ID_Mapping;

	std::function<void(int)> connect_ev;
	std::function<void(int)> disconnect_ev;
	std::function<void()> loop;
private:
	void Init();

public:
	JobManager* mJobManager;
	Strand* Job;

public:
	YCServer(int port,
		std::function<void(int)> c,
		std::function<void(int)> d,
		std::function<void()> l) :
		PORT(port),
		connect_ev(c),
		disconnect_ev(d),
		loop(l)
	{ Init(); }
	~YCServer();


	void Srv_Start();
	void Update();

	Strand* get_sync(int id)
	{
		if (mSessions.size() <= id) assert("클라이언트 ID가 잘못됨!");
		return mSessions[id].job;
	}
	Strand* get_server_sync()
	{
		return Job;
	}

	template <typename T>
	void Send(int id, T* p)
	{
		if (mSessions.size() <= id) assert("클라이언트 ID가 잘못됨!");

		auto IO = new IOData;
		memset(&IO->_overlap, 0, sizeof(OVERLAPPED));
		IO->mType = IO->Out;

		auto len = ((packet_t<T>*) p)->pack((unsigned char*)IO->_buffer);
		IO->_wsabuf.buf = IO->_buffer;
		IO->_wsabuf.len = len;
		auto socket = mSessions[id].socket_id;

		mSessions[id].job->Add([IO, socket] {
			WSASend(socket, &IO->_wsabuf, 1, NULL, 0, &IO->_overlap, NULL);
		});
	}

	bool Srv_Startup();
	bool Srv_bind();
	bool Srv_listen();
	void Srv_RunWorker();
	void Srv_Accept();
	void Client_Disconnect(SOCKET);
};