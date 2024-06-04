export module xnb;

import std;
import binary_reader;
import binary_writer;
import platform;
import flags;
import compress;
import converter;
import convertible;
import readers;

export class xnb final {
private:
    constexpr static std::uint8_t _header_size{ 14 };

public:
    const platform target_platform;
    const std::uint8_t format_version;
    const flags flags_bits;
    const std::unique_ptr<converter> type;

    xnb(const platform target_platform, const std::uint8_t format_version, const flags flags_bits, std::unique_ptr<converter> type) noexcept : target_platform{ target_platform }, format_version{ format_version }, flags_bits{ flags_bits }, type{ std::move(type) } {}
    xnb() = delete;

    xnb(const xnb&) = delete;
    xnb& operator=(const xnb&) = delete;

    xnb(xnb&&) = delete;
    xnb& operator=(xnb&&) = delete;

    static [[nodiscard]] std::expected<xnb, std::string> read_xnb(const std::filesystem::path& file) noexcept
    {
        if (file.extension() != ".xnb") return std::unexpected("Wrong file format");
        if (!std::filesystem::exists(file)) return std::unexpected("File does not exists");

        binary_reader stream{ file };

        const std::string header{ stream.read<std::string>(3) };
        if (header != "XNB") return std::unexpected("Invalid header");

        const char platform_bits{ static_cast<char>(stream.read<std::uint8_t>()) };
        platform target_platform{ platform::none };
        switch (platform_bits) {
        case 'w':
            target_platform = platform::windows;
            break;
        case 'm':
            target_platform = platform::phone;
            break;
        case 'x':
            target_platform = platform::xbox;
            break;
        }

        if (target_platform != platform::windows) return std::unexpected("Invalid platform, only Windows supported");

        std::uint8_t format_version{ stream.read<std::uint8_t>() };
        if (format_version != 5) return std::unexpected("Invalid format, must be XNA 4.0");

        std::uint8_t bits{ stream.read<std::uint8_t>() };
        flags flags_bits{ flags::none };
        switch (bits) {
        case 0:
            flags_bits = flags::reach;
            break;
        case 1:
            flags_bits = flags::hi_def;
            break;
        case 128:
            flags_bits = flags::compressed;
            break;
        }

        if (flags_bits == flags::none) return std::unexpected("Invalid flags");

        std::size_t compressed_size{ stream.read<std::uint32_t>() };
        std::size_t decompressed_size{ 0 };

        if (flags_bits == flags::compressed) {
            decompressed_size = stream.read<std::uint32_t>();

            std::string compressed_data{ stream.read<std::string>(static_cast<std::size_t>(compressed_size) - _header_size) };

            XMEMDECOMPRESSION_CONTEXT decompression_context{ 0 };

            XMEMCODEC_PARAMETERS_LZX codec_params{
                .Flags = 0,
                .WindowSize = 64 * 1024,
                .CompressionPartitionSize = 256 * 1024
            };

            if (XMemCreateDecompressionContext(XMEMCODEC_TYPE::XMEMCODEC_LZX, &codec_params, 0, &decompression_context) != 0) {
                return std::unexpected("Could not create decompression context");
            }

            std::string decompressed_data{};
            decompressed_data.resize(decompressed_size);
            if (XMemDecompress(decompression_context, &decompressed_data[0], &decompressed_size, &compressed_data[0], compressed_data.size()) != 0) {
                XMemDestroyDecompressionContext(decompression_context);
                return std::unexpected("Failed decompression");
            }

            XMemDestroyDecompressionContext(decompression_context);

            binary_reader memory_stream{ decompressed_data };
            return xnb::finish_reading(memory_stream, target_platform, format_version, flags_bits);
        }

        return xnb::finish_reading(stream, target_platform, format_version, flags_bits);
    }

