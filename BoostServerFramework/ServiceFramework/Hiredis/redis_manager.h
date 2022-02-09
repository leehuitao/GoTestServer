#include <string>
#include <stdio.h>
#include <memory>
#include <queue>
#include <mutex>
#include <vector>
#include <cstring>
#include <map>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#ifdef WIN32
#include <time.h>
#include <WinSock2.h>
#define strcasecmp _stricmp
#else
#define strcasecmp strcasecmp
#include <sys/time.h>
#endif

#ifdef WIN32
#include "hiredis_win/hiredis.h"
#else
#include "hiredis/hiredis.h"
#endif



class  RedisConn
{
private:
	/* data */
public:
	RedisConn();
	virtual ~RedisConn();
	bool Init(const std::string& redis_addr, const std::size_t& port, const std::string& pwd, int conn_timeout);
	bool RedisConnect();
	bool RedisReConnect();
	bool auth();
	redisContext* ConnectWithTimeout();
	void Release();

	bool Ping();

	redisReply* ExecuteCmd(const char* format, ...);
	bool ExecuteResonse(std::string& response, const char* format, ...);
	bool ExecuteMap(std::map<std::string, std::string>& response, const char* format, ...);
	bool ExecuteVec(std::vector<std::string>& response, const char* format, ...);
	void ParamRly(redisReply* rly, std::vector<std::string>& params);
	void ParamRlyHash(redisReply* rly, std::map<std::string, std::string>& params);



private:
	redisContext* ctx_;
	std::string     redis_ip_;
	std::uint32_t   redis_port_;
	std::uint32_t   time_out_;
	std::string     redis_pwd_;
	bool            status_;
};

typedef boost::shared_ptr<RedisConn> RedisConnPrt;
typedef std::queue<RedisConnPrt> RedisConnPool;

class RedisPool {
public:
	RedisPool();
	virtual ~RedisPool();

	bool InitPool(const std::string& redis_addr, const std::size_t& port, const std::string& pwd, int conn_timeout, std::size_t pool_size, std::size_t pool_max_size);

	RedisConnPrt GetRedisConn();
	bool CreateConnection();
	void FreeRedisConn(RedisConnPrt redis_conn);
	void ReleaseRedisConn();
	void CheckStatus();
private:
	boost::mutex mutex_;
	RedisConnPool conn_pool_;

	std::string     redis_ip_;
	std::uint32_t   redis_port_;
	std::uint32_t   time_out_;
	std::string     redis_pwd_;
	bool            conn_status_;
	std::size_t pool_size_;
	std::size_t pool_max_size_;
};

typedef boost::shared_ptr<RedisPool> RedisPoolPtr;
