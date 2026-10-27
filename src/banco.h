
#ifndef BANCO_H
#define BANCO_H

#define MAX_CONTAS 100
#define MAX_NOME 50
#define MAX_TRANSACOES 1000

typedef struct {
    int numero;
    char titular[MAX_NOME];
    long long saldo_centavos;
} Conta;

typedef struct {
    int id;
    int numero_conta;
    char tipo[15];
    long long valor_centavos;
    char data[30];
} Transacao;

void inicializar_banco(void);
int criar_conta(const char *titular);
void listar_contas(void);
void consultar_saldo(int numero);
void depositar(int numero, long long valor);
void sacar(int numero, long long valor);
void mostrar_historico(int numero);
int buscar_conta(int numero);

#endif
