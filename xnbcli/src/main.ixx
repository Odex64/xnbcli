import std;
import xnb;
import texture;
import sound;

int main(int argc, char* argv[])
{
    for (std::size_t i{ 1 }; i < argc; i++) {
        std::filesystem::path file{ argv[i] };
        if (!std::filesystem::exists(file)) continue;

        const std::filesystem::path extension{ file.extension() };

        if (extension == ".xnb") {
            std::expected<xnb, std::string> result{ xnb::read_xnb(file) };
            if (result.has_value()) {
                const std::string& type_name{ result->type->reader_name() };
                if (type_name == texture::reader) {
                    std::optional<std::string> error{ result->type->export_file(file.replace_extension(".png")) };
                    if (error.has_value()) {
                        std::println("There was an error while exporting to .png\n{}", error.value());
                    }
                    else {
                        std::println("Successfully exported: {}", file.string());
                    }
                }
                else if (type_name == sound::reader) {
                    std::optional<std::string> error{ result->type->export_file(file.replace_extension(".wav")) };
                    if (error.has_value()) {
                        std::println("There was an error while exporting to .wav\n{}", error.value());
                    }
                    else {
                        std::println("Successfully exported {}", file.string());
                    }
                }
                else {
                    std::println("Unknown header while reading .xnb\n{}", result.error());
                }
            }
            else {
                std::println("Error while reading .xnb\n{}", result.error());
            }
        }
        else if (extension == ".png") {
            std::expected<xnb, std::string> result{ xnb::write_xnb<texture>(file) };
            if (result.has_value()) {
                std::println("Successfully exported: {}", file.string());
            }
            else {
                std::println("There was an error while converting .png to .xnb\n{}", result.error());
            }
        }
        else if (extension == ".wav") {
            std::expected<xnb, std::string> result{ xnb::write_xnb<sound>(file) };
            if (result.has_value()) {
                std::println("Successfully exported: {}", file.string());
            }
            else {
                std::println("There was an error while converting .wav to .xnb\n{}", result.error());
            }
        }
        else {
            std::println("{}", "Unsupported file format");
        }
    }

    std::system("pause");

    return 0;
}