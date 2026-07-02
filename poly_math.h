#ifndef POLY_MATH_H
#define POLY_MATH_H
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;

typedef long long BigInt;
typedef vector<BigInt> Poly;

inline BigInt mod(BigInt a, BigInt p)
{
    BigInt r = a % p;
    return r < 0 ? r + p : r;
}

inline BigInt modInverse(BigInt a, BigInt m)
{
    BigInt m0 = m, y = 0, x = 1;
    if(m == 1)
    {
        return 0;
    }
    while(a > 1)
    {
        BigInt q = a / m;
        BigInt t = m;
        m = a % m, a = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    if(x < 0)
    {
        x += m0;
    }
    return x;
}

inline void clean(Poly& poly)
{
    while(poly.size() > 1 && poly.back() == 0)
    {
        poly.pop_back();
    }
    if(poly.empty())
    {
        poly.push_back(0);
    }
}

inline int degree(const Poly& poly)
{
    if(poly.size() == 1 && poly[0] == 0)
    {
        return -1;
    }
    return poly.size() - 1;
}

inline Poly polyAdd(const Poly& a, const Poly& b, BigInt p)
{
    Poly res(max(a.size(), b.size()), 0);
    for(size_t i = 0; i < res.size(); ++i)
    {
        BigInt valA = (i < a.size()) ? a[i] : 0;
        BigInt valB = (i < b.size()) ? b[i] : 0;
        res[i] = mod(valA + valB, p);
    }
    clean(res);
    return res;
}

inline Poly polySub(const Poly& a, const Poly& b, BigInt p)
{
    Poly res(max(a.size(), b.size()), 0);
    for(size_t i = 0; i < res.size(); ++i)
    {
        BigInt valA = (i < a.size()) ? a[i] : 0;
        BigInt valB = (i < b.size()) ? b[i] : 0;
        res[i] = mod(valA - valB, p);
    }
    clean(res);
    return res;
}

inline Poly polyMul(const Poly& a, const Poly& b, BigInt p)
{
    if(a.empty() || (a.size() == 1 && a[0] == 0) || b.empty() || (b.size() == 1 && b[0] == 0))
    {
        return {0};
    }
    Poly res(a.size() + b.size() - 1, 0);
    for(size_t i = 0; i < a.size(); ++i)
    {
        for(size_t j = 0; j < b.size(); ++j)
        {
            res[i + j] = mod(res[i + j] + mod(a[i] * b[j], p), p);
        }
    }
    clean(res);
    return res;
}

inline void polyDivMod(const Poly& A, const Poly& B, BigInt p, Poly& Q, Poly& R)
{
    R = A;
    Q.clear();
    clean(R);
    Poly bClean = B; 
    clean(bClean);
    int degB = degree(bClean);
    if(degB < 0)
    {
        throw runtime_error("Error");
    }
    int degR = degree(R);
    if(degR < degB)
    {
        Q = {0};
        return;
    }
    Q.assign(degR - degB + 1, 0);
    BigInt invB = modInverse(bClean.back(), p);
    while(degree(R) >= degB)
    {
        degR = degree(R);
        int shift = degR - degB;
        BigInt coef = mod(R.back() * invB, p);
        Q[shift] = coef;
        for(size_t i = 0; i < bClean.size(); ++i)
        {
            R[i + shift] = mod(R[i + shift] - mod(coef * bClean[i], p), p);
        }
        clean(R);
    }
    clean(Q);
}

inline Poly polyExtendedGCD(Poly a, Poly b, BigInt p, Poly& x, Poly& y)
{
    if(a.empty() || (a.size() == 1 && a[0] == 0))
    {
        x = {0};
        y = {1};
        return b;
    }
    if(b.empty() || (b.size() == 1 && b[0] == 0))
    {
        x = {1};
        y = {0};
        return a;
    }
    Poly x1 = {1}, y1 = {0};
    Poly x2 = {0}, y2 = {1};
    Poly q, r;
    while(degree(b) >= 0 && (b.size() > 1 || b[0] != 0))
    {
        polyDivMod(a, b, p, q, r);
        a = b;
        b = r;
        Poly temp_x = polySub(x1, polyMul(q, x2, p), p);
        Poly temp_y = polySub(y1, polyMul(q, y2, p), p);
        x1 = x2;
        y1 = y2;
        x2 = temp_x;
        y2 = temp_y;
        clean(b);
    }
    if(degree(a) >= 0 && a.back() != 0)
    {
        BigInt inv = modInverse(a.back(), p);
        for(size_t i = 0; i < a.size(); ++i)
        {
            a[i] = mod(a[i] * inv, p);
        }
        for(size_t i = 0; i < x1.size(); ++i)
        {
            x1[i] = mod(x1[i] * inv, p);
        }
        for(size_t i = 0; i < y1.size(); ++i)
        {
            y1[i] = mod(y1[i] * inv, p);
        }
    }
    clean(a);
    clean(x1);
    clean(y1);
    x = x1;
    y = y1;
    return a;
}

inline ostream& operator<<(ostream& os, const Poly& p)
{
    if(p.empty() || (p.size() == 1 && p[0] == 0))
    {
        os << "0";
        return os;
    }
    bool primeiro = true;
    for(int i = p.size() - 1; i >= 0; --i)
    {
        if(p[i] == 0)
        {
            continue;
        }
        if(!primeiro)
        {
            os << " + ";
        }
        if(p[i] != 1 || i == 0)
        {
            os << p[i];
        }
        if(i > 0)
        {
            os << "x";
        }
        if(i > 1)
        {
            os << "^" << i;
        }
        primeiro = false;
    }
    return os;
}
#endif