export module binary_writer;

import binary;
import binary_type;

export class binary_writer final : public binary<std::ostream> {
public:
    explicit binary_writer(const std::filesystem::path& file) noexcept : binary{ std::make_unique<std::ofstream>(file, std::ios::binary) } {}
    explicit binary_writer() noexcept : binary{ std::make_unique<std::ostringstream>(std::ios::binary) } {}

    binary_writer(const binary_writer&) noexcept = delete;
    binary_writer& operator=(const binary_writer&) noexcept = delete;

    binary_writer(binary_writer&&) noexcept = delete;
    binary_writer& operator=(binary_writer&&) noexcept = delete;

    template<binary_type T>
    void write(const T& data)
    {
        stream->write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    template<>
    void write(const std::string& data)
    {
        std::size_t size{ data.size() };
        stream->write(data.c_str(), size);
    }

    void write_7_bit_encoded_int(std::int32_t value)
    {
        while (value > 0x7Fu) {
            write<std::uint8_t>(static_cast<std::uint8_t>(value | ~0x7Fu));
            value >>= 7;
        }

        write<std::uint8_t>(static_cast<std::uint8_t>(value));
    }

    [[nodiscard]] std::string to_string() const noexcept override
    {
        if (std::ostringstream* derived = dynamic_cast<std::ostringstream*>(stream.get()); derived != nullptr) {
            return derived->str();
        }

        return std::string{};
    }
};