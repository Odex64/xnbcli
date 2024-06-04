export module binary_type;

import std;

export template <typename T>
concept binary_type = std::integral<T> || std::floating_point<T> || std::same_as<T, std::string>;