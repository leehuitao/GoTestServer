#pragma once
#include "NetworkManager/message_protocol.h"

class TestMethod 
{
public:
	virtual void HandleMessage(RequestBufferPtr msg);

};

void TestMethod::HandleMessage(RequestBufferPtr msg)
{
	if (!msg)
	{
		return;
	}

	std::cout << __FUNCTION__ << "receive data ";
}



