#pragma once
#include <sstream>
#include "message_protocol.h"

/**
 * @brief 打包应答数据类
 */
class BundleResponseData
{
public:
    explicit BundleResponseData();
	~BundleResponseData();
    std::string bundleAllData(ResponseBufferPtr data);

private:
    void writeParamsData(ResponseBufferPtr data);
    void writePropsData(ResponseBufferPtr data);
    void writeBodyData(ResponseBufferPtr data);

private:
    
    std::ostringstream _allData;    /**< 打包数据所需流 */

};
