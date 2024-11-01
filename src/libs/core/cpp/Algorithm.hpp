#pragma once

template <class T> 
const T& Max(const T& a, const T& b) 
{
    return (a < b) ? b : a;
}

template <class T> 
const T& Min(const T& a, const T& b) 
{
    return (a < b) ? a : b;
}

template <class T>
const T& Clamp(const T& value, const T& min, const T& max)
{
    if (value < min)
        return min;
    if (max < value)
        return max;
    return value;
}