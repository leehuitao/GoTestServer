#include "dbio_redis.h"
#include <numeric>
#include <boost/lexical_cast.hpp>
DBIORedis::DBIORedis() :pool_(new RedisPool())
{
	redis_ip_ = "127.0.0.1";
	redis_port_ = 6379;
	time_out_ = 1;
}
DBIORedis::~DBIORedis()
{
}


bool DBIORedis::InitPool(const std::string& redis_addr, const std::size_t& port, const std::string& pwd, int conn_timeout, std::size_t pool_size, std::size_t pool_max_size)
{
	bool ret = pool_->InitPool(redis_addr, port, pwd, conn_timeout, pool_size, pool_max_size);

	return ret;
}

bool DBIORedis::flushal()
{
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "flushall";
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::getAllKeys(const std::string& key, std::vector<std::string>& response)
{
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "keys " + key + '\0';
	bool ret = conn->ExecuteVec(response, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

void DBIORedis::ReleasePool()
{
	pool_->ReleaseRedisConn();
}

void DBIORedis::CheckStatus()
{
	pool_->CheckStatus();
}

bool DBIORedis::SetKeyExpire(std::string& key, int time) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "EXPIRE " + key + " " + boost::lexical_cast<std::string>(time);
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::DelKey(const std::string& key) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "DEL " + key + "\0";
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::KeyExists(const std::string& key) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "EXISTS " + key + "\0";
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	if (reponse == "1") {
		return true;
	}
	else {
		return false;
	}
}

bool DBIORedis::SetString(const std::string& key, const std::string& value) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "set " + key + " " + value;
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

std::string DBIORedis::GetString(const std::string& key) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return "";

	std::string commond = "get " + key;
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	//bool ret = conn->ExecuteResonse(reponse, "get %s", key.c_str());
	pool_->FreeRedisConn(conn);
	return reponse;
}


bool DBIORedis::Set(const std::string& key, const std::string& field, const std::string& value)
{
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "set " + key + " " + field + " " + value;
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	//bool ret = conn->ExecuteResonse(reponse, "set %s %s %s", key.c_str(), field.c_str(), value.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}



bool DBIORedis::Get(const std::string& key, const std::string& field, std::string& reponse)
{
	//std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "get " + key;
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	//bool ret = conn->ExecuteResonse(reponse, "get %s", key.c_str());
	pool_->FreeRedisConn(conn);
	return ret;

}
bool DBIORedis::HSet(const std::string& key, const std::string& commond)
{
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond_ = "hmset " + key + " " + commond;
	bool ret = conn->ExecuteResonse(reponse, commond_.c_str());
	//bool ret = conn->ExecuteResonse(reponse, "hset %s %s %s", key.c_str(), field.c_str(), value.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}
bool DBIORedis::HSet(const std::string& key, const std::string& field, const std::string& value)
{
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "hset " + key + " " + field + " " + value;
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	//bool ret = conn->ExecuteResonse(reponse, "hset %s %s %s", key.c_str(), field.c_str(), value.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::HGet(const std::string& key, const std::string& field, std::string& response)
{
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "hget " + key + " " + field;
	bool ret = conn->ExecuteResonse(response, commond.c_str());
	/*bool ret = conn->ExecuteResonse(reponse, "hget %s %s", key.c_str(), field.c_str());*/
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::HGetAll(const std::string& key, std::map<std::string, std::string>& response)
{
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "hgetall " + key + '\0';
	bool ret = conn->ExecuteMap(response, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::HDelField(const std::string& key, const std::string& field) {
	std::string reponse;
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "HDEL  " + key + " " + field + "\0";
	bool ret = conn->ExecuteResonse(reponse, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}

bool DBIORedis::HFieldCount(const std::string& key, std::string& count) {
	RedisConnPrt conn = pool_->GetRedisConn();
	if (!conn.get()) return false;

	std::string commond = "HLEN " + key + " \0";
	bool ret = conn->ExecuteResonse(count, commond.c_str());
	pool_->FreeRedisConn(conn);
	return ret;
}
