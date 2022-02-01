#include "redis_manager.h"
RedisManager* RedisManager::instance_ = nullptr;

void RedisManager::initRedis(std::string hostname = "127.0.0.1", int port = 6379, int poolsize = 100) {

}