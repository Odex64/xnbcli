export module readers;

import std;
import binary_reader;
import converter;
import texture;
import sound;

export struct reader {
    const std::string& name;
    const std::function<std::expected<std::unique_ptr<converter>, std::string>(const std::filesystem::path& file)> read_from_file;
    const std::function<std::expected<std::unique_ptr<converter>, std::string>(binary_reader& stream)> read_from_memory;

    reader(const std::string& name, const std::function<std::expected<std::unique_ptr<converter>, std::string>(const std::filesystem::path& file)> read_from_file, std::function<std::expected<std::unique_ptr<converter>, std::string>(binary_reader& stream)> read_from_memory) noexcept : name{ name }, read_from_file{ read_from_file }, read_from_memory{ read_from_memory } {}
};

export const std::array<reader, 2> readers{
    reader {
        texture::reader,
        texture::read_from_file,
        texture::read_from_memory
    },
    reader {
        sound::reader,
        sound::read_from_file,
        sound::read_from_memory
    }
};