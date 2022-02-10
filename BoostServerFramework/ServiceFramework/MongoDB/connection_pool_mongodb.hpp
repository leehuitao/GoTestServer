
#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <bson/bson.h>
#include <mongoc/mongoc.h>

using namespace std;

class  MongoDBConnectionPool
{
public:
	MongoDBConnectionPool();
	~MongoDBConnectionPool();

public:
	void mongodb_init(const char* url);

	bool create_client_pool();

	bool destory_client_pool();

	void free_bjson(const bson_t *command);
	//mongodb��һϵ�е����е�����������������������Ϣ�����
	bool  insert_coll(const char*databasename, const char* collname, const bson_t *doc);
	bool  delete_coll(const char*databasename, const char* collname, const bson_t *doc);
	bool  update_coll(const char*databasename, const char* collname, const bson_t* _id, const bson_t *doc);
	bool  select_coll(const char*databasename, const char* collname, bson_t* query, std::vector<std::map<std::string, std::string>>& doc);
	bool  select_coll(const char* databasename, const char* collname, bson_t* query, std::vector<std::map<std::string, std::string>>& doc, std::vector < std::vector<std::string>>& params);
	bool  select_coll_by_fields(const char*databasename, const char* collname, bson_t* query, bson_t* fields, std::vector<std::map<std::string, std::string>>& msg_data);
	bool  find_coll(const char*databasename, const char* collname, bson_t* query);
	bool batch_insert_coll(const char*databasename, const char* collname, const  bson_t **docs, size_t n_documents);
	bool create_index(const char* databasename, const char* collname,const char* keyname,const char* indextype);
	void GetRecord(const bson_t *doc, std::vector<std::map<std::string, std::string>>& msg_data);
	void GetVecRecord(const bson_t *doc, std::vector<std::map<std::string, std::string>>& msg_data);
	void GetVecAndDocRecord(const bson_t* doc, std::vector<std::map<std::string, std::string>>& msg_data, std::vector < std::vector<std::string>>& param);
private:
	int m_ulthreads = 10;
	void *  m_pool = nullptr;
	void *  m_url = nullptr;
	std::string  m_strurl;
};

