#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <memory>

namespace Mango {
    class Arena
    {
    public:
        Arena(USize size) : m_Size(size), m_Ptr(static_cast<U8*>(operator new(size))) {}

        ~Arena() { operator delete(m_Ptr); }

        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;
        Arena(Arena&&) = delete;
        Arena& operator=(Arena&&) = delete;

        void Reset() { m_Offset = 0; }

        template<typename T, typename... Args>
        T* Create(Args&&... args)
        {
            USize space = Space();
            void* aligned = m_Ptr + m_Offset;
            if (std::align(alignof(T), sizeof(T), aligned, space) == nullptr)
            {
                throw std::bad_alloc{};
            }

            m_Offset = static_cast<U8*>(aligned) - m_Ptr + sizeof(T);

            return std::construct_at(static_cast<T*>(aligned), std::forward<Args>(args)...);
        }

        template<typename T>
        void Destroy(T* ptr)
        {
            std::destroy_at(static_cast<T*>(ptr));
        }

        USize Space() const { return m_Size - m_Offset; }

    private:
        USize m_Size = 0;

        USize m_Offset = 0;
        U8* m_Ptr = nullptr;
    };
} // namespace Mango
