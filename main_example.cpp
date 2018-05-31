#include "worker_queue.h"

struct WorkItem {
	unsigned int id;
	std::string something;
};

class ExampleUseWorkQueue {
 public:
	ExampleUseWorkQueue() {
		work_queue_.Start();
		work_thread_ = std::thread(&ExampleUseWorkQueue::WorkThread, this);
	}

	~ExampleUseWorkQueue() {
		work_queue_.Stop();
		work_thread_.join();
	}

	void WorkThread() {
		WorkItem work_item;
		while (work_queue_.Get(work_item)) {
			// does a processing with your item...
		}
	}

	void ReceiverWithoutPriority(const WorkItem& item) {
		work_queue_.Add(item);
	}

	void ReceiverWithPriority(const WorkItem& item, const size_t& priority) {
		work_queue_.Add(item, priority);
	}

 private:
	 std::thread work_thread_;
	 WorkQueue<WorkItem> work_queue_;
};

int main() {
	ExampleUseWorkQueue example_use_work_queue;
	WorkItem item;
	item.id = 1;
	item.something = "something_to_process";
	example_use_work_queue.ReceiverWithoutPriority(item);
	WorkItem item2;
	item2.id = 2;
	item2.something = "something_to_process_2";
	example_use_work_queue.ReceiverWithoutPriority(item2);
	WorkItem item_priority_1;
	item_priority_1.id = 5;
	item_priority_1.something = "something_priority_1";
	example_use_work_queue.ReceiverWithPriority(item_priority_1, 10);
	WorkItem item_priority_2;
	item_priority_2.id = 5;
	item_priority_2.something = "something_priority_2";
	example_use_work_queue.ReceiverWithPriority(item_priority_2, 6);
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}
