export module converter;

import std;

export class converter {
public:
    virtual std::optional<std::string> export_file(const std::filesystem::path& file) const noexcept = 0;
    virtual [[nodiscard]] std::string to_binary() const noexcept = 0;
    virtual [[nodiscard]] const std::string& reader_name() const noexcept = 0;
    virtual ~converter() noexcept = default;
};