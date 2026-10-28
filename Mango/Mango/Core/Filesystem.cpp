#include "Filesystem.hpp"

#include "Core.hpp"

#include <fstream>

namespace Mango {
    std::vector<Path> FileSystem::ListDirectory(const Path& path)
    {
        std::vector<Path> entries;
        std::error_code ec;

        for (std::filesystem::directory_iterator it(path, ec), end; !ec && it != end; it.increment(ec))
        {
            entries.push_back(it->path());
        }

        return entries;
    }

    std::optional<std::vector<U8>> FileSystem::ReadBinary(const Path& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
            return std::nullopt;

        auto end = file.tellg();

        if (end < 0)
            return std::nullopt;

        auto size = static_cast<U64>(end);

        if (size > std::vector<U8>().max_size())
            return std::nullopt;

        std::vector<U8> data(size);

        file.seekg(0, std::ios::beg);

        if (!data.empty() && !file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size())))
        {
            return std::nullopt;
        }

        return data;
    }

    bool FileSystem::WriteBinary(const Path& path, const std::vector<U8>& data)
    {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);

        if (!file)
            return false;

        if (!data.empty())
        {
            file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        }

        return file.good();
    }

    namespace {
        std::ios::openmode GetOpenMode(FileMode mode)
        {
            switch (mode)
            {
                case FileMode::Read:
                    return std::ios::in | std::ios::binary;

                case FileMode::Write:
                    return std::ios::out | std::ios::trunc | std::ios::binary;

                case FileMode::Append:
                    return std::ios::out | std::ios::app | std::ios::binary;

                case FileMode::ReadWrite:
                    return std::ios::in | std::ios::out | std::ios::binary;
            }

            return {};
        }
    } // namespace

    File::File(const std::filesystem::path& path, FileMode mode) { Open(path, mode); }

    File::~File() { Close(); }

    File::File(File&& other) noexcept : m_Path(std::move(other.m_Path)), m_Stream(std::move(other.m_Stream)), m_Mode(other.m_Mode)
    {
        other.m_Path.clear();
    }

    File& File::operator=(File&& other) noexcept
    {
        if (this == &other)
            return *this;

        Close();

        m_Path = std::move(other.m_Path);
        m_Stream = std::move(other.m_Stream);
        m_Mode = other.m_Mode;

        other.m_Path.clear();

        return *this;
    }

    bool File::Open(const std::filesystem::path& path, FileMode mode)
    {
        Close();

        m_Path = path;
        m_Mode = mode;

        m_Stream.open(path, GetOpenMode(mode));

        return m_Stream.is_open();
    }

    void File::Close()
    {
        if (m_Stream.is_open())
            m_Stream.close();
    }

    U64 File::GetPosition()
    {
        if (!IsOpen())
            return 0;

        if (auto position = m_Stream.tellg(); position >= 0)
            return position;

        if (auto writePosition = m_Stream.tellp(); writePosition >= 0)
            return writePosition;

        return 0;
    }

    bool File::Seek(U64 position)
    {
        if (!IsOpen() || position > static_cast<U64>(std::numeric_limits<std::streamoff>::max()))
        {
            return false;
        }

        auto offset = static_cast<std::streamoff>(position);
        bool success = true;

        if (m_Mode != FileMode::Append)
        {
            if (m_Mode == FileMode::Read || m_Mode == FileMode::ReadWrite)
            {
                m_Stream.seekg(offset, std::ios::beg);
                success &= !m_Stream.fail();
            }

            if (m_Mode == FileMode::Write || m_Mode == FileMode::ReadWrite)
            {
                m_Stream.seekp(offset, std::ios::beg);
                success &= !m_Stream.fail();
            }
        }

        return success;
    }

    bool File::Read(void* buffer, U64 size)
    {
        if (!IsOpen() || (m_Mode != FileMode::Read && m_Mode != FileMode::ReadWrite) || (!buffer && size != 0) ||
            size > static_cast<U64>(std::numeric_limits<std::streamsize>::max()))
        {
            return false;
        }

        if (size == 0)
            return true;

        m_Stream.read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));

        return m_Stream.gcount() == static_cast<std::streamsize>(size);
    }

    bool File::Write(const void* buffer, U64 size)
    {
        if (!IsOpen() || (m_Mode != FileMode::Write && m_Mode != FileMode::Append && m_Mode != FileMode::ReadWrite) ||
            (!buffer && size != 0) || size > static_cast<U64>(std::numeric_limits<std::streamsize>::max()))
        {
            return false;
        }

        if (size == 0)
            return true;

        m_Stream.write(static_cast<const char*>(buffer), static_cast<std::streamsize>(size));

        return m_Stream.good();
    }

    std::vector<U8> File::ReadAll()
    {
        if (!IsOpen() || (m_Mode != FileMode::Read && m_Mode != FileMode::ReadWrite))
        {
            return {};
        }

        m_Stream.clear();
        m_Stream.seekg(0, std::ios::end);

        std::streampos end = m_Stream.tellg();

        if (end < 0 || static_cast<U64>(end) > std::vector<U8>().max_size())
        {
            return {};
        }

        std::vector<U8> buffer(end);

        m_Stream.seekg(0, std::ios::beg);

        if (!buffer.empty() && !Read(buffer.data(), buffer.size()))
        {
            return {};
        }

        return buffer;
    }
} // namespace Mango
