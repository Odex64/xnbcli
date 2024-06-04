export module binary;

import std;

template<typename T>
concept binary_stream = std::derived_from<T, std::ios>;

export template<binary_stream T>
class binary {
protected:
    std::unique_ptr<T> stream;

public:
    explicit binary(std::unique_ptr<T> stream) noexcept : stream{ std::move(stream) } {}
    binary() noexcept = delete;

    binary(const binary&) noexcept = delete;
    binary& operator=(const binary&) noexcept = delete;

    binary(binary&&) noexcept = delete;
    binary& operator=(binary&&) noexcept = delete;

    [[nodiscard]] virtual std::string to_string() const noexcept = 0;
};