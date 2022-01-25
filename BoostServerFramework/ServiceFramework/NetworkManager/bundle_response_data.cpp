/**
 * @file BundleResponseData.cpp
 * @brief 打包应答数据的实现文件
 * @author Wim
 * @version v4.0
 * @date 2014-12-26
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include "bundle_response_data.h"


/**
 * @brief 初始化BundleResponseData对象
 */
BundleResponseData::BundleResponseData()
{
	//AntUtils::mmonitor::instance().increase("BundleResponseData");
}

BundleResponseData::~BundleResponseData() {
	//AntUtils::mmonitor::instance().reduce("BundleResponseData");
}
/**
 * @brief 打包所有需应答的数据
 * @param data 应答数据缓冲区的指针
 * @return 返回打包好的应答数据
 */
std::string BundleResponseData::bundleAllData(ResponseBufferPtr data)
{
    _allData.str("");

    this->writeParamsData(data);
    this->writePropsData(data);
    this->writeBodyData(data);

    std::string ret_string = _allData.str();

	std::string endata;

    char rand_char1 = '1';// = (char)_utils.randIntValue(0, 255);
    char rand_char2 = '1';// = (char)_utils.randIntValue(0, 255);
    std::string psw_key = "";// _utils.GetMsgEncryptKey();
	psw_key = rand_char1 + (rand_char2 + psw_key);

	if (MsgProtocolEncryptFuc::kFuncSM4 == data->encript_func_)
	{
		//_utils.SM4Encrypt(ret_string, psw_key, endata);

	}
	else if (MsgProtocolEncryptFuc::kFuncAes == data->encript_func_)
	{
		//_utils.AES_ECB_EncryptString(ret_string, psw_key, 256, endata);
	}
	else
	{
		endata = ret_string;
	}

    MessageHeader msg_header;
    memset( &msg_header, 0, sizeof(MessageHeader));
    msg_header.packet_length_ 	= sizeof(MessageHeader) + endata.size();
    msg_header.order_num_ 		= data->order_num_;
    msg_header.header_length_ 	= rand_char1;
    msg_header.protocol_ver_ 	= rand_char2;
    msg_header.method_ 			= data->method_;
    msg_header.status_num_ 		= data->status_num_;
    msg_header.encript_func_ 	= data->encript_func_;

    msg_header.packet_length_ 	= htobe32(msg_header.packet_length_);
    msg_header.order_num_ 		= htobe32(msg_header.order_num_);
    msg_header.method_ 			= htobe16(msg_header.method_);

    std::string msg_header_str;
    msg_header_str.assign((char*)&msg_header, sizeof(MessageHeader));
    return msg_header_str + endata;
}

/**
 * @brief 写入params数据
 * @param data 应答数据缓冲区的指针
 */
void BundleResponseData::writeParamsData(ResponseBufferPtr data)
{
    for (int i = 0; i < data->params_.size(); ++i) {
    	if (i != 0)
    	_allData << " ";
        _allData << "";// utils.encodeURL(data->params_[i]);
    }
    _allData << "\n";
}

/**
 * @brief 写入props数据
 * @param data 应答数据缓冲区的指针
 */
void BundleResponseData::writePropsData(ResponseBufferPtr data)
{
    PropMap::iterator it = data->props_.begin();
    for (; it != data->props_.end(); ++it) {
        _allData << it->first;
        _allData << ":";
        _allData << "";// _utils.encodeURL(it->second);
        _allData << "\n";
    }
    _allData << "\n";
}

void BundleResponseData::writeBodyData(ResponseBufferPtr data)
{
    _allData << data->body_;
}
