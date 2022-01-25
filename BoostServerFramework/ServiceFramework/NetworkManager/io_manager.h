#pragma once
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

	
/**
 * @brief IO管理类
 */

class io_service_pool;
class  IOManager: private boost::noncopyable 
{
public:
    
    static IOManager& instance();

    void setThreadNumber(const std::size_t& num);
    void SetIOServiceNumber(const std::size_t& num);
    const std::size_t& threadNumber() const;

    boost::asio::io_service& ioService();
    boost::asio::io_service& GetAsyncIoService();
	boost::asio::io_service& GetLogService();
	boost::asio::io_service& GetDbService();

    void run();
    void stop();

private:
    explicit IOManager();

private:
    std::size_t _coreNumber;                                /**< io数量 */
    std::size_t thread_number_;
    std::size_t service_pool_size_;
    std::size_t async_process_service_pool_size_;

    boost::shared_ptr<io_service_pool> service_pool_;                       /**< 需要用到的io */
    boost::shared_ptr<io_service_pool> async_process_service_pool_;         /* 需要用到的io */
	boost::shared_ptr<io_service_pool> async_log_service_pool_;
	boost::shared_ptr<io_service_pool> async_db_service_pool_;				/*数据库插入io*/
};

