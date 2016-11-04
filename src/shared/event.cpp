#include "event.h"


Event::EventBase::~EventBase()
{
}

bool Event::EventBase::CanCancel() const noexcept
{
	return false;
}

void Event::EventBase::SetCancel(bool value) noexcept
{
	if (CanCancel())
	{
		mCanceled = value;
	}
}

bool Event::EventBase::IsCanceled() const noexcept
{
	return mCanceled;
}
