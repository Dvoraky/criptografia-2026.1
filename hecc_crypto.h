#ifndef HECC_CRYPTO_H
#define HECC_CRYPTO_H
#include <vector>
#include "poly_math.h"

using namespace std;

struct Divisor
{
    Poly u;
    Poly v;
    bool operator==(const Divisor& other) const
    {
        return (u == other.u && v == other.v);
    }
};

inline ostream& operator<<(ostream& os, const Divisor& d)
{
    os << "[ u(x) = " << d.u << "  |  v(x) = " << d.v << " ]";
    return os;
}

class HECC
{
private:
    Poly f, h;
    BigInt p;
public:
    HECC(Poly f_poly, Poly h_poly, BigInt prime) : f(f_poly), h(h_poly), p(prime) {}
    
    bool isIdentity(const Divisor& D) const
    {
        return (D.u.size() == 1 && D.u[0] == 1 && D.v.size() == 1 && D.v[0] == 0);
    }
    
    Divisor negate(const Divisor& D) const
    {
        Divisor D_inv;
        D_inv.u = D.u;
        D_inv.v = polySub(polySub({0}, D.v, p), h, p);
        return D_inv;
    }
    
    Divisor add(const Divisor& D1, const Divisor& D2) const
    {
        if(isIdentity(D1))
        {
            return D2;
        }
        if(isIdentity(D2))
        {
            return D1;
        }
        
        Poly neg_v2 = negate(D2).v;
        if(D1.u == D2.u && D1.v == neg_v2)
        {
            return {{1}, {0}};
        }
        
        Poly e1, e2, d1, c1, c2, d;
        d1 = polyExtendedGCD(D1.u, D2.u, p, e1, e2);
        Poly v_sum = polyAdd(polyAdd(D1.v, D2.v, p), h, p);
        d = polyExtendedGCD(d1, v_sum, p, c1, c2);
        Poly s1 = polyMul(c1, e1, p), s2 = polyMul(c1, e2, p), s3 = c2;
        Poly u1_u2 = polyMul(D1.u, D2.u, p), d_sq = polyMul(d, d, p), u, R_temp;
        polyDivMod(u1_u2, d_sq, p, u, R_temp);
        Poly term1 = polyMul(polyMul(s1, D1.u, p), D2.v, p);
        Poly term2 = polyMul(polyMul(s2, D2.u, p), D1.v, p);
        Poly v1_v2_f = polyAdd(polyMul(D1.v, D2.v, p), f, p);
        Poly num = polyAdd(polyAdd(term1, term2, p), polyMul(s3, v1_v2_f, p), p);
        Poly v_div_d, v, Q_temp;
        polyDivMod(num, d, p, v_div_d, R_temp);
        polyDivMod(v_div_d, u, p, Q_temp, v);
        
        int g = 2;
        while(degree(u) > g)
        {
            Poly v_h = polyMul(v, h, p), v_sq = polyMul(v, v, p);
            Poly num_u_new = polySub(polySub(f, v_h, p), v_sq, p);
            Poly u_new, v_new;
            polyDivMod(num_u_new, u, p, u_new, R_temp);
            polyDivMod(polySub(polySub({0}, h, p), v, p), u_new, p, Q_temp, v_new);
            u = u_new;
            v = v_new;
        }
        
        if(!u.empty() && u.back() != 0)
        {
            BigInt invLider = modInverse(u.back(), p);
            for(size_t i = 0; i < u.size(); ++i)
            {
                u[i] = mod(u[i] * invLider, p);
            }
        }
        return {u, v};
    }
    
    Divisor scalar_multiply(const Divisor& D, BigInt k) const
    {
        Divisor R = {{1}, {0}}, base = D;
        while(k > 0)
        {
            if(k % 2 == 1)
            {
                R = add(R, base);
            }
            base = add(base, base);
            k /= 2;
        }
        return R;
    }
};

class DiffieHellman
{
private:
    HECC curva; 
    Divisor gerador; 
    BigInt chavePrivada;
public:
    DiffieHellman(HECC c, Divisor g, BigInt pK) : curva(c), gerador(g), chavePrivada(pK) {}
    
    Divisor getChavePublica()
    {
        return curva.scalar_multiply(gerador, chavePrivada);
    }
    
    Divisor computarSegredo(Divisor pub)
    {
        return curva.scalar_multiply(pub, chavePrivada);
    }
};

class ElGamal
{
private:
    HECC curva; 
    Divisor gerador; 
    BigInt chavePrivada; 
    Divisor chavePublica;
public:
    ElGamal(HECC c, Divisor g, BigInt pK) : curva(c), gerador(g), chavePrivada(pK)
    {
        chavePublica = curva.scalar_multiply(gerador, chavePrivada);
    }
    
    Divisor getChavePublica()
    {
        return chavePublica;
    }
    
    pair<Divisor, Divisor> cifrar(Divisor msg, BigInt kEf, Divisor pubDest)
    {
        return {curva.scalar_multiply(gerador, kEf), curva.add(msg, curva.scalar_multiply(pubDest, kEf))};
    }
    
    Divisor decifrar(pair<Divisor, Divisor> cript)
    {
        return curva.add(cript.second, curva.negate(curva.scalar_multiply(cript.first, chavePrivada)));
    }
};
#endif