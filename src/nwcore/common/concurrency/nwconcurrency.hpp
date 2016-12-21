#pragma once

class Task
{
public:
	enum class Priority { VeryHigh, High, Normal, Low, VeryLow };
	virtual void execute() = 0;
	virtual ~Task() = default;
private:
	std::chrono::duration mDuration;
};

class FunctionTask : public Task
{
public:
	FunctionTask(const std::function<void()>& callable) :mCallable(callable) {}
	virtual void execute() override { mCallable(); }
private:
	std::function<void()> mCallable;
};
