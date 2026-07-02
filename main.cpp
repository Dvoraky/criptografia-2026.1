#include <iostream>
#include "hecc_crypto.h"
#include <string>

using namespace std;

int main()
{
    cout << "================================================" << endl;
    cout << "   Trabalho de Criptografia - Parte I: HECC" << endl;
    cout << "================================================\n" << endl;

    BigInt p = 999959;
    Poly f = {1, 0, 0, 0, 0, 1};
    Poly h = {0};
    HECC curva(f, h, p);
    
    Divisor geradorG;
    geradorG.u = {0, 1};
    geradorG.v = {1};

    cout << "--- [ DIFFIE-HELLMAN ] ---" << endl;
    
    BigInt privAlice = 123;
    BigInt privBob = 456;
    
    cout << "[Alice] Chave Privada (a): " << privAlice << endl;
    cout << "[Bob]   Chave Privada (b): " << privBob << endl;
    
    DiffieHellman dhAlice(curva, geradorG, privAlice);
    DiffieHellman dhBob(curva, geradorG, privBob);
    
    Divisor pubAlice = dhAlice.getChavePublica();
    Divisor pubBob = dhBob.getChavePublica();
    
    cout << "\n[TR FEGO DE REDE - CHAVES P BLICAS]" << endl;
    cout << "Alice envia (A): " << pubAlice << endl;
    cout << "Bob envia   (B): " << pubBob << endl;
    
    Divisor segredoAlice = dhAlice.computarSegredo(pubBob);
    Divisor segredoBob = dhBob.computarSegredo(pubAlice);
    
    cout << "\n[COMPUTA O INTERNA - SEGREDO COMPARTILHADO]" << endl;
    cout << "Segredo (Alice): " << segredoAlice << endl;
    cout << "Segredo (Bob):   " << segredoBob << endl;
    
    if(segredoAlice == segredoBob)
    {
        cout << "[+] SUCESSO: O segredo compartilhado (S) bateu!" << endl;
    }
    else
    {
        cout << "[-] ERRO: Diverg ncia no segredo." << endl;
    }

    cout << "\n\n--- [ EL GAMAL ] ---" << endl;
    
    ElGamal egAlice(curva, geradorG, privAlice);
    
    Divisor mensagem;
    mensagem.u = {1, 1};
    mensagem.v = {0};
    
    BigInt kEfemero = 789;
    
    cout << "Mensagem Original (M): " << mensagem << endl;
    cout << "Chave Publica Destino: " << egAlice.getChavePublica() << endl;
    
    pair<Divisor, Divisor> cip = egAlice.cifrar(mensagem, kEfemero, egAlice.getChavePublica());
    
    cout << "\n[TR FEGO DE REDE - CRIPTOGRAMA]" << endl;
    cout << "C1 (Chave Ef mera): " << cip.first << endl;
    cout << "C2 (Carga Cifrada): " << cip.second << endl;
    
    Divisor msgDecifrada = egAlice.decifrar(cip);
    
    cout << "\n[COMPUTA O INTERNA - RECUPERA O]" << endl;
    cout << "Mensagem Decifrada: " << msgDecifrada << endl;
    
    if(mensagem == msgDecifrada)
    {
        cout << "[+] SUCESSO: Mensagem recuperada integralmente!" << endl;
    }
    else
    {
        cout << "[-] ERRO: Falha ao decifrar a mensagem." << endl;
    }

    cout << "\n\n--- [ EL GAMAL - MENSAGEM DE TEXTO ] ---" << endl;
    
    string meuTexto = "Oi, meu nome eh Yohanna Dvorak, tudo bem?";
    cout << "Mensagem Original (String): " << meuTexto << endl;
    
    BigInt kAleatorioTexto = 404;
    
    Divisor C1_texto = curva.scalar_multiply(geradorG, kAleatorioTexto);
    Divisor segredoCifracao = curva.scalar_multiply(pubBob, kAleatorioTexto);
    BigInt chaveSimetricaCifra = segredoCifracao.u[0];
    
    vector<BigInt> textoCifrado;
    for(char c : meuTexto)
    {
        textoCifrado.push_back(mod((int)c + chaveSimetricaCifra, p));
    }
    
    cout << "\n[TR FEGO DE REDE]" << endl;
    cout << "C1 (Ponto na Curva): " << C1_texto << endl;
    cout << "C2 (Texto Cifrado):  [ ";
    for(auto n : textoCifrado)
    {
        cout << n << " ";
    }
    cout << "]" << endl;
    
    Divisor segredoDecifracao = curva.scalar_multiply(C1_texto, privBob);
    BigInt chaveSimetricaDecifra = segredoDecifracao.u[0];
    
    string textoDecifrado = "";
    for(BigInt n : textoCifrado)
    {
        textoDecifrado += (char)mod(n - chaveSimetricaDecifra, p);
    }
    
    cout << "\n[COMPUTA O INTERNA]" << endl;
    cout << "Texto Recuperado: " << textoDecifrado << endl;
    
    if(meuTexto == textoDecifrado)
    {
        cout << "[+] SUCESSO: A string foi criptografada e recuperada!" << endl;
    }
    else
    {
        cout << "[-] ERRO: Falha ao decifrar a string." << endl;
    }
    
    return 0;
}