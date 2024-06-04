export module binary_reader;

import binary;
import binary_type;

export class binary_reader final : public binary<std::istream> {
public:
    explicit binary_reader(const std::filesystem::path& file) noexcept : binary{ std::make_unique<std::ifstream>(file, std::ios::binary) } {}
    explicit binary_reader(const std::string& data) noexcept : binary{ std::make_unique<std::istringstream>(data, std::ios::binary) } {}
    binary_reader() noexcept = delete;

    binary_reader(const binary_reader&) noexcept = delete;
    binary_reader& operator=(const binary_reader&) noexcept = delete;

    binary_reader(binary_reader&&) noexcept = delete;
    binary_reader& operator=(binary_reader&&) noexcept = delete;

    template<binary_type T>
    [[nodiscard]] T read()
    {
        T data{};
        stream->read(reinterpret_cast<char*>(&data), sizeof(T));

        return data;
    }

    template<binary_type T>
    [[nodiscard]] T read(std::size_t size) {}

    template<>
    [[nodiscard]] std::string read(std::size_t size)
    {
        std::string data{};
        data.resize(size);
        stream->read(&data[0], size);

        return data;
    }

    [[nodiscard]] std::uint8_t read_7_bit_encoded_int()
    {
        std::uint8_t result{};
        std::uint8_t bitsRead{};
        std::uint8_t value{};

        do {
            value = read<std::uint8_t>();
            result |= (value & 0x7f) << bitsRead;
            bitsRead += 7;
        } while (value & 0x80);

        return result;
    }

    [[nodiscard]] std::string to_string() const noexcept override
    {
        if (std::istringstream* derived = dynamic_cast<std::istringstream*>(stream.get()); derived != nullptr) {
            return derived->str();
        }

        return std::string{};
    }

    void ignore(std::streamsize count) const noexcept
    {
        stream->ignore(count);
    }
};