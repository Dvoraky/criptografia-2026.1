# Criptografia em C++: HECC e Feistel_Fp

Este repositório contém o trabalho prático final da disciplina de **Tópicos Especiais em Segurança da Informação** (TCC00230) da Universidade Federal Fluminense (UFF), desenvolvido pela aluna Yohanna Dvorak.

O projeto consiste na implementação integral de dois ecossistemas matemáticos complexos:

*   **Parte I (Criptografia Assimétrica):** Implementação do Algoritmo de Cantor sobre a Variedade Jacobiana de Curvas Hiperelípticas (HECC) de gênero 2. Inclui os protocolos Diffie-Hellman e ElGamal (KEM/DEM).
*   **Parte II (Criptografia Simétrica):** Criação de uma cifra, `Feistel_Fp`, que utiliza uma Rede de Feistel operando exclusivamente com aritmética de polinômios em corpos finitos, eliminando matrizes estáticas e S-Boxes.

## Como Compilar (Makefile)

O projeto acompanha um `Makefile` configurado para compilar todos os utilitários de teste com máxima otimização (`-O3`).

No terminal, basta rodar:
```bash
make
