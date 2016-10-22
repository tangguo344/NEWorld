/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EVENT_H_
#define EVENT_H_
#include <map>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include "exception.h"

namespace Event
{
    namespace Priority
    {
        ////////////////////////////////////////////////////////////////////////////////
        ///    @brief    Event优先级
        ///    @note    为防止污染全局命名空间将其放置于Priority命名空间\n
        ///            EventHandle执行优先级顺序为高-普通-低
        ////////////////////////////////////////////////////////////////////////////////
        enum Priority
        {
            High = 1,    ///< @brief    高优先级
            Normal = 2,    ///< @brief    普通优先级
            Low = 3        ///< @brief    低优先级
        };
    }

    class EventBase
    {
    public:
        constexpr EventBase() noexcept
            : m_Canceled(false)
        {
        }
        virtual ~EventBase();

        virtual bool CanCancel() const noexcept;
        virtual void SetCancel(bool value) noexcept;
        virtual bool IsCanceled() const noexcept;

    private:
        bool m_Canceled;
    };

    class EventBus final
    {
    public:
        typedef std::function<void(EventBase&)> EventListenerDelegate;
        typedef int PriorityType;
        typedef unsigned ListenerIDType;

        template <typename EventClass>
        std::enable_if_t<std::is_base_of<EventBase, EventClass>::value, void> RegisterEvent()
        {
            bool Succeeded;
            tie(std::ignore, Succeeded) = m_EventListenerMap.try_emplace(typeid(EventClass));

            if (!Succeeded)
            {
                nw_throw(Exception::Exception, "Cannot register event \"{0}\"", typeid(EventClass).name());
            }
        }

        template <typename EventClass>
        ListenerIDType RegisterEventListener(EventListenerDelegate const& listener, PriorityType priority = Priority::Normal)
        {
            auto iter = m_EventListenerMap.find(typeid(EventClass));
            if (iter == m_EventListenerMap.end())
            {
                nw_throw(Exception::Exception, "Unregistered event.");
            }

            auto&& listeners = iter->second[priority];
            auto ret = listeners.empty() ? 0u : listeners.rbegin()->first + 1u;
            listeners.try_emplace(ret, listener);
            return ret;
        }

        template <typename EventClass>
        void UnregisterEventListener(PriorityType priority, ListenerIDType ListenerID)
        {
            auto iter = m_EventListenerMap.find(typeid(EventClass));
            if (iter == m_EventListenerMap.end())
            {
                nw_throw(Exception::Exception, "Unregistered event.");
            }

            auto listeneriter = iter->second.find(priority);
            if (listeneriter != iter->second.end())
            {
                listeneriter->second.erase(ListenerID);
            }
        }

        template <typename EventClass>
        bool Post(EventClass& event)
        {
            auto iter = m_EventListenerMap.find(typeid(EventClass));
            if (iter == m_EventListenerMap.end())
            {
                nw_throw(Exception::Exception, "Unregistered event.");
            }

            for (auto&& listeners : iter->second)
            {
                for (auto&& listener : listeners.second)
                {
                    listener.second(static_cast<EventBase&>(event));
                }
            }

            return event.IsCanceled();
        }

    private:
        std::unordered_map<std::type_index, std::map<PriorityType, std::map<ListenerIDType, EventListenerDelegate>>> m_EventListenerMap;
    };
}

#endif // EVENTBUS_H_
