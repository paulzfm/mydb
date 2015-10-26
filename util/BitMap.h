#ifndef UTIL_BIT_MAP_H_
#define UTIL_BIT_MAP_H_

#include <vector>

#define SHIFT 5
#define MASK 0x1F

// Assume that the size of bit array is divided by 32 bits.
class BitMap
{
public:
    // set the specified bit as 1
    static void set(int *bits, int index)
    {
        bits[index >> SHIFT] = bits[index >> SHIFT] | (1 << (index & MASK));
    }

    // set the specified bit as 0
    static void reset(int *bits, int index)
    {
        bits[index >> SHIFT] = bits[index >> SHIFT] & (~(1 << (index & MASK)));
    }

    // test whether the specified bit is 1
    static bool test(int *bits, int index)
    {
        return bits[index >> SHIFT] & (1 << (index & MASK));
    }

    // find the first bit who is 1
    static int findOne(int *bits, int max)
    {
        for (int i = 0; i < max; i++) {
            if (bits[i >> SHIFT] == 0) {
                i <<= SHIFT;
            } else if (test(bits, i)) {
                return i;
            }
        }

        return -1;
    }

    // find all bits who is 0
    static void findZeros(int *bits, int max, std::vector<int>& v)
    {
        for (int i = 0; i < max; i++) {
            if (!test(bits, i)) {
                v.push_back(i);
            }
        }
    }
};

#endif // UTIL_BIT_MAP_H_
