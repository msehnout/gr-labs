#ifndef UTIL_H
#define UTIL_H

#include <vector>

/* We can use a function like this to print some GL capabilities of our adapter
to the log file. handy if we want to debug problems on other people's computers
*/
void logGLParameters();

template<typename T>
std::vector<T> slice(const std::vector<T>& v, int start = 0, int end = -1)
{
    int oldlen = v.size();
    int newlen;

    if (end == -1 && end >= oldlen)
    {
        newlen = oldlen - start;
    }
    else
    {
        newlen = end - start;
    }

    std::vector<T> nv(newlen);

    for (int i = 0; i < newlen; i++) {
        nv[i] = v[start + i];
    }
    return nv;
}

#endif