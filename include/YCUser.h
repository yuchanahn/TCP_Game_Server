#pragma once
#include "YCPacket.h"


class YCUser
{
private:
protected:
public:
	int ID;
	vector<IBehavior*> mBehaviors;
	YC_Packet_ReadManager mReadManager;
	class YCSync* mSyncer;
	class YCSend* mSender;

	void In(unsigned char* buf, int size);


	YCUser(class YCSync* sy, class YCSend* se, int id) :mSyncer(sy), mSender(se), ID(id) {};
	~YCUser();
};