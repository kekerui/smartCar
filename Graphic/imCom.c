#include "imCom.h"

imINT32 int_min(imINT32 a,imINT32 b)
{
    if (a < b)
        return a;
    else 
        return b;
}

imINT32 int_max(imINT32 a,imINT32 b)
{
    if (a > b)
        return a;
    else 
        return b;
}

imINT32 int_abs(imINT32 a)
{
    if(a >= 0)
        return a;
    else
        return -a;
}