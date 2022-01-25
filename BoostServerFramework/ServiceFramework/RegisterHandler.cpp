#include "RegisterHandler.h"

RegisterHandler* RegisterHandler::instance_ = nullptr;
bool RegisterHandler::InitHandler(unsigned short cmd, RequestHandler handler)
{
	return MsgHandlerRepository<RequestHandler>::instance().registerHandler(cmd, handler);
}