#ifndef WORK_QUEUE_H_
#define WORK_QUEUE_H_

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class custom_priority_queue : public std::priority_queue<T, std::vector<T>> {
 public:
	typedef typename std::vector<T>::iterator iterator;
	T PopBack() {
		std::make_heap(this->c.rbegin(), this->c.rend());
		T ret = this->c.front();
		this->c.erase(this->c.begin());
		std::make_heap(this->c.begin(), this->c.end());
		return ret;
	}

	void Clear() {
		this->c.clear();
		std::make_heap(this->c.begin(), this->c.end());
	}
};

template<typename T>
class WorkQueue {
 public:
	WorkQueue() : initialized_(false) {
	}

	bool IsActive() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		return initialized_;
	}

	void Start() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		initialized_ = true;
	}

	void Stop() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		initialized_ = false;
		work_message_.notify_all();
	}

	void Add(const T& data, size_t priority) {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		PriorityStruct priority_struct;
		priority_struct.data = data;
		priority_struct.priority = priority;
		data_queue_.push(priority_struct);
		work_message_.notify_all();
	}

	void Add(const T& data) {
		Add(data, 0);
	}

	template<typename ChronoDurationType>
	bool Get(T& data, ChronoDurationType timeout) {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		while (initialized_) {
			if (!data_queue_.empty()) {
				data = data_queue_.top().data;
				data_queue_.pop();
				return true;
			}
			else {
				if (!Wait(locker, timeout)) return false;
			}
		}
		return false;
	}

	bool Get(T& data) {
		return Get(data, std::chrono::nanoseconds::zero());
	}

	void Clear() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		data_queue_.Clear();
	}

	bool PopBack() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		if (data_queue_.empty()) return false;
		data_queue_.PopBack();
		return true;
	}

	bool PopBack(T& back_item) {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		if (data_queue_.empty()) return false;
		back_item = data_queue_.PopBack().data;
		return true;
	}

	size_t Size() {
		std::unique_lock<std::mutex> locker(queue_mutex_);
		return data_queue_.size();
	}

 private:
	template<typename ChronoDurationType>
	bool Wait(std::unique_lock<std::mutex>& locker, ChronoDurationType timeout) {
		if (timeout != std::chrono::nanoseconds::zero()) {
			return work_message_.wait_for(locker, timeout) == std::cv_status::no_timeout;
		}
		work_message_.wait(locker);
		return true;
	}

	struct PriorityStruct {
		size_t priority;
		T data;
		bool operator<(const PriorityStruct& right_data) const {
			return this->priority < right_data.priority;
		}
	};

	bool initialized_;
	std::mutex queue_mutex_;
	custom_priority_queue<PriorityStruct> data_queue_;
	std::condition_variable work_message_;
};

#endif  // WORK_QUEUE_H_
