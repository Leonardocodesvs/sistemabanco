
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "banco.h"
#include "arquivos.h"

static Conta contas[MAX_CONTAS];
static int quantidade_contas = 0;

static void formatar_dinheiro(long long centavos) {
    printf("R$ %lld,%02lld",
           centavos / 100,
           centavos % 100);
}

static int proximo_numero(void) {
    int maior = 0;

    for (int i = 0; i < quantidade_contas; i++) {
        if (contas[i].numero > maior) {
            maior = contas[i].numero;
        }
    }

    if (maior == INT_MAX) {
        return 0;
    }

    return maior + 1;
}

static int proximo_id_transacao(void) {
    Transacao transacoes[MAX_TRANSACOES];
    int quantidade = carregar_transacoes(
        transacoes, MAX_TRANSACOES
    );
    int maior = 0;

    for (int i = 0; i < quantidade; i++) {
        if (transacoes[i].id > maior) {
            maior = transacoes[i].id;
        }
    }

    return maior == INT_MAX ? 0 : maior + 1;
}

static int registrar_transacao(
    int numero,
    const char *tipo,
    long long valor
) {
    Transacao transacao = {0};
    time_t agora = time(NULL);
    struct tm *tempo = localtime(&agora);

    if (tempo == NULL) {
        return 0;
    }

    transacao.id = proximo_id_transacao();

    if (transacao.id == 0) {
        return 0;
    }

    transacao.numero_conta = numero;
    transacao.valor_centavos = valor;

    if (snprintf(transacao.tipo,
                 sizeof(transacao.tipo),
                 "%s", tipo) >= (int)sizeof(transacao.tipo)) {
        return 0;
    }

    if (strftime(transacao.data,
                 sizeof(transacao.data),
                 "%Y-%m-%d %H:%M:%S",
                 tempo) == 0) {
        return 0;
    }

    return salvar_transacao(&transacao);
}

void inicializar_banco(void) {
    quantidade_contas = carregar_contas(
        contas, MAX_CONTAS
    );

    printf("Sistema inicializado. Contas carregadas: %d\n",
           quantidade_contas);
}

int buscar_conta(int numero) {
    for (int i = 0; i < quantidade_contas; i++) {
        if (contas[i].numero == numero) {
            return i;
        }
    }

    return -1;
}

int criar_conta(const char *titular) {
    if (titular == NULL ||
        quantidade_contas >= MAX_CONTAS) {
        printf("Nao foi possivel criar a conta.\n");
        return 0;
    }

    size_t tamanho = strlen(titular);

    if (tamanho == 0 || tamanho >= MAX_NOME) {
        printf("Nome invalido.\n");
        return 0;
    }

    if (strchr(titular, '|') != NULL ||
        strchr(titular, '\n') != NULL ||
        strchr(titular, '\r') != NULL) {
        printf("O nome contem caracteres invalidos.\n");
        return 0;
    }

    int numero = proximo_numero();

    if (numero == 0) {
        printf("Limite de numeracao atingido.\n");
        return 0;
    }

    Conta nova = {0};
    nova.numero = numero;

    if (snprintf(nova.titular, sizeof(nova.titular),
                 "%s", titular) >= (int)sizeof(nova.titular)) {
        printf("Nome muito longo.\n");
        return 0;
    }

    nova.saldo_centavos = 0;
    contas[quantidade_contas] = nova;

    if (!salvar_contas(contas, quantidade_contas + 1)) {
        printf("Erro ao salvar a conta. Operacao cancelada.\n");
        return 0;
    }

    quantidade_contas++;

    printf("Conta criada com sucesso! Numero: %d\n", numero);
    return numero;
}

void listar_contas(void) {
    if (quantidade_contas == 0) {
        printf("Nenhuma conta cadastrada.\n");
        return;
    }

    printf("\n--- CONTAS CADASTRADAS ---\n");

    for (int i = 0; i < quantidade_contas; i++) {
        printf("Conta: %d | Titular: %s | Saldo: ",
               contas[i].numero, contas[i].titular);
        formatar_dinheiro(contas[i].saldo_centavos);
        printf("\n");
    }
}

void consultar_saldo(int numero) {
    int indice = buscar_conta(numero);

    if (indice == -1) {
        printf("Conta nao encontrada.\n");
        return;
    }

    printf("Titular: %s\n", contas[indice].titular);
    printf("Saldo: ");
    formatar_dinheiro(contas[indice].saldo_centavos);
    printf("\n");
}

void depositar(int numero, long long valor) {
    int indice = buscar_conta(numero);

    if (indice == -1) {
        printf("Conta nao encontrada.\n");
        return;
    }

    if (valor <= 0) {
        printf("O valor deve ser maior que zero.\n");
        return;
    }

    if (contas[indice].saldo_centavos > LLONG_MAX - valor) {
        printf("Valor excede o limite permitido.\n");
        return;
    }

    long long saldo_anterior = contas[indice].saldo_centavos;
    contas[indice].saldo_centavos += valor;

    if (!salvar_contas(contas, quantidade_contas)) {
        contas[indice].saldo_centavos = saldo_anterior;
        printf("Erro ao salvar saldo. Operacao cancelada.\n");
        return;
    }

    if (!registrar_transacao(numero, "DEPOSITO", valor)) {
        printf("Aviso: saldo salvo, mas houve falha no historico.\n");
        return;
    }

    printf("Deposito realizado com sucesso.\n");
}

void sacar(int numero, long long valor) {
    int indice = buscar_conta(numero);

    if (indice == -1) {
        printf("Conta nao encontrada.\n");
        return;
    }

    if (valor <= 0) {
        printf("O valor deve ser maior que zero.\n");
        return;
    }

    if (valor > contas[indice].saldo_centavos) {
        printf("Saldo insuficiente.\n");
        return;
    }

    long long saldo_anterior = contas[indice].saldo_centavos;
    contas[indice].saldo_centavos -= valor;

    if (!salvar_contas(contas, quantidade_contas)) {
        contas[indice].saldo_centavos = saldo_anterior;
        printf("Erro ao salvar saldo. Operacao cancelada.\n");
        return;
    }

    if (!registrar_transacao(numero, "SAQUE", valor)) {
        printf("Aviso: saldo salvo, mas houve falha no historico.\n");
        return;
    }

    printf("Saque realizado com sucesso.\n");
}

void mostrar_historico(int numero) {
    if (buscar_conta(numero) == -1) {
        printf("Conta nao encontrada.\n");
        return;
    }

    Transacao transacoes[MAX_TRANSACOES];
    int quantidade = carregar_transacoes(
        transacoes, MAX_TRANSACOES
    );
    int encontrou = 0;

    printf("\n--- HISTORICO DA CONTA %d ---\n", numero);

    for (int i = 0; i < quantidade; i++) {
        if (transacoes[i].numero_conta == numero) {
            printf("ID: %d | %s | ",
                   transacoes[i].id,
                   transacoes[i].tipo);
            formatar_dinheiro(transacoes[i].valor_centavos);
            printf(" | %s\n", transacoes[i].data);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhuma transacao encontrada.\n");
    }
}
