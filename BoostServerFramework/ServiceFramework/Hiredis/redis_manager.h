
#pragma once
#include "connection_pool_redis.hpp"

class  RedisManager : public boost::enable_shared_from_this<RedisManager>
{
public:
	static RedisManager& instance();
	
	bool InitPool(const std::string& redis_addr, const std::size_t& port, const std::string& pwd, int conn_timeout, std::size_t pool_size, std::size_t pool_max_size);

	void ReleasePool();
	void CheckStatus();
	bool SetKeyExpire(std::string& key, int time);
	bool DelKey(const std::string& key);
	bool KeyExists(const std::string& key);
	// String
	bool SetString(const std::string& key, const std::string& value);
	std::string GetString(const std::string& key);
	bool Set(const std::string& key, const std::string& field, const std::string& value);
	bool Get(const std::string& key, const std::string& field, std::string& reponse);

	// Hash
	bool HSet(const std::string& key, const std::string& commond);
	bool HSet(const std::string& key, const std::string& field, const std::string& value);
	bool HGet(const std::string& key, const std::string& field, std::string& response);
	bool HGetAll(const std::string& key, std::map<std::string, std::string>& response);
	bool HDelField(const std::string& key, const std::string& field);
	bool HFieldCount(const std::string& key, std::string& count);
	bool flushal();

	bool getAllKeys(const std::string& key, std::vector<std::string>& response);

private:

	std::string     redis_ip_;
	std::uint32_t   redis_port_;
	std::uint32_t   time_out_;
	std::string     redis_pwd_;

	RedisPoolPtr pool_;
private:
	RedisManager();
	virtual ~RedisManager();
};
