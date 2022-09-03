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