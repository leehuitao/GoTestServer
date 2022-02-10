#pragma once
#include "connection_pool_mongodb.hpp"
class  MongoDBManager
{
public:
	static MongoDBManager& instance();
	/*MongoDB数据库初始化*/
	bool mongodb_init(const char* url);
	/*清理数据库*/
	bool mongodb_cleanup();
	bool create_index(const char* databasename, const char* collname, const char* keyname, const char* indextype);
	bool select_coll(const char* dbname, const char* collname, const char* id_, std::vector<std::map<std::string, std::string>>& msg_data);

	bool insert_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data);
	bool insert_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data, const std::vector<std::string> params, const std::map<std::string, std::string>& props);
	bool delete_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data);
	bool update_coll(const char* dbname, const char* collname,
		const std::map<std::string, std::string>& map_qurey,
		const std::string& update_key, const std::string& update_value);
	bool select_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data, std::vector<std::map<std::string, std::string>>& msg_data);
	bool select_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data, std::vector<std::map<std::string, std::string>>& msg_data,
		std::vector < std::vector<std::string>>& params, const std::string& platform);

	bool  find_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data);

	bool InsertNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& map_data);
	bool PushNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::map<std::string, std::map<std::string, std::string>>& map_data);
	bool DelNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, std::map<std::string, std::map<std::string, std::string>>& map_data);
	bool SelectNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::vector< std::string> field_map, std::vector<std::map<std::string, std::string>>& map_data);
private:
	void* m_mongodb_ptr = nullptr;
};