#pragma once

namespace Enum
{
    template<typename Enum>
    constexpr bool HasFlag(Enum a, Enum b) noexcept {
        return (a & b) == b;
    }

    // template<typename Enum>
    // constexpr Enum operator|(Enum a, Enum b) noexcept {
    //     using underlying = std::underlying_type_t<Enum>;
    //     return static_cast<Enum>(static_cast<underlying>(a) | static_cast<underlying>(b));
    // }

    // template<typename Enum>
    // constexpr Enum& operator|=(Enum& a, Enum b) noexcept {
    //     return a = a | b;
    // }

    // template<typename Enum>
    // constexpr Enum operator&(Enum a, Enum b) noexcept {
    //     using underlying = std::underlying_type_t<Enum>;
    //     return static_cast<Enum>(static_cast<underlying>(a) & static_cast<underlying>(b));
    // }

    // template<typename Enum>
    // constexpr Enum& operator&=(Enum& a, Enum b) noexcept {
    //     return a = a & b;
    // }

    // template<typename Enum>
    // constexpr Enum operator~(Enum a) noexcept {
    //     using underlying = std::underlying_type_t<Enum>;
    //     return static_cast<Enum>(~static_cast<underlying>(a));
    // }

    // template<typename Enum>
    // constexpr Enum remove(Enum a, Enum b) noexcept {
    //     return a & ~b;
    // }

    // template<typename Enum>
    // constexpr Enum toggle(Enum a, Enum b) noexcept {
    //     return a ^ b;
    // }
}
