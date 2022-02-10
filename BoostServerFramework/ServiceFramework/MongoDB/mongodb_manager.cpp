#include "mongodb_manager.h"

#include <numeric>
DBIOMongoDB& DBIOMongoDB::instance()
{
	static DBIOMongoDB s_instance;
	return s_instance;
}

bool DBIOMongoDB::mongodb_init(const char* url)
{
	MongoDBConnectionPool* client_pool = new MongoDBConnectionPool();
	client_pool->mongodb_init(url);
	client_pool->create_client_pool();

	m_mongodb_ptr = static_cast<void*>(client_pool);
	return true;
}

bool DBIOMongoDB::mongodb_cleanup()
{
	bool ulret = false;
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return ulret; }
	return dbapi->destory_client_pool();
}
bool DBIOMongoDB::create_index(const char* databasename, const char* collname, const char* keyname, const char* indextype)
{
	bool ulret = false;
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return ulret; }
	return dbapi->create_index(databasename, collname, keyname, indextype);
}

bool DBIOMongoDB::select_coll(const char* dbname, const char* collname, const char* id_, std::vector<std::map<std::string, std::string>>& msg_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }

	//bson_t* cond = bson_new_from_json((const unsigned char*)command.c_str(), command.length(), &errorinfo);

	bson_t* query = bson_new();
	bool ret = dbapi->select_coll(dbname, collname, query, msg_data);
	bson_destroy(query);
	return ret;
}


bool DBIOMongoDB::insert_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* doc = bson_new();
	for (auto& itr : map_data)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(doc, key.c_str(), value.c_str());
	}
	bool ret = dbapi->insert_coll(dbname, collname, doc);
	bson_destroy(doc);
	return ret;
}

bool DBIOMongoDB::insert_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data, const std::vector<std::string> params, const std::map<std::string, std::string>& props)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	uint32_t index;
	char  buf[16];
	std::string key, value;
	const char* number;
	bson_t* doc = bson_new();
	bson_t paramArray;
	bson_t prop;
	for (auto mapiter = map_data.begin(); mapiter != map_data.end(); ++mapiter)
	{
		key = mapiter->first;
		value = mapiter->second;
		BSON_APPEND_UTF8(doc, key.c_str(), value.c_str());
	}
	//add paramsArray data
	BSON_APPEND_ARRAY_BEGIN(doc, "params", &paramArray);
	for (index = 0; index < params.size(); index++)
	{
		bson_uint32_to_string(index, &number, buf, sizeof buf);
		BSON_APPEND_UTF8(&paramArray, number, params[index].c_str());
	}
	bson_append_array_end(doc, &paramArray);
	//add propsdocument data
	BSON_APPEND_DOCUMENT_BEGIN(doc, "props", &prop);
	for (auto propitr = props.begin(); propitr != props.end(); ++propitr)
	{
		key = propitr->first;
		value = propitr->second;
		BSON_APPEND_UTF8(&prop, key.c_str(), value.c_str());
	}
	bson_append_document_end(doc, &prop);

	bool ret = dbapi->insert_coll(dbname, collname, doc);
	bson_destroy(doc);
	return ret;
}

bool DBIOMongoDB::delete_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* doc = bson_new();
	for (auto& itr : map_data)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(doc, key.c_str(), value.c_str());
	}
	bool ret = dbapi->delete_coll(dbname, collname, doc);
	bson_destroy(doc);
	return ret;
}


bool DBIOMongoDB::update_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_qurey,
	const std::string& update_key, const std::string& update_value)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_qurey.size() < 1) return false;

	std::string key, value;
	//bson_t* doc= bson_new();
	bson_t query;
	bson_init(&query);
	for (auto& itr : map_qurey)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(&query, key.c_str(), value.c_str());
	}
	bson_t* update = BCON_NEW("$set", "{", update_key.c_str(), update_value.c_str(), "}");

	bool ret = dbapi->update_coll(dbname, collname, &query, update);
	bson_destroy(update);
	bson_destroy(&query);
	return ret;
}

bool DBIOMongoDB::select_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data, std::vector<std::map<std::string, std::string>>& msg_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* query = bson_new();

	for (auto& itr : map_data)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(query, key.c_str(), value.c_str());
	}
	bool ret = dbapi->select_coll(dbname, collname, query, msg_data);
	bson_destroy(query);
	return ret;
}
bool DBIOMongoDB::select_coll(const char* dbname, const char* collname,
	const std::map<std::string, std::string>& map_data,
	std::vector<std::map<std::string, std::string>>& msg_data,
	std::vector < std::vector<std::string>>& params,
	const std::string& platform)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* query = bson_new();

	for (auto& itr : map_data)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(query, key.c_str(), value.c_str());
	}
	string platform_ = "0";
	BSON_APPEND_UTF8(query, platform.c_str(), platform_.c_str());
	bool ret = dbapi->select_coll(dbname, collname, query, msg_data, params);
	bson_destroy(query);
	return ret;
}

