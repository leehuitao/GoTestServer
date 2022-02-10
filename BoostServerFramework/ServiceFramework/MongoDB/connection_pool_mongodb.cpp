/*
 * dbio_mysql.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: root
 */
#include "connection_pool_mongodb.hpp"
#include <numeric>
MongoDBConnectionPool::MongoDBConnectionPool()
{
	mongoc_init();
}


MongoDBConnectionPool::~MongoDBConnectionPool()
{
}

void MongoDBConnectionPool::mongodb_init(const char* url)
{
	m_strurl = url;
	create_client_pool();
}

bool MongoDBConnectionPool::create_client_pool()
{
	mongoc_client_pool_t * client_pool = nullptr;
	mongoc_uri_t         *uri = nullptr;
	uri = mongoc_uri_new(m_strurl.c_str());//urlָ��ip��ַ��û��ָ��
	client_pool = mongoc_client_pool_new(uri);
	m_pool = static_cast<void*>(client_pool);
	m_url = static_cast<void*>(uri);
	mongoc_client_pool_max_size(static_cast<mongoc_client_pool_t*>(m_pool), 50);
	return true;
}
bool MongoDBConnectionPool::create_index(const char* databasename, const char* collname, const char* keyname, const char* indextype)
{
	mongoc_client_pool_t* pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t* client;
	bson_error_t error;
	client = mongoc_client_pool_pop(pool);
	if (client == nullptr) {
		return false;
	}
	mongoc_collection_t* collection = mongoc_client_get_collection(client, databasename, collname);
	bson_t keys;
	bson_init(&keys);
	BSON_APPEND_UTF8(&keys, keyname, indextype);
	bool r = mongoc_collection_create_index(collection, &keys, NULL, &error);
	if (!r)
		std::cout << "mongoc_collection_create_index error =" << &error;
	bson_destroy(&keys);
	mongoc_collection_destroy(collection);
	mongoc_client_pool_push(pool, client);
	return r;
}
bool MongoDBConnectionPool::destory_client_pool()
{
	if (m_url == nullptr || m_pool == nullptr)
	{
		return false;
	}
	mongoc_client_pool_destroy(static_cast<mongoc_client_pool_t*>(m_pool));
	mongoc_uri_destroy(static_cast<mongoc_uri_t*>(m_url));
	mongoc_cleanup();
	return true;
}

void MongoDBConnectionPool::free_bjson(const bson_t *command)
{
	char *commandStr = ::bson_as_json(command, NULL);
	bson_free(commandStr);
}

bool MongoDBConnectionPool::insert_coll(const char*databasename, const char* collname, const bson_t *doc)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;
	client = mongoc_client_pool_pop(pool);//�����ӳ��л�ȡ���Ӷ��� 
	if (client == nullptr) {
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);

	// ��bson�ĵ����뵽����
	if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
		mongoc_collection_destroy(collection);
		mongoc_client_pool_push(pool, client);
		return false;
	}
	mongoc_collection_destroy(collection);
	mongoc_client_pool_push(pool, client);
	return true;
}


bool MongoDBConnectionPool::delete_coll(const char*databasename, const char* collname, const bson_t *doc)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;
	client = mongoc_client_pool_pop(pool);
	if (client == nullptr) {
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);
	if (!mongoc_collection_remove(collection, MONGOC_REMOVE_SINGLE_REMOVE, doc, NULL, &error)) {
		mongoc_client_pool_push(pool, client);
		return false;
	}
	mongoc_client_pool_push(pool, client);
	return true;
}


bool MongoDBConnectionPool::update_coll(const char*databasename, const char* collname, const bson_t* _id, const bson_t *doc)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;
	client = mongoc_client_pool_pop(pool);
	if (client == nullptr) {
	
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);
	if (!mongoc_collection_update_one(collection, _id, doc, NULL, NULL, &error)) {
		mongoc_client_pool_push(pool, client);
		return false;
	}
	mongoc_client_pool_push(pool, client);
	mongoc_collection_destroy(collection);
	return true;
}

