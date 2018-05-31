# work-queue
A queue system with processing priorities for processing certain information that the user wants, everything being thread safe

In this project we have 2 files:

main_example.cpp and worker_queue.h

In main_example.cpp, we have an example of using work queue, an executable.
in worker_queue.h, we have the implementation of work queue

You can insert items in the worker queue to be processed, you can enter items with priority, the higher the value will be the priority to be processed .. if you do not have items to be processed the work queue will wait until you receive a new item.
All this mechanism is thread safe.

Method documentation:

# void Add(const T& data)
adds an item to be processed

# void Add(const T& data, size_t priority)
adds an item to be processed with priority the higher the number it will be processed first

# bool Get(T& data, ChronoDurationType timeout)
picks up an item to be processed with timeout

# bool Get(T& data)
picks up an item to be processed if dont have item to be processed then will wait until have item to process

# bool PopBack(T& back_item)
picks up the item from the top of the queue and places it in the parameter

# bool PopBack()
remove the item from the top of the queue

# size_t Size()
return size of the queue

# void Clear()
clear the stack

# void Stop()
stop worker queue

# void Start()
start worker queue

# bool IsActive()
verify if work queue is active
