#pragma once

#include <string>

#include "utils/Common.h"

static inline uint32_t hash(const void *object, size_t size)
{
    uint32_t hash = 5381;

    for (size_t i = 0; i < size; i++)
    {
        hash = ((hash << 5) + hash) + ((const uint8_t *)object)[i];
    }

    return hash;
}

template <typename TObject>
constexpr bool has_hash_function()
{
    return false;
}

template <typename TObject>
inline uint32_t hash(const TObject &)
{
    static_assert(has_hash_function<TObject>(), "The type don't define an hash() function.");
    return 0;
}

template <>
inline uint32_t hash<uint32_t>(const uint32_t &value)
{
    return hash(&value, sizeof(value));
}

template <>
inline uint32_t hash<std::string>(const std::string &value)
{
    return hash(reinterpret_cast<const void *>(value.c_str()), value.length());
}