bool MongoDBConnectionPool::select_coll(const char*databasename, const char* collname, bson_t* query, std::vector<std::map<std::string, std::string>>& msg_data)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;

	client = mongoc_client_pool_pop(pool);//�����ӳ��л�ȡ���Ӷ��� 
	if (client == nullptr) {
		//H3C_CLOUD_MONGODB_LOG1(H3C_LOG_ERROR, "fail get client ptr url:[%s] database name:[%s]", databasename);
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);

	mongoc_cursor_t *cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	const bson_t *doc;
	while (!mongoc_cursor_error(cursor, &error) &&
		mongoc_cursor_more(cursor)) {
		if (mongoc_cursor_next(cursor, &doc))
		{
			GetRecord(doc, msg_data);
		}
	}

	bool result = true;
	if (mongoc_cursor_error(cursor, &error))
		result = false;

	if (doc) free_bjson(doc);
	mongoc_collection_destroy(collection);
	mongoc_cursor_destroy(cursor);
	mongoc_client_pool_push(pool, client);
	return result;
}
bool  MongoDBConnectionPool::select_coll(const char* databasename, const char* collname, bson_t* query, std::vector<std::map<std::string, std::string>>& msg_data, std::vector < std::vector<std::string>>& params)
{
	mongoc_client_pool_t* pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t* client;
	bson_error_t error;

	client = mongoc_client_pool_pop(pool);//�����ӳ��л�ȡ���Ӷ��� 
	if (client == nullptr) {
		//H3C_CLOUD_MONGODB_LOG1(H3C_LOG_ERROR, "fail get client ptr url:[%s] database name:[%s]", databasename);
		return false;
	}

	mongoc_collection_t* collection = mongoc_client_get_collection(client, databasename, collname);

	mongoc_cursor_t* cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	const bson_t* doc;
	while (!mongoc_cursor_error(cursor, &error) &&
		mongoc_cursor_more(cursor)) {
		if (mongoc_cursor_next(cursor, &doc))
		{
			GetVecAndDocRecord(doc, msg_data, params);
		}
	}

	bool result = true;
	if (mongoc_cursor_error(cursor, &error))
		result = false;

	if (doc) free_bjson(doc);
	mongoc_collection_destroy(collection);
	mongoc_cursor_destroy(cursor);
	mongoc_client_pool_push(pool, client);
	return result;
}
bool MongoDBConnectionPool::select_coll_by_fields(const char*databasename, const char* collname, bson_t* query, bson_t* fields, std::vector<std::map<std::string, std::string>>& msg_data)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;

	client = mongoc_client_pool_pop(pool);
	if (client == nullptr) {
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);

	mongoc_cursor_t *cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, fields, NULL);

	const bson_t *doc;
	while (!mongoc_cursor_error(cursor, &error) &&
		mongoc_cursor_more(cursor)) {
		if (mongoc_cursor_next(cursor, &doc))
		{
			GetVecRecord(doc, msg_data);
		}
	}

	bool result = true;
	if (mongoc_cursor_error(cursor, &error))
		result = false;

	if (doc) free_bjson(doc);
	mongoc_collection_destroy(collection);
	mongoc_cursor_destroy(cursor);
	mongoc_client_pool_push(pool, client);
	return result;
}

bool  MongoDBConnectionPool::find_coll(const char*databasename, const char* collname, bson_t* query)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;

	client = mongoc_client_pool_pop(pool);//�����ӳ��л�ȡ���Ӷ��� 
	if (client == nullptr) {
		//H3C_CLOUD_MONGODB_LOG1(H3C_LOG_ERROR, "fail get client ptr url:[%s] database name:[%s]", databasename);
		return false;
	}

	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);

	mongoc_cursor_t *cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

	const bson_t *doc;
	while (!mongoc_cursor_error(cursor, &error) &&
		mongoc_cursor_more(cursor)) {
		if (mongoc_cursor_next(cursor, &doc))
		{
			free_bjson(doc);
			mongoc_collection_destroy(collection);
			mongoc_cursor_destroy(cursor);
			mongoc_client_pool_push(pool, client);
			return true;
		}
	}
	if (doc)
		free_bjson(doc);
	
	mongoc_collection_destroy(collection);
	mongoc_cursor_destroy(cursor);
	mongoc_client_pool_push(pool, client);
	return false;
}


bool MongoDBConnectionPool::batch_insert_coll(const char*databasename, const char* collname, const bson_t **docs, size_t n_documents)
{
	mongoc_client_pool_t *pool = static_cast<mongoc_client_pool_t*>(m_pool);
	mongoc_client_t      *client;
	bson_error_t error;
	client = mongoc_client_pool_pop(pool);//�����ӳ��л�ȡ���Ӷ��� 
	if (client == nullptr) {
		//H3C_CLOUD_MONGODB_LOG1(H3C_LOG_ERROR, "fail get client ptr url:[%s] database name:[%s]", databasename);
		return false;
	}
	bson_t *reply = new bson_t;
	mongoc_collection_t *collection = mongoc_client_get_collection(client, databasename, collname);
	// ��bson�ĵ����뵽����
	if (!mongoc_collection_insert_many(collection, docs, n_documents, NULL, NULL, &error)) {
		mongoc_collection_destroy(collection);
		free_bjson(*docs);
		mongoc_client_pool_push(pool, client);
		return false;
	}
	// �ͷ���Դ
	mongoc_collection_destroy(collection);
	free_bjson(*docs);
	mongoc_client_pool_push(pool, client);
	return true;
}

