
#include <stdio.h>
#include <string.h>
#include "arquivos.h"

#define ARQUIVO_CONTAS "data/contas.txt"
#define ARQUIVO_TRANSACOES "data/transacoes.txt"

int carregar_contas(Conta contas[], int capacidade) {
    FILE *arquivo = fopen(ARQUIVO_CONTAS, "r");

    if (arquivo == NULL) {
        return 0;
    }

    int quantidade = 0;
    Conta conta;

    while (quantidade < capacidade &&
           fscanf(arquivo, "%d|%49[^|]|%lld",
                  &conta.numero,
                  conta.titular,
                  &conta.saldo_centavos) == 3) {

        if (conta.numero <= 0 ||
            conta.saldo_centavos < 0 ||
            strchr(conta.titular, '\n') != NULL) {
            continue;
        }

        contas[quantidade++] = conta;
    }

    fclose(arquivo);
    return quantidade;
}

int salvar_contas(const Conta contas[], int quantidade) {
    FILE *arquivo = fopen(ARQUIVO_CONTAS, "w");

    if (arquivo == NULL) {
        return 0;
    }

    for (int i = 0; i < quantidade; i++) {
        if (fprintf(arquivo, "%d|%s|%lld\n",
                    contas[i].numero,
                    contas[i].titular,
                    contas[i].saldo_centavos) < 0) {
            fclose(arquivo);
            return 0;
        }
    }

    if (fclose(arquivo) != 0) {
        return 0;
    }

    return 1;
}

int carregar_transacoes(
    Transacao transacoes[],
    int capacidade
) {
    FILE *arquivo = fopen(ARQUIVO_TRANSACOES, "r");

    if (arquivo == NULL) {
        return 0;
    }

    int quantidade = 0;
    Transacao transacao;

    while (quantidade < capacidade &&
           fscanf(arquivo, "%d|%d|%14[^|]|%lld|%29[^\n]",
                  &transacao.id,
                  &transacao.numero_conta,
                  transacao.tipo,
                  &transacao.valor_centavos,
                  transacao.data) == 5) {

        if (transacao.id <= 0 ||
            transacao.numero_conta <= 0 ||
            transacao.valor_centavos <= 0) {
            continue;
        }

        transacoes[quantidade++] = transacao;
    }

    fclose(arquivo);
    return quantidade;
}

int salvar_transacao(const Transacao *transacao) {
    if (transacao == NULL) {
        return 0;
    }

    FILE *arquivo = fopen(ARQUIVO_TRANSACOES, "a");

    if (arquivo == NULL) {
        return 0;
    }

    int resultado = fprintf(
        arquivo, "%d|%d|%s|%lld|%s\n",
        transacao->id,
        transacao->numero_conta,
        transacao->tipo,
        transacao->valor_centavos,
        transacao->data
    );

    if (resultado < 0) {
        fclose(arquivo);
        return 0;
    }

    return fclose(arquivo) == 0;
}
