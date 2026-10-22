#pragma once

#include <memory>

namespace Mango {
    template<typename T>
    class Shared
    {
    public:
        Shared() = default;
        ~Shared() = default;

        Shared(const Shared& other) { m_Data = other.m_Data; }
        Shared& operator=(const Shared& other) = default;
        Shared(Shared&& other) noexcept { m_Data = std::move(other.m_Data); }
        Shared& operator=(Shared&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename U>
        Shared(const Shared<U>& other)
        {
            m_Data = other.m_Data;
        }
        template<typename U>
        Shared& operator=(const Shared<U>& other)
        {
            m_Data = other.m_Data;
            return *this;
        }
        template<typename U>
        Shared(Shared<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
        }
        template<typename U>
        Shared& operator=(Shared<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename... Args>
        static Shared Create(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        void Reset() { m_Data.reset(); }
        void Swap(Shared& other) { m_Data.swap(other); }

        T* Raw() const { return m_Data.get(); }
        long GetSharedCount() const { return m_Data.use_count(); }

        T& operator*() const { return *m_Data; }
        T* operator->() const { return m_Data.get(); }
        T& operator[](size_t idx) const { return m_Data[idx]; }
        explicit operator bool() const { return static_cast<bool>(m_Data); }

        template<typename U>
        bool operator==(const Shared<U>& other) const
        {
            return m_Data == other.m_Data;
        }

        template<typename U>
        std::strong_ordering operator<=>(const Shared<U>& other)
        {
            return m_Data <=> other.m_Data;
        }

    private:
        Shared(std::shared_ptr<T>&& data) { m_Data = std::move(data); }

    private:
        std::shared_ptr<T> m_Data = nullptr;

        template<typename U>
        friend class Shared;
    };

    template<typename T>
    class Unique
    {
    public:
        Unique() = default;
        ~Unique() = default;

        Unique(const Unique& other) = delete;
        Unique& operator=(const Unique& other) = delete;
        Unique(Unique&& other) noexcept { m_Data = std::move(other.m_Data); }
        Unique& operator=(Unique&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename U>
        Unique(const Unique& other) = delete;
        template<typename U>
        Unique& operator=(const Unique& other) = delete;
        template<typename U>
        Unique(Unique<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
        }
        template<typename U>
        Unique& operator=(Unique<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename... Args>
        static Unique Create(Args&&... args)
        {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }

        T* Release() { return m_Data.release(); }
        void Reset(T* ptr) { m_Data.reset(ptr); }
        void Reset(std::nullptr_t ptr = nullptr) { m_Data.reset(ptr); }
        void Swap(Unique& other) { m_Data.swap(other.m_Data); }
        T* Raw() const { return m_Data.get(); }

        T& operator*() const { return *m_Data; }
        T* operator->() const { return m_Data.get(); }
        T& operator[](size_t idx) const { return m_Data[idx]; }
        explicit operator bool() const { return static_cast<bool>(m_Data); }

        template<typename U>
        bool operator==(const Shared<U>& other) const
        {
            return m_Data == other.m_Data;
        }

        template<typename U>
        std::strong_ordering operator<=>(const Shared<U>& other)
        {
            return m_Data <=> other.m_Data;
        }

    private:
        Unique(std::unique_ptr<T>&& data) { m_Data = std::move(data); }

    private:
        std::unique_ptr<T> m_Data = nullptr;

        template<typename U>
        friend class Unique;
    };

    template<typename T>
    class Weak
    {
    public:
        Weak() = default;
        ~Weak() = default;

        Weak(const Weak& other) { m_Data = other.m_Data; }
        Weak& operator=(const Weak& other) = default;
        Weak(Weak&& other) noexcept { m_Data = std::move(other.m_Data); }
        Weak& operator=(Weak&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename U>
        Weak(const Weak<U>& other)
        {
            m_Data = other.m_Data;
        }
        template<typename U>
        Weak& operator=(const Weak<U>& other)
        {
            m_Data = other.m_Data;
            return *this;
        }
        template<typename U>
        Weak(Weak<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
        }
        template<typename U>
        Weak& operator=(Weak<U>&& other) noexcept
        {
            m_Data = std::move(other.m_Data);
            return *this;
        }

        template<typename... Args>
        static Weak Create(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        void Reset() { m_Data.reset(); }
        void Swap(Weak& other) { m_Data.swap(other); }

        T* Raw() const { return m_Data.get(); }
        long GetSharedCount() const { return m_Data.use_count(); }
        bool Expired() const { return m_Data.expired(); }
        Shared<T> Lock() const { return m_Data.lock(); }

        T& operator*() const { return *m_Data; }
        T* operator->() const { return m_Data.get(); }
        T& operator[](size_t idx) const { return m_Data[idx]; }
        explicit operator bool() const { return static_cast<bool>(m_Data); }

    private:
        Weak(std::weak_ptr<T>&& data) { m_Data = std::move(data); }

    private:
        std::weak_ptr<T> m_Data = nullptr;
    };
} // namespace Mango
