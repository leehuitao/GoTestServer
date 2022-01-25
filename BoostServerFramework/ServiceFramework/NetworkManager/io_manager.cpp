
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include "io_manager.h"
#include <iostream>

	class io_service_pool: private boost::noncopyable
	{
	public://io_service总数 设置每个io_service 几个线程  
		explicit io_service_pool(std::size_t pool_size, std::size_t thread_num) : next_io_service_(0) {
			if (pool_size == 0 || thread_num <= 0)
				throw std::runtime_error("io_service_pool size is 0");

			for (std::size_t i = 0; i < pool_size; ++i) {
				io_service_ptr io_service(new boost::asio::io_service);
				work_ptr work(new boost::asio::io_service::work(*io_service));
				io_services_.push_back(io_service);
				work_.push_back(work);
			}

			thread_num_ = thread_num;
		}

		void run() {
			for (auto io_service : io_services_) {
				for (std::size_t i = 0; i < thread_num_; ++i) {
					threads.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, io_service)));
				}
			}
			
		}
		void join() {
			threads.join_all();
		}

		void stop() {
			for (std::size_t i = 0; i < io_services_.size(); ++i) {
				io_services_[i]->stop();
			}
				
		}

		boost::asio::io_service& get_io_service() {
			boost::mutex::scoped_lock lock(mutex_service_);
			boost::asio::io_service& io_service = *io_services_[next_io_service_];

			++next_io_service_;
			if (next_io_service_ == io_services_.size()) {
				next_io_service_ = 0;
			}

			return io_service;
		}

	private:
		typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
		typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

		std::vector<io_service_ptr> io_services_;

		boost::mutex mutex_service_;
		std::vector<work_ptr> work_;
		std::size_t next_io_service_;
		
		std::size_t thread_num_;

		// std::vector<boost::shared_ptr<boost::thread> > threads;
		boost::thread_group threads;

	};

/**
 * @brief 创建IOManager的单例对象
 * @return 返回IOManager对象
 */
IOManager& IOManager::instance()
{
    static IOManager _gInstance;
    return _gInstance;
}

/**
 * @brief 初始化IOManager对象
 * @param
 */
IOManager::IOManager():
        _coreNumber(boost::thread::hardware_concurrency())
{
	service_pool_size_ = _coreNumber*2;// _coreNumber; // 默认1个IO_SERVICE对象
	async_process_service_pool_size_ = _coreNumber * 2;// _coreNumber;

	thread_number_ = 1; // 默认每个IO_SERVICE两个thread，经验值
}

/**
 * @brief 初始化IOManager对象
 * @param
 */
void IOManager::setThreadNumber(const std::size_t& num)
{
	std::cout << std::endl << "[INIT] IOManager::setThreadNumber(" << num << ")" << std::endl;

	if (num > 0) {
		thread_number_ = num * 2;
	}
}

void IOManager::SetIOServiceNumber(const std::size_t& num)
{
	std::cout << "IOManager::SetIOServiceNumber" << std::endl;
	if (num > 0) {
		service_pool_size_ = num;
		async_process_service_pool_size_ = num;
	}
}

const std::size_t& IOManager::threadNumber() const
{
    return thread_number_;
}

boost::asio::io_service& IOManager::ioService()
{
	if (!service_pool_)	{
		service_pool_.reset(new io_service_pool(service_pool_size_, thread_number_));
	}

	return service_pool_->get_io_service();
}

boost::asio::io_service& IOManager::GetAsyncIoService()
{
	if (!async_process_service_pool_) {
		async_process_service_pool_.reset(new io_service_pool(async_process_service_pool_size_, thread_number_));
	}

	return async_process_service_pool_->get_io_service();
}

boost::asio::io_service& IOManager::GetLogService()
{
	if (!async_log_service_pool_) {
		async_log_service_pool_.reset(new io_service_pool(1, 1));
	}

	return async_log_service_pool_->get_io_service();
}

boost::asio::io_service& IOManager::GetDbService()
{
	if (!async_db_service_pool_) {
		async_db_service_pool_.reset(new io_service_pool(1, 10));
	}

	return async_db_service_pool_->get_io_service();
}

void IOManager::run()
{
	if (!service_pool_) {
		service_pool_.reset(new io_service_pool(service_pool_size_, thread_number_));
	}

	service_pool_->run();

	if (!async_process_service_pool_){
		async_process_service_pool_.reset(new io_service_pool(async_process_service_pool_size_, thread_number_));
	}
	async_process_service_pool_->run();

	if (!async_log_service_pool_) {
		async_log_service_pool_.reset(new io_service_pool(1, 1));
	}
	async_log_service_pool_->run();

	if (!async_db_service_pool_) {
		async_db_service_pool_.reset(new io_service_pool(1, 1));
	}
	async_db_service_pool_->run();

	service_pool_->join();
	async_process_service_pool_->join();
	async_log_service_pool_->join();
	async_db_service_pool_->join();

}

void IOManager::stop()
{
	if (service_pool_) service_pool_->stop();
	if (async_process_service_pool_) async_process_service_pool_->stop();
	if (async_log_service_pool_) async_log_service_pool_->stop();
	if (async_db_service_pool_) async_db_service_pool_->stop();
}