    template<convertible T>
    static [[nodiscard]] std::expected<xnb, std::string> write_xnb(std::filesystem::path& file) noexcept
    {
        if (!std::filesystem::exists(file)) return std::unexpected("File does not exist");

        std::expected<std::unique_ptr<converter>, std::string> type{ nullptr };
        const std::filesystem::path& extension{ file.extension() };

        for (const reader& current_reader : readers) {
            if (T::reader == current_reader.name) {
                if (std::expected<std::unique_ptr<converter>, std::string> reader_type = current_reader.read_from_file(file); reader_type.has_value()) {
                    type = std::move(reader_type);
                }
            }
        }

        if (!type.has_value()) return std::unexpected(type.error());

        const std::string& reader_name{ type.value()->reader_name() };

        binary_writer stream{ file.replace_extension(".xnb") };
        stream.write<std::string>("XNB");
        stream.write<std::uint8_t>(static_cast<std::uint8_t>('w'));
        stream.write<std::uint8_t>(5);
        stream.write<std::uint8_t>(128);

        binary_writer memory_stream{};
        memory_stream.write_7_bit_encoded_int(1);
        memory_stream.write_7_bit_encoded_int(static_cast<std::int32_t>(reader_name.size()));
        memory_stream.write<std::string>(reader_name);
        memory_stream.write<std::int32_t>(0);
        memory_stream.write_7_bit_encoded_int(0);
        memory_stream.write_7_bit_encoded_int(1);
        memory_stream.write<std::string>(type.value()->to_binary());

        const std::string decompressed_data{ memory_stream.to_string() };
        const std::size_t decompressed_size{ decompressed_data.size() };

        XMEMCOMPRESSION_CONTEXT compression_context{ 0 };

        XMEMCODEC_PARAMETERS_LZX codec_params{
            .Flags = 0,
            .WindowSize = 64 * 1024,
            .CompressionPartitionSize = 256 * 1024
        };

        if (XMemCreateCompressionContext(XMEMCODEC_TYPE::XMEMCODEC_LZX, &codec_params, 0, &compression_context) != 0) {
            return std::unexpected("Could not create decompression context");
        }

        std::size_t compressed_size{ decompressed_size };
        std::string compressed_data{};
        compressed_data.resize(compressed_size);
        if (XMemCompress(compression_context, &compressed_data[0], &compressed_size, &decompressed_data[0], decompressed_size) != 0) {
            XMemDestroyCompressionContext(compression_context);
            return std::unexpected("Failed compression");
        }

        XMemDestroyCompressionContext(compression_context);

        compressed_data.resize(compressed_size);

        stream.write<std::uint32_t>(static_cast<std::uint32_t>(compressed_size + _header_size));
        stream.write<std::uint32_t>(static_cast<std::uint32_t>(decompressed_size));
        stream.write<std::string>(compressed_data);

        return std::expected<xnb, std::string>(
            std::in_place,
            platform::windows,
            static_cast<std::uint8_t>(5),
            flags::compressed,
            std::move(type.value())
        );
    }

private:
    [[nodiscard]] static std::expected<xnb, std::string> finish_reading(binary_reader& stream, const platform target_platform, const std::uint8_t format_version, const flags flags_bits)
    {
        const std::uint8_t readers_count{ stream.read_7_bit_encoded_int() };
        if (readers_count > 1) return std::unexpected("Only 1 reader supported");

        const std::uint8_t type_length{ stream.read_7_bit_encoded_int() };
        const std::string type_name{ stream.read<std::string>(type_length) };
        const std::int32_t type_version{ stream.read<std::int32_t>() };

        const std::uint8_t resources_count{ stream.read_7_bit_encoded_int() };
        if (resources_count != 0) std::unexpected("Invalid resources count");

        const std::uint8_t index{ stream.read_7_bit_encoded_int() };
        if (index > readers_count) std::unexpected("Resources count and readers count do not match");

        std::expected<std::unique_ptr<converter>, std::string> type{ nullptr };
        for (const reader& current_reader : readers) {
            if (type_name == current_reader.name) {
                if (std::expected<std::unique_ptr<converter>, std::string> reader_type = current_reader.read_from_memory(stream); reader_type.has_value()) {
                    type = std::move(reader_type);
                }
            }
        }

        if (type.has_value()) {
            return std::expected<xnb, std::string>(
                std::in_place,
                target_platform,
                format_version,
                flags_bits,
                std::move(type.value())
            );
        }

        return std::unexpected(type.error());
    }
};