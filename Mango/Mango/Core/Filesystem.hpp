#pragma once
#include "Mango/Common/CommonTypes.hpp"

#include <filesystem>
#include <fstream>

namespace Mango {
    using Path = std::filesystem::path;

    class FileSystem
    {
    public:
        static bool Exists(const Path& path) { return std::filesystem::exists(path); }
        static bool IsFile(const Path& path) { return std::filesystem::is_regular_file(path); }
        static bool IsDirectory(const Path& path) { return std::filesystem::is_directory(path); }

        static bool CreateDirectory(const Path& path) { return std::filesystem::create_directory(path); }
        static bool CreateDirectories(const Path& path) { return std::filesystem::create_directories(path); }
        static bool Delete(const Path& path) { return std::filesystem::remove(path); }

        static std::vector<Path> ListDirectory(const Path& path);

        static std::optional<std::vector<U8>> ReadBinary(const Path& path);
        static bool WriteBinary(const Path& path, const std::vector<U8>& data);

        static Path GetAbsolutePath(const Path& path) { return std::filesystem::absolute(path); }
        static Path GetParentPath(const Path& path) { return path.parent_path(); }
        static std::string GetExtension(const Path& path) { return path.extension().string(); }
        static std::string GetFileName(const Path& path) { return path.filename().string(); }
        static std::string GetStem(const Path& path) { return path.stem().string(); }
    };

    enum class FileMode : U8
    {
        Read,
        Write,
        Append,
        ReadWrite
    };

    class File
    {
    public:
        File() = default;
        File(const std::filesystem::path& path, FileMode mode);
        ~File();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&& other) noexcept;
        File& operator=(File&& other) noexcept;

        bool Open(const std::filesystem::path& path, FileMode mode);
        void Close();

        bool IsOpen() const { return m_Stream.is_open(); }
        bool IsEOF() const { return m_Stream.eof(); }

        U64 GetSize() const { return std::filesystem::file_size(m_Path); }
        U64 GetPosition();

        bool Seek(U64 position);

        bool Read(void* buffer, U64 size);
        bool Write(const void* buffer, U64 size);

        std::vector<U8> ReadAll();

        const std::filesystem::path& GetPath() const { return m_Path; }

    private:
        std::filesystem::path m_Path;
        std::fstream m_Stream;
        FileMode m_Mode = FileMode::Read;
    };
} // namespace Mango
