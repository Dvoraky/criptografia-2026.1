#include <iostream>
#include <chrono>
#include "poly_math.h"
#include "feistel_poly.h"

using namespace std;
using namespace std::chrono;

int main()
{
    cout << "================================================" << endl;
    cout << "  Trabalho Criptografia - Parte II: Feistel_Fp" << endl;
    cout << "================================================\n" << endl;

    BigInt p = 999959;
    int rounds = 16;
    Poly m_x = {1, 0, 0, 0, 1};
    Poly masterKey = {99, 88, 77, 66, 55};
    FeistelPoly cifra(p, m_x, rounds, masterKey);
    
    Poly L_in = {10, 20, 30, 40};
    Poly R_in = {50, 60, 70, 80};
    
    cout << "--- [ TESTE DE FUNCIONAMENTO ] ---" << endl;
    cout << "Texto Claro (L): " << L_in << endl;
    cout << "Texto Claro (R): " << R_in << endl;
    
    pair<Poly, Poly> ciphertext = cifra.encryptBlock(L_in, R_in);
    
    cout << "\n[Criptograma Gerado]" << endl;
    cout << "Cifrado (L): " << ciphertext.first << endl;
    cout << "Cifrado (R): " << ciphertext.second << endl;
    
    pair<Poly, Poly> decrypted = cifra.decryptBlock(ciphertext.first, ciphertext.second);
    
    cout << "\n[Recupera o de Dados]" << endl;
    cout << "Decifrado (L): " << decrypted.first << endl;
    cout << "Decifrado (R): " << decrypted.second << endl;
    
    if(L_in == decrypted.first && R_in == decrypted.second)
    {
        cout << "[+] SUCESSO: O bloco foi revertido perfeitamente!" << endl;
    }
    else
    {
        cout << "[-] ERRO: Perda de dados na decifra o." << endl;
    }

    cout << "\n--- [ TESTE DE ESTRESSE E VELOCIDADE ] ---" << endl;
    const int iteracoes = 10000;
    
    auto start = high_resolution_clock::now();
    
    for(int i = 0; i < iteracoes; i++)
    {
        cifra.encryptBlock(L_in, R_in);
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    
    cout << "Tempo para cifrar " << iteracoes << " blocos: " << duration.count() << " ms" << endl;
    
    double bytes_totais = iteracoes * 64.0;
    double megabytes = bytes_totais / (1024 * 1024);
    double segundos = duration.count() / 1000.0;
    
    if(segundos > 0)
    {
        cout << "Velocidade estimada: " << (megabytes / segundos) << " MB/s" << endl;
    }
    
    return 0;
}