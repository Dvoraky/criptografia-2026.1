#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "poly_math.h"
#include "feistel_poly.h"

using namespace std;

void processFile(const string& inputFile, const string& outputFile, bool encrypt)
{
    BigInt p = 999959; 
    int rounds = 16;       
    Poly m_x = {1, 0, 0, 0, 1}; 
    Poly masterKey = {99, 88, 77, 66, 55};
    FeistelPoly cifra(p, m_x, rounds, masterKey);
    
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    
    if(!in.is_open() || !out.is_open())
    {
        cerr << "Erro ao abrir os arquivos!" << endl;
        return;
    }
    
    if(encrypt)
    {
        char buffer[8];
        while(in.read(buffer, 8) || in.gcount() > 0)
        {
            int bytesRead = in.gcount();
            for(int i = bytesRead; i < 8; i++)
            {
                buffer[i] = 0;
            }
            
            Poly L(4, 0), R(4, 0);
            for(int i = 0; i < 4; i++)
            {
                L[i] = (unsigned char)buffer[i];
            }
            for(int i = 0; i < 4; i++)
            {
                R[i] = (unsigned char)buffer[i + 4];
            }
            
            pair<Poly, Poly> ciphered = cifra.encryptBlock(L, R);
            
            for(BigInt val : ciphered.first)
            {
                out << val << " ";
            }
            for(BigInt val : ciphered.second)
            {
                out << val << " ";
            }
        }
        cout << "[+] Arquivo criptografado com sucesso para: " << outputFile << endl;
    }
    else
    {
        Poly L(4, 0), R(4, 0);
        BigInt val;
        while(in >> val)
        {
            L[0] = val;
            for(int i = 1; i < 4; i++)
            {
                in >> L[i];
            }
            for(int i = 0; i < 4; i++)
            {
                in >> R[i];
            }
            
            pair<Poly, Poly> decrypted = cifra.decryptBlock(L, R);
            
            for(int i = 0; i < 4; i++)
            {
                if(decrypted.first[i] != 0)
                {
                    out.put((char)decrypted.first[i]);
                }
            }
            for(int i = 0; i < 4; i++)
            {
                if(decrypted.second[i] != 0)
                {
                    out.put((char)decrypted.second[i]);
                }
            }
        }
        cout << "[+] Arquivo descriptografado com sucesso para: " << outputFile << endl;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        cout << "Uso: ./cifra_arquivo [enc|dec] <entrada> <saida>" << endl;
        return 1;
    }
    
    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    
    if(mode == "enc")
    {
        processFile(inputFile, outputFile, true);
    }
    else if(mode == "dec")
    {
        processFile(inputFile, outputFile, false);
    }
    else
    {
        cout << "Modo invalido. Use 'enc' ou 'dec'." << endl;
    }
    
    return 0;
}