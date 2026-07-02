#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "poly_math.h"
#include "hecc_crypto.h"

using namespace std;

const BigInt p = 999959;
const Poly f = {1, 0, 0, 0, 0, 1};
const Poly h = {0};

Divisor getG()
{
    Divisor G; 
    G.u = {0, 1}; 
    G.v = {1};
    return G;
}

HECC getCurve()
{
    return HECC(f, h, p);
}

void generateKey(BigInt privKey, const string& pubFile)
{
    HECC curve = getCurve();
    Divisor pubKey = curve.scalar_multiply(getG(), privKey);
    
    ofstream out(pubFile);
    out << pubKey.u.size() << " ";
    for(auto x : pubKey.u)
    {
        out << x << " ";
    }
    
    out << pubKey.v.size() << " ";
    for(auto x : pubKey.v)
    {
        out << x << " ";
    }
    
    cout << "[+] Chave Publica HECC salva em: " << pubFile << endl;
}

void encryptFile(const string& pubFile, const string& inFile, const string& outFile)
{
    ifstream pubIn(pubFile);
    if(!pubIn)
    {
        cerr << "Erro ao ler a chave publica." << endl; 
        return; 
    }
    
    Divisor pubKey;
    size_t u_size, v_size;
    
    pubIn >> u_size; 
    pubKey.u.resize(u_size); 
    for(size_t i = 0; i < u_size; ++i)
    {
        pubIn >> pubKey.u[i];
    }
    
    pubIn >> v_size; 
    pubKey.v.resize(v_size); 
    for(size_t i = 0; i < v_size; ++i)
    {
        pubIn >> pubKey.v[i];
    }

    ifstream in(inFile, ios::binary);
    ofstream out(outFile);
    if(!in || !out)
    {
        cerr << "Erro de IO (Entrada/Saida)." << endl; 
        return; 
    }

    string plaintext((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    HECC curve = getCurve();
    BigInt kEf = 789; 
    
    Divisor C1 = curve.scalar_multiply(getG(), kEf);
    Divisor segredo = curve.scalar_multiply(pubKey, kEf);
    BigInt chaveSimetrica = segredo.u[0];

    out << C1.u.size() << " ";
    for(auto x : C1.u)
    {
        out << x << " ";
    }
    out << C1.v.size() << " ";
    for(auto x : C1.v)
    {
        out << x << " ";
    }

    for(char c : plaintext)
    {
        out << mod((unsigned char)c + chaveSimetrica, p) << " ";
    }
    cout << "[+] Arquivo criptografado salvo em: " << outFile << endl;
}

void decryptFile(BigInt privKey, const string& inFile, const string& outFile)
{
    ifstream in(inFile);
    if(!in)
    {
        cerr << "Erro ao ler o criptograma." << endl; 
        return; 
    }

    Divisor C1;
    size_t u_size, v_size;
    
    if(!(in >> u_size))
    {
        return;
    }
    C1.u.resize(u_size); 
    for(size_t i = 0; i < u_size; ++i)
    {
        in >> C1.u[i];
    }
    
    in >> v_size; 
    C1.v.resize(v_size); 
    for(size_t i = 0; i < v_size; ++i)
    {
        in >> C1.v[i];
    }

    vector<BigInt> cipher_text;
    BigInt val;
    while(in >> val)
    {
        cipher_text.push_back(val);
    }

    HECC curve = getCurve();
    Divisor segredo = curve.scalar_multiply(C1, privKey);
    BigInt chaveSimetrica = segredo.u[0];

    ofstream out(outFile, ios::binary);
    for(BigInt c : cipher_text)
    {
        out.put((char)mod(c - chaveSimetrica, p));
    }
    cout << "[+] Arquivo descriptografado salvo em: " << outFile << endl;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << "Modo de Uso:\n"
             << "  Gerar Chave: ./hecc_rede genkey <sua_chave_privada> <nome_chave.pub>\n"
             << "  Cifrar:      ./hecc_rede enc <chave_destino.pub> <texto_claro.txt> <criptograma.dat>\n"
             << "  Decifrar:    ./hecc_rede dec <sua_chave_privada> <criptograma.dat> <texto_recuperado.txt>\n";
        return 1;
    }
    string mode = argv[1];
    if(mode == "genkey" && argc == 4)
    {
        generateKey(stoll(argv[2]), argv[3]);
    }
    else if(mode == "enc" && argc == 5)
    {
        encryptFile(argv[2], argv[3], argv[4]);
    }
    else if(mode == "dec" && argc == 5)
    {
        decryptFile(stoll(argv[2]), argv[3], argv[4]);
    }
    else
    {
        cout << "[-] Argumentos invalidos." << endl;
    }
    return 0;
}