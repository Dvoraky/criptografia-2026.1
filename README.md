# Criptografia em C++: HECC e Feistel_Fp

Este repositório contém o meu trabalho prático final da disciplina de **Tópicos Avançados em Ciência da Computação VI (Introdução à Criptografia)** (TCC00230) da Universidade Federal Fluminense (UFF).

O projeto consiste na implementação integral de dois ecossistemas matemáticos complexos:

*   **Parte I (Criptografia Assimétrica):** Implementação do Algoritmo de Cantor sobre a Variedade Jacobiana de Curvas Hiperelípticas (HECC) de gênero 2. Inclui os protocolos Diffie-Hellman e ElGamal (KEM/DEM).
*   **Parte II (Criptografia Simétrica):** Criação de uma cifra, `Feistel_Fp`, que utiliza uma Rede de Feistel operando exclusivamente com aritmética de polinômios em corpos finitos, eliminando matrizes estáticas e S-Boxes.

## Como Compilar (Makefile)

O projeto acompanha um `Makefile` configurado para compilar todos os utilitários de teste com máxima otimização (`-O3`).

No terminal, basta rodar:
```bash
make
