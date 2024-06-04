module;

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Dependencies/stb_image_write.h"

export module texture;

import std;
import converter;
import binary_reader;
import binary_writer;

export class texture final : public converter {
public:
    inline static const std::string reader{ "Microsoft.Xna.Framework.Content.Texture2DReader, Microsoft.Xna.Framework.Graphics, Version=4.0.0.0, Culture=neutral, PublicKeyToken=842cf8be1de50553" };
    const std::int32_t format;
    const std::uint32_t width;
    const std::uint32_t height;
    const std::uint32_t mip_count;
    const std::uint32_t data_size;
    const std::vector<std::uint8_t> data;

    texture(const std::int32_t format, const std::uint32_t width, const std::uint32_t height, const std::uint32_t mip_count, const std::uint32_t data_size, const std::vector<std::uint8_t>& data) noexcept : format{ format }, width{ width }, height{ height }, mip_count{ mip_count }, data_size{ data_size }, data{ data } {}
    texture() noexcept = delete;

    texture(const texture&) noexcept = delete;
    texture& operator=(const texture&) noexcept = delete;

    texture(texture&& other) noexcept = delete;
    texture& operator=(texture&&) noexcept = delete;

    static [[nodiscard]] std::expected<std::unique_ptr<texture>, std::string> read(binary_reader& stream) noexcept
    {
        const std::int32_t format{ stream.read<std::int32_t>() };
        if (format != 0) return std::unexpected("Invalid texture format");

        const std::uint32_t width{ stream.read<std::uint32_t>() };
        const std::uint32_t height{ stream.read<std::uint32_t>() };

        const std::uint32_t mip_count{ stream.read<std::uint32_t>() };
        if (mip_count > 1) return std::unexpected("Invalid mip count. Must be 1");

        const std::uint32_t data_size{ stream.read<std::uint32_t>() };

        std::vector<std::uint8_t> data(data_size);
        for (std::size_t i{ 0 }; i < data.size(); i += 4) {
            data[i] = stream.read<std::uint8_t>();
            data[i + 1] = stream.read<std::uint8_t>();
            data[i + 2] = stream.read<std::uint8_t>();
            data[i + 3] = stream.read<std::uint8_t>();
        }

        return std::expected<std::unique_ptr<texture>, std::string>(
            std::make_unique<texture>(
                format,
                width,
                height,
                mip_count,
                data_size,
                data
            )
        );
    }

    static [[nodiscard]] std::expected<std::unique_ptr<texture>, std::string> read(const std::filesystem::path& file) noexcept
    {
        if (file.extension() != ".png") return std::unexpected("Wrong file format");

        std::int32_t width{};
        std::int32_t height{};
        std::int32_t comp{};
        stbi__result_info info{};

        stbi_uc* raw{ stbi_load(file.string().c_str(), &width, &height, &comp, STBI_rgb_alpha) };
        std::vector<std::uint8_t> data(raw, raw + width * height * STBI_rgb_alpha);

        return std::expected<std::unique_ptr<texture>, std::string>(
            std::make_unique<texture>(
                0,
                width,
                height,
                1,
                static_cast<std::uint32_t>(data.size()),
                data
            )
        );
    }

    std::optional<std::string> export_file(const std::filesystem::path& file) const noexcept override
    {
        if (file.extension() != ".png") return std::make_optional(std::string{ "Wrong file extension" });

        stbi_write_png(file.string().c_str(), width, height, STBI_rgb_alpha, data.data(), width * STBI_rgb_alpha);

        return std::nullopt;
    }

    [[nodiscard]] std::string to_binary() const noexcept override
    {
        binary_writer stream{};
        stream.write<std::int32_t>(format);
        stream.write<std::uint32_t>(width);
        stream.write<std::uint32_t>(height);
        stream.write<std::uint32_t>(mip_count);
        stream.write<std::uint32_t>(data_size);

        for (const std::uint8_t pixel : data) {
            stream.write<std::uint8_t>(pixel);
        }

        return stream.to_string();
    }

    [[nodiscard]] const std::string& reader_name() const noexcept override
    {
        return texture::reader;
    }
};