void MongoDBConnectionPool::GetRecord(const bson_t *doc, std::vector<std::map<std::string, std::string>>& msg_data)
{
	bson_iter_t iter;
	bson_iter_init(&iter, doc);
	uint32_t len = 12;
	const uint8_t* uintlen = nullptr;

	std::string key, value;
	bson_type_t type;
	std::map < std::string, std::string > tmp_data;
	tmp_data.clear();
	while (bson_iter_next(&iter))
	{
		key.clear(); value.clear();
		key = bson_iter_key(&iter);
		type = bson_iter_type(&iter);
		switch (type)
		{
		case BSON_TYPE_UTF8:
			value = bson_iter_utf8(&iter, &len);
			tmp_data[key] = value;
			break;
		case BSON_TYPE_ARRAY:
			bson_iter_array(&iter, &len, &uintlen);
			break;
		default:
			break;
		}

	}
	if (tmp_data.size() < 1) return;
	msg_data.push_back(tmp_data);
}

void MongoDBConnectionPool::GetVecRecord(const bson_t *doc, std::vector<std::map<std::string, std::string>>& msg_data)
{
	bson_iter_t iter;
	bson_iter_init(&iter, doc);
	uint32_t len = 12;
	const uint8_t* uintlen = nullptr;
	bson_iter_t child;
	bson_iter_t child1;

	std::string key, value, k1, value1;
	bson_type_t type;
	std::map < std::string, std::string > tmp_data;
	while (bson_iter_next(&iter))
	{
		key = bson_iter_key(&iter);
		type = bson_iter_type(&iter);
		switch (type)
		{
		case BSON_TYPE_UTF8:
			value = bson_iter_utf8(&iter, &len);
			tmp_data[key] = value;
			break;
		case BSON_TYPE_ARRAY:
			tmp_data.clear();
			bson_iter_array(&iter, &len, &uintlen);
			bson_iter_recurse(&iter, &child);
			while (bson_iter_next(&child)) {
				bson_iter_recurse(&child, &child1);
				while (bson_iter_next(&child1)) {
					k1 = bson_iter_key(&child1);
					type = bson_iter_type(&child1);
					if (BSON_TYPE_UTF8 == type)
						value = bson_iter_utf8(&child1, &len);
					tmp_data[k1] = value;
				}
				msg_data.push_back(tmp_data);
			}
			break;
		default:
			break;
		}

	}
	if (tmp_data.size() < 1) return;
	//msg_data.push_back(tmp_data);
}
void MongoDBConnectionPool::GetVecAndDocRecord(const bson_t* doc, std::vector<std::map<std::string, std::string>>& msg_data, std::vector < std::vector<std::string>>& param)
{
	bson_iter_t iter;
	bson_iter_init(&iter, doc);
	uint32_t len = 12;
	const uint8_t* uintlen = nullptr;
	bson_iter_t child;
	bson_iter_t child1;
	std::string key, value;
	bson_type_t type;
	std::map < std::string, std::string > tmp_data;
	std::vector<std::string> tmp_param;
	while (bson_iter_next(&iter))
	{
		key = bson_iter_key(&iter);
		type = bson_iter_type(&iter);
		switch (type)
		{
		case BSON_TYPE_UTF8:
			value = bson_iter_utf8(&iter, &len);
			tmp_data.insert(make_pair(key, value));
			break;
		case BSON_TYPE_ARRAY:
			bson_iter_array(&iter, &len, &uintlen);
			bson_iter_recurse(&iter, &child);
			while (bson_iter_next(&child)) {
				value = bson_iter_utf8(&child, &len);
				tmp_param.push_back(value);
			}
			//bson_iter_array(&iter, &len, &uintlen);
			break;
		case BSON_TYPE_DOCUMENT:
			bson_iter_recurse(&iter, &child1);
			while (bson_iter_next(&child1)) {
				key = bson_iter_key(&child1);
				value = bson_iter_utf8(&child1, &len);
				tmp_data.insert(make_pair(key, value));
			}
			break;
		default:
			break;
		
		}

	}
	if (tmp_data.size() < 1) return;
	msg_data.push_back(tmp_data);
	param.push_back(tmp_param);
}