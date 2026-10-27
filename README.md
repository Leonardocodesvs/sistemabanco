
# Sistema Bancário em C

Projeto educacional desenvolvido em linguagem C para praticar
programacao estruturada, modularizacao, validacao de entradas
e manipulacao de arquivos.

## Funcionalidades

- Criacao de contas
- Listagem de contas
- Consulta de saldo
- Depositos
- Saques com validacao de saldo
- Historico de transacoes
- Persistencia local em arquivos de texto

## Tecnologias

- Linguagem C
- GCC
- Git e GitHub
- Terminal do Windows

## Estrutura

```text
SistemaBancario/
├── src/
│   ├── main.c
│   ├── banco.c
│   ├── banco.h
│   ├── arquivos.c
│   └── arquivos.h
├── tests/
│   └── testes.c
├── docs/
│   └── SEGURANCA.md
├── data/
├── .gitignore
├── LICENSE
└── README.md
```

## Como compilar no Windows

Abra o terminal na pasta raiz do projeto e execute:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic src/main.c src/banco.c src/arquivos.c -o sistema_bancario.exe
```

Para executar:

```powershell
.\sistema_bancario.exe
```

E necessario ter o GCC instalado e disponivel no PATH.

## Armazenamento

Os registros sao salvos localmente na pasta `data/`.
Essa pasta e ignorada pelo Git para evitar a publicacao
acidental dos dados locais.

## Seguranca e limitacoes

Este projeto tem finalidade exclusivamente educacional.
Os arquivos de dados nao sao criptografados e nao possuem
garantias de integridade ou protecao contra alteracoes manuais.

O projeto nao implementa autenticacao, autorizacao,
controle de concorrencia ou transacoes atomicas.

Nao utilize dados reais, senhas, informacoes bancarias
ou dinheiro real neste sistema.

Consulte [docs/SEGURANCA.md](docs/SEGURANCA.md).

## Objetivo

Praticar conceitos de C e construir um projeto organizado
para estudo e portfolio.
