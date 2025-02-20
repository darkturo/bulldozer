#pragma once

namespace bulldozer {
    template<typename T>
    concept ByteType = requires(T a) {
        { a } -> std::convertible_to<uint8_t>;
    };
}