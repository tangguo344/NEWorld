#pragma once

class Task
{
public:
    enum class Priority { VeryHigh, High, Normal, Low, VeryLow };
    Task(std::chrono::steady_clock::duration duration, Priority priority)
        :mDuration(duration), mPriority(priority) {}
    virtual ~Task() = default;
    virtual void execute() = 0;
private:
    std::chrono::steady_clock::duration mDuration;
    Priority mPriority;
};

class FunctionTask : public Task
{
public:
    FunctionTask(std::chrono::steady_clock::duration duration, Priority priority,
        const std::function<void()>& callable) :
        mCallable(callable), Task(duration, priority) {}
    virtual void execute() override { mCallable(); }
private:
    std::function<void()> mCallable;
};
