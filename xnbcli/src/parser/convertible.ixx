export module convertible;

import std;
import converter;

export template<typename T>
concept convertible = std::derived_from<T, converter>;