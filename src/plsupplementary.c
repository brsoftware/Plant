#include <math.h>

#include <PlSupplementary>

double pl_combination(double n, double r)
{
    if (n < r || r < 0 || (int)n != n || (int)r != r)
        return NAN;

    if (n == r || n == 0)
        return 1;

    return trunc(pl_multiplier(fmax(r, n - r) + 1, n) / pl_multiplier(1, fmin(r, n - r)));
}

double pl_factorial(double n)
{
    return trunc(pl_multiplier(1, n));
}

double pl_multiplier(double s, double e)
{
    if (s == e)
        return s;

    double rst = 1;

    while (s <= e)
    {
        rst *= s;
        s++;
    }

    return rst;
}

double pl_permutation(double n, double r)
{
    if (n < r || r < 0 || (int)n != n || (int)r != r)
        return NAN;

    if (n == r)
        return pl_factorial(n);

    return trunc(pl_multiplier(1, n) / pl_multiplier(1, n - r));
}
