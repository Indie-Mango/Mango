#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <any>
#include <functional>
#include <map>
#include <ranges>
#include <typeindex>

namespace Mango {
    enum class EventState : U8
    {
        Continue,
        Handled
    };

    using EventConnectionHandle = U64;

    class EventBus;

    template<typename T>
    class EventConnection
    {
    public:
        EventConnection() = default;
        EventConnection(EventBus* eventBus, EventConnectionHandle handle);

        ~EventConnection() { Disconnect(); }

        EventConnection(const EventConnection&) = delete;
        EventConnection& operator=(const EventConnection&) = delete;

        EventConnection(EventConnection&& other) noexcept :
            m_EventBus(std::exchange(other.m_EventBus, nullptr)), m_Handle(std::exchange(other.m_Handle, 0))
        {
        }

        EventConnection& operator=(EventConnection&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_EventBus = std::exchange(other.m_EventBus, nullptr);
            m_Handle = std::exchange(other.m_Handle, 0);

            return *this;
        }

        bool Connected() const;
        void Disconnect();

    private:
        EventBus* m_EventBus = nullptr;
        EventConnectionHandle m_Handle = 0;
    };

    class EventBus
    {
    public:
        using EventCallback = std::function<EventState(const std::any&)>;

    public:
        template<typename T>
        bool Connected(EventConnectionHandle handle);

        template<typename T, typename F>
        EventConnection<T> Connect(F&& callback);

        template<typename T>
        void Disconnect(EventConnectionHandle handle);

        template<typename T>
            requires std::is_copy_constructible_v<T>
        void Emit(T&& event)
        {
            std::any any{std::forward<T>(event)};
            m_Events.push_back(std::make_pair(std::type_index{typeid(T)}, std::move(any)));
        }

        void Update()
        {
            for (auto events = std::exchange(m_Events, {}); auto& [type, event]: events)
            {
                if (auto it = m_Callbacks.find(type); it != m_Callbacks.end())
                {
                    for (auto& callback: it->second | std::views::values)
                    {
                        if (callback(event) == EventState::Handled)
                            break;
                    }
                }
            }
        }

    private:
        std::map<std::type_index, std::map<EventConnectionHandle, EventCallback>> m_Callbacks;
        std::vector<std::pair<std::type_index, std::any>> m_Events;
        EventConnectionHandle m_CurrentHandle = 1;
    };

    template<typename T>
    EventConnection<T>::EventConnection(EventBus* eventBus, EventConnectionHandle handle) : m_EventBus(eventBus), m_Handle(handle)
    {
    }

    template<typename T>
    bool EventConnection<T>::Connected() const
    {
        return m_EventBus && m_EventBus->Connected<T>(m_Handle);
    }

    template<typename T>
    void EventConnection<T>::Disconnect()
    {
        if (!m_EventBus)
            return;

        m_EventBus->Disconnect<T>(m_Handle);

        m_EventBus = nullptr;
        m_Handle = 0;
    }

    template<typename T, typename F>
    EventConnection<T> EventBus::Connect(F&& callback)
    {
        auto wrapper = [fn = std::forward<F>(callback)](const std::any& event) mutable -> EventState {
            return std::invoke(fn, std::any_cast<const T&>(event));
        };

        EventCallback eventCallback = std::move(wrapper);

        m_Callbacks[std::type_index{typeid(T)}].emplace(m_CurrentHandle, std::move(eventCallback));

        return EventConnection<T>{this, m_CurrentHandle++};
    }

    template<typename T>
    bool EventBus::Connected(EventConnectionHandle handle)
    {
        if (auto it = m_Callbacks.find(std::type_index{typeid(T)}); it != m_Callbacks.end())
        {
            if (it->second.contains(handle))
            {
                return true;
            }
        }

        return false;
    }

    template<typename T>
    void EventBus::Disconnect(EventConnectionHandle handle)
    {
        if (auto it = m_Callbacks.find(std::type_index{typeid(T)}); it != m_Callbacks.end())
        {
            it->second.erase(handle);
        }
    }
} // namespace Mango
