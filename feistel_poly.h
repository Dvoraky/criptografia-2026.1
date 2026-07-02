#ifndef FEISTEL_POLY_H
#define FEISTEL_POLY_H
#include <vector>
#include "poly_math.h"

using namespace std;

class FeistelPoly
{
private:
    BigInt p;
    Poly m_x, master_key;
    int rounds;
    vector<Poly> subkeys;
    
    Poly reduceMod(const Poly& A) const
    {
        Poly Q, R; 
        polyDivMod(A, m_x, p, Q, R); 
        return R;
    }
    
    void generateSubkeys()
    {
        subkeys.resize(rounds);
        Poly current = master_key;
        for(int i = 0; i < rounds; i++)
        {
            Poly mixer = {(BigInt)(i + 1), 1}; 
            subkeys[i] = reduceMod(polyMul(current, mixer, p));
            current = subkeys[i];
        }
    }
    
    Poly F(const Poly& R, const Poly& K) const
    {
        return reduceMod(polyMul(polyAdd(polyMul(R, R, p), K, p), {1, 1}, p));
    }
    
public:
    FeistelPoly(BigInt pr, Poly irr, int rd, Poly k) : p(pr), m_x(irr), master_key(k), rounds(rd)
    {
        generateSubkeys();
    }
    
    pair<Poly, Poly> encryptBlock(Poly L, Poly R) const
    {
        for(int i = 0; i < rounds; i++)
        {
            Poly next_L = R;
            Poly next_R = polyAdd(L, F(R, subkeys[i]), p); 
            L = next_L; 
            R = next_R;
        }
        return {L, R};
    }
    
    pair<Poly, Poly> decryptBlock(Poly L, Poly R) const
    {
        for(int i = rounds - 1; i >= 0; i--)
        {
            Poly prev_R = L;
            Poly prev_L = polySub(R, F(L, subkeys[i]), p); 
            L = prev_L; 
            R = prev_R;
        }
        return {L, R};
    }
};
#endif