#pragma once

#include <functional>
#include <mutex>
#include <vector>

namespace bvr::core {

class TaskQueue {
public:
    using Task = std::function<void()>;

    void submit(Task task) {
        std::scoped_lock lock(mutex_);
        tasks_.push_back(std::move(task));
    }

    void process() {
        std::vector<Task> tasks_to_execute;

        {
            // Lock, steal the data, and unlock immediately
            std::scoped_lock lock(mutex_);
            tasks_to_execute = std::move(tasks_);
        }

        for (auto& task : tasks_to_execute) {
            task();
        }
    }

private:
    std::mutex mutex_;
    std::vector<Task> tasks_;
};

}  // namespace bvr::core
