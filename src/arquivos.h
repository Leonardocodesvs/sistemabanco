
#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include "banco.h"

int carregar_contas(Conta contas[], int capacidade);
int salvar_contas(const Conta contas[], int quantidade);

int carregar_transacoes(
    Transacao transacoes[],
    int capacidade
);

int salvar_transacao(const Transacao *transacao);

#endif