bool DBIOMongoDB::find_coll(const char* dbname, const char* collname, const std::map<std::string, std::string>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }

	std::string key, value;
	bson_t* query = bson_new();

	for (auto& itr : map_data)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(query, key.c_str(), value.c_str());
	}
	bool ret = dbapi->find_coll(dbname, collname, query);
	bson_destroy(query);
	return ret;
}

bool DBIOMongoDB::InsertNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::map<std::string, std::vector<std::map<std::string, std::string>>>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* doc = bson_new();
	bson_t child;
	bson_t child2;
	char        buf[16];
	std::size_t      keylen;
	const       char* key1;
	bson_init(&child2);


	for (auto& itr : id_map)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(doc, key.c_str(), value.c_str());
	}
	//
	int i = 0;
	for (auto& itr : map_data)
	{
		key = itr.first;
		i = 0;
		BSON_APPEND_ARRAY_BEGIN(doc, key.c_str(), &child);
		for (auto& itr_child_vec : itr.second)
		{
			keylen = bson_uint32_to_string(i, &key1, buf, sizeof buf);
			bson_append_document_begin(&child, key1, keylen, &child2);
			for (auto& itr_child_map : itr_child_vec)
			{
				key = itr_child_map.first;
				value = itr_child_map.second;
				BSON_APPEND_UTF8(&child2, key.c_str(), value.c_str());
			}
			i++;
			bson_append_document_end(&child, &child2);
		}
		bson_append_array_end(doc, &child);
	}
	bool ret = dbapi->insert_coll(dbname, collname, doc);
	bson_destroy(doc);
	return ret;
}


bool DBIOMongoDB::PushNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::map<std::string, std::map<std::string, std::string>>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* doc = bson_new();
	bson_t* query = bson_new();
	bson_t child;

	for (auto& itr : id_map)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(query, key.c_str(), value.c_str());
	}
	for (auto& itr : map_data)
	{
		key = itr.first;
		BSON_APPEND_DOCUMENT_BEGIN(doc, key.c_str(), &child);
		for (auto& itr_child_map : itr.second)
		{
			key = itr_child_map.first;
			value = itr_child_map.second;
			BSON_APPEND_UTF8(&child, key.c_str(), value.c_str());
		}
		bson_append_document_end(doc, &child);
	}
	bson_t* docs = bson_new();
	BSON_APPEND_DOCUMENT(docs, "$push", doc);
	bool ret = dbapi->update_coll(dbname, collname, query, doc);
	bson_destroy(doc);
	bson_destroy(query);
	return ret;
}

bool DBIOMongoDB::DelNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, std::map<std::string, std::map<std::string, std::string>>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }
	if (map_data.size() < 1) return false;

	std::string key, value;
	bson_t* doc = bson_new();
	bson_t* query = bson_new();
	bson_t child;


	for (auto& itr : id_map)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(query, key.c_str(), value.c_str());
	}
	for (auto& itr : map_data)
	{
		key = itr.first;
		BSON_APPEND_DOCUMENT_BEGIN(doc, key.c_str(), &child);
		for (auto& itr_child_map : itr.second)
		{
			key = itr_child_map.first;
			value = itr_child_map.second;
			BSON_APPEND_UTF8(&child, key.c_str(), value.c_str());
		}
		bson_append_document_end(doc, &child);
	}
	bson_t* docs = bson_new();
	BSON_APPEND_DOCUMENT(docs, "$pull", doc);

	bool ret = dbapi->update_coll(dbname, collname, query, doc);
	bson_destroy(doc);
	bson_destroy(query);
	return ret;
}

bool DBIOMongoDB::SelectNoticeColl(const char* dbname, const char* collname, const std::map<std::string, std::string> id_map, const std::vector< std::string> field_map, std::vector<std::map<std::string, std::string>>& map_data)
{
	MongoDBConnectionPool* dbapi = static_cast<MongoDBConnectionPool*>(m_mongodb_ptr);
	if (dbapi == nullptr) { return false; }

	bson_t query;
	bson_t fields;
	std::string key, value;
	bson_init(&query);
	bson_init(&fields);

	for (auto& itr : id_map)
	{
		key = itr.first;
		value = itr.second;
		BSON_APPEND_UTF8(&query, key.c_str(), value.c_str());
	}
	for (auto& itr : field_map)
	{
		key = itr;
		BSON_APPEND_UTF8(&fields, key.c_str(), "1");
	}
	return dbapi->select_coll_by_fields(dbname, collname, &query, &fields, map_data);
}