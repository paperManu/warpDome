#include <iostream>
#include <vector>

/*************/
template <typename T>
struct Point
{
    Point() {}
    Point(T i, T j)
    {
        x = i;
        y = j;
    }

    T x;
    T y;
};
