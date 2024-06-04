export module sound;

import std;
import converter;
import binary_reader;
import binary_writer;

export class sound final : public converter {
public:
    inline static const std::string reader{ "Microsoft.Xna.Framework.Content.SoundEffectReader" };
    const std::uint32_t format_size;
    const std::vector<std::uint8_t> format;
    const std::uint32_t data_size;
    const std::vector<std::uint8_t> data;
    const std::int32_t loop_start;
    const std::int32_t loop_length;
    const std::int32_t duration;

    sound(const std::uint32_t format_size, const std::vector<std::uint8_t>& format, const std::uint32_t data_size, const std::vector<std::uint8_t>& data, const std::int32_t loop_start, const std::int32_t loop_length, const std::int32_t duration) noexcept : format_size{ format_size }, format{ format }, data_size{ data_size }, data{ data }, loop_start{ loop_start }, loop_length{ loop_length }, duration{ duration } {}
    sound() noexcept = delete;

    sound(const sound&)  noexcept = delete;
    sound& operator=(const sound&) noexcept = delete;

    sound(sound&& other) noexcept = delete;
    sound& operator=(sound&&) noexcept = delete;

    static [[nodiscard]] std::expected<std::unique_ptr<sound>, std::string> read_from_memory(binary_reader& stream) noexcept
    {
        const std::uint32_t format_size{ stream.read<std::uint32_t>() };
        if (format_size != 18) return std::unexpected("Audio format not supported");

        std::vector<std::uint8_t> format{};
        for (std::size_t i{ 0 }; i < format_size; i++) {
            format.emplace_back(stream.read<std::uint8_t>());
        }

        const std::uint32_t data_size{ stream.read<std::uint32_t>() };
        std::vector<std::uint8_t> data{};
        for (std::size_t i{ 0 }; i < data_size; i++) {
            data.emplace_back(stream.read<std::uint8_t>());
        }

        const std::int32_t loop_start{ stream.read<std::int32_t>() };
        const std::int32_t loop_length{ stream.read<std::int32_t>() };
        const std::int32_t duration{ stream.read<std::int32_t>() };

        return std::expected<std::unique_ptr<sound>, std::string>(
            std::make_unique<sound>(
                format_size,
                format,
                data_size,
                data,
                loop_start,
                loop_length,
                duration
            )
        );
    }

    static [[nodiscard]] std::expected<std::unique_ptr<sound>, std::string> read_from_file(const std::filesystem::path& file) noexcept
    {
        if (file.extension() != ".wav") return std::unexpected("Wrong file format");

        binary_reader stream{ file };

        stream.ignore(16);
        const std::uint32_t format_size{ stream.read<std::uint32_t>() };

        std::vector<std::uint8_t> format;
        for (std::size_t i{ 0 }; i < format_size; i++) {
            format.emplace_back(stream.read<std::uint8_t>());
        }

        stream.ignore(4);
        const std::uint32_t data_size{ stream.read<std::uint32_t>() };

        std::vector<std::uint8_t> data;
        for (std::size_t i{ 0 }; i < data_size; i++) {
            data.emplace_back(stream.read<std::uint8_t>());
        }

        return std::expected<std::unique_ptr<sound>, std::string>(
            std::make_unique<sound>(
                format_size,
                format,
                data_size,
                data,
                0,
                0,
                0
            )
        );
    }

    std::optional<std::string> export_file(const std::filesystem::path& file) const noexcept override
    {
        if (file.extension() != ".wav") return std::make_optional(std::string{ "Wrong file extension" });

        const std::size_t size{ data.size() + format.size() + 20 };

        binary_writer stream{ file };
        stream.write<std::string>("RIFF");
        stream.write<std::int32_t>(static_cast<std::int32_t>(size - 8));
        stream.write<std::string>("WAVE");
        stream.write<std::string>("fmt ");

        const std::int16_t type_format{ static_cast<std::int16_t>((format[1] << 8) | format[0]) };
        const std::int16_t flags{ static_cast<std::int16_t>((format[3] << 8) | format[2]) };
        const std::int32_t sample_rate{ static_cast<std::int32_t>((format[7] << 24) | (format[6] << 16) | (format[5] << 8) | format[4]) };
        const std::int32_t bytes_per_sec{ static_cast<std::int32_t>((format[11] << 24) | (format[10] << 16) | (format[9] << 8) | format[8]) };
        const std::int16_t block_alignment{ static_cast<std::int16_t>((format[13] << 8) | format[12]) };
        const std::int16_t bits_per_sample{ static_cast<std::int16_t>((format[15] << 8) | format[14]) };

        stream.write<std::int32_t>(format_size);
        stream.write<std::int16_t>(type_format);
        stream.write<std::int16_t>(flags);
        stream.write<std::int32_t>(sample_rate);
        stream.write<std::int32_t>(bytes_per_sec);
        stream.write<std::int16_t>(block_alignment);
        stream.write<std::int16_t>(bits_per_sample);

        stream.write<std::uint8_t>(0);
        stream.write<std::uint8_t>(0);

        stream.write<std::string>("data");
        stream.write<std::int32_t>(data_size);
        for (const std::uint8_t data : data) {
            stream.write<std::uint8_t>(data);
        }

        return std::nullopt;
    }

    [[nodiscard]] std::string to_binary() const noexcept override
    {
        binary_writer stream{};
        stream.write<std::uint32_t>(format_size);
        for (const std::uint8_t format : format) {
            stream.write<std::uint8_t>(format);
        }

        stream.write<std::uint32_t>(data_size);
        for (const std::uint8_t data : data) {
            stream.write<std::uint8_t>(data);
        }

        stream.write<std::int32_t>(loop_start);
        stream.write<std::int32_t>(loop_length);
        stream.write<std::int32_t>(duration);

        return stream.to_string();
    }

    [[nodiscard]] const std::string& reader_name() const noexcept override
    {
        return sound::reader;
    }
};