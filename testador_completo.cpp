#include <iostream>
#include <vector>
#include <string>
#include "poly_math.h"
#include "hecc_crypto.h"
#include "feistel_poly.h"

using namespace std;

const string GREEN = "\033[32m";
const string RED = "\033[31m";
const string RESET = "\033[0m";

void runTest(const string& testName, bool passed)
{
    cout << "[-] Teste: " << testName;
    int dots = 60 - testName.length();
    for(int i = 0; i < dots; ++i)
    {
        cout << ".";
    }
    if(passed)
    {
        cout << GREEN << " [OK]" << RESET << endl;
    }
    else
    {
        cout << RED << " [ERRO]" << RESET << endl;
    }
}

int main()
{
    cout << "============================================================" << endl;
    cout << "      SUITE DE TESTES AUTOMATIZADOS - CRIPTOGRAFIA UFF      " << endl;
    cout << "============================================================\n" << endl;

    BigInt p = 999959;
    Poly f = {1, 0, 0, 0, 0, 1}; 
    Poly h = {0};
    HECC curva(f, h, p);
    
    Divisor G; 
    G.u = {0, 1}; 
    G.v = {1};    

    cout << ">>> PARTE I: MATEMATICA E CHAVE PUBLICA (HECC)" << endl;

    Divisor negG = curva.negate(G);
    Divisor resultIdentidade = curva.add(G, negG);
    runTest("Algoritmo Cantor: G + (-G) == Identidade", curva.isIdentity(resultIdentidade));

    Divisor D1 = curva.scalar_multiply(G, 5);
    Divisor D2 = curva.scalar_multiply(G, 7);
    Divisor soma1 = curva.add(D1, D2);
    Divisor soma2 = curva.add(D2, D1);
    runTest("Algoritmo Cantor: Comutatividade (D1+D2 == D2+D1)", soma1 == soma2);

    Divisor multEscalar = curva.scalar_multiply(G, 2);
    Divisor somaDireta = curva.add(G, G);
    runTest("Multiplicacao Escalar: 2*G == G + G", multEscalar == somaDireta);

    DiffieHellman dhAlice(curva, G, 123); 
    DiffieHellman dhBob(curva, G, 456);   
    
    Divisor pubAlice = dhAlice.getChavePublica();
    Divisor pubBob = dhBob.getChavePublica();
    
    Divisor segredoAlice = dhAlice.computarSegredo(pubBob);
    Divisor segredoBob = dhBob.computarSegredo(pubAlice);
    runTest("Diffie-Hellman: Segredos derivados sao iguais", segredoAlice == segredoBob);

    ElGamal egBob(curva, G, 999); 
    Divisor mensagemOriginal = curva.scalar_multiply(G, 42); 
    BigInt kEfemero = 789;
    pair<Divisor, Divisor> criptograma = egBob.cifrar(mensagemOriginal, kEfemero, egBob.getChavePublica());
    Divisor mensagemRecuperada = egBob.decifrar(criptograma);
    
    runTest("ElGamal: Decifracao recupera divisor exato", mensagemRecuperada == mensagemOriginal);

    cout << "\n>>> PARTE II: CRIPTOSSISTEMA SIMETRICO (Feistel_Fp)" << endl;
    
    Poly m_x = {1, 0, 0, 0, 1}; 
    int rounds = 16;
    Poly masterKey = {99, 88, 77, 66, 55};
    FeistelPoly cifraSimetrica(p, m_x, rounds, masterKey);

    Poly L_in = {10, 20, 30, 40};
    Poly R_in = {50, 60, 70, 80};

    pair<Poly, Poly> cipherBlock = cifraSimetrica.encryptBlock(L_in, R_in);
    pair<Poly, Poly> plainBlock = cifraSimetrica.decryptBlock(cipherBlock.first, cipherBlock.second);
    bool blockRecovered = (L_in == plainBlock.first && R_in == plainBlock.second);
    runTest("Feistel_Fp: Cifracao e Decifracao de Bloco Reversivel", blockRecovered);

    pair<Poly, Poly> cipherBlock2 = cifraSimetrica.encryptBlock(L_in, R_in);
    bool isDeterministic = (cipherBlock.first == cipherBlock2.first && cipherBlock.second == cipherBlock2.second);
    runTest("Feistel_Fp: Cifra eh perfeitamente deterministica", isDeterministic);

    Poly L_in_mutado = {11, 20, 30, 40}; 
    pair<Poly, Poly> cipherBlockMutado = cifraSimetrica.encryptBlock(L_in_mutado, R_in);
    bool avalancheFuncionou = (cipherBlock.first != cipherBlockMutado.first || cipherBlock.second != cipherBlockMutado.second);
    runTest("Feistel_Fp: Efeito Avalanche detecta alteracoes minimas", avalancheFuncionou);

    string textoOriginal = "Oi meu nome eh Yohanna e eu amo gatos";
    string textoCopia = textoOriginal;
    
    int espacosFaltando = 8 - (textoCopia.length() % 8);
    if(espacosFaltando != 8)
    {
        for(int i = 0; i < espacosFaltando; i++)
        {
            textoCopia += " ";
        }
    }

    vector<pair<Poly, Poly>> blocosCifrados;
    
    for(size_t i = 0; i < textoCopia.length(); i += 8)
    {
        Poly L(4, 0), R(4, 0);
        for(int j = 0; j < 4; j++)
        {
            L[j] = (unsigned char)textoCopia[i + j];
        }
        for(int j = 0; j < 4; j++)
        {
            R[j] = (unsigned char)textoCopia[i + 4 + j];
        }
        blocosCifrados.push_back(cifraSimetrica.encryptBlock(L, R));
    }

    string textoRecuperado = "";
    for(auto bloco : blocosCifrados)
    {
        pair<Poly, Poly> decifrado = cifraSimetrica.decryptBlock(bloco.first, bloco.second);
        for(int j = 0; j < 4; j++)
        {
            textoRecuperado += (char)decifrado.first[j];
        }
        for(int j = 0; j < 4; j++)
        {
            textoRecuperado += (char)decifrado.second[j];
        }
    }
    
    bool stringRecovered = (textoCopia == textoRecuperado);
    runTest("Feistel_Fp: Cifracao e Decifracao de String Real", stringRecovered);

    cout << "\n============================================================" << endl;
    cout << "                    TESTES FINALIZADOS                      " << endl;
    cout << "============================================================" << endl;

    return 0;
}