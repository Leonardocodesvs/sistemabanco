
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "banco.h"

static int ler_linha(char *buffer, size_t tamanho) {
    if (fgets(buffer, (int)tamanho, stdin) == NULL) {
        return 0;
    }

    size_t comprimento = strlen(buffer);

    if (comprimento > 0 &&
        buffer[comprimento - 1] == '\n') {
        buffer[--comprimento] = '\0';
    } else {
        int caractere;
        while ((caractere = getchar()) != '\n' &&
               caractere != EOF) {
        }
        return 0;
    }

    if (comprimento > 0 &&
        buffer[comprimento - 1] == '\r') {
        buffer[--comprimento] = '\0';
    }

    return 1;
}

static int ler_inteiro(const char *mensagem, int *resultado) {
    char entrada[64];
    char *fim;

    printf("%s", mensagem);

    if (!ler_linha(entrada, sizeof(entrada))) {
        printf("Entrada invalida ou muito longa.\n");
        return 0;
    }

    errno = 0;
    long valor = strtol(entrada, &fim, 10);

    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    if (entrada[0] == '\0' ||
        *fim != '\0' ||
        errno == ERANGE ||
        valor < 0 ||
        valor > INT_MAX) {
        printf("Digite um numero inteiro valido.\n");
        return 0;
    }

    *resultado = (int)valor;
    return 1;
}

static int ler_dinheiro(const char *mensagem,
                        long long *centavos) {
    char entrada[64];
    char *p;
    unsigned long long reais = 0;
    int digitos = 0;
    int casas_decimais = 0;
    int fracao = 0;
    int encontrou_separador = 0;

    printf("%s", mensagem);

    if (!ler_linha(entrada, sizeof(entrada))) {
        printf("Entrada invalida ou muito longa.\n");
        return 0;
    }

    p = entrada;

    if (*p == '\0' || *p == '-') {
        printf("Digite um valor positivo.\n");
        return 0;
    }

    while (*p != '\0') {
        if (isdigit((unsigned char)*p)) {
            int digito = *p - '0';
            digitos++;

            if (!encontrou_separador) {
                if (reais > ((unsigned long long)LLONG_MAX / 100
                             - (unsigned long long)digito) / 10) {
                    printf("Valor muito grande.\n");
                    return 0;
                }

                reais = reais * 10 + (unsigned long long)digito;
            } else {
                if (casas_decimais >= 2) {
                    printf("Use no maximo duas casas decimais.\n");
                    return 0;
                }

                fracao = fracao * 10 + digito;
                casas_decimais++;
            }
        } else if ((*p == ',' || *p == '.') &&
                   !encontrou_separador) {
            encontrou_separador = 1;
        } else {
            printf("Formato invalido. Exemplo: 150,50\n");
            return 0;
        }

        p++;
    }

    if (digitos == 0) {
        printf("Digite um valor valido.\n");
        return 0;
    }

    if (casas_decimais == 1) {
        fracao *= 10;
    }

    if (reais > ((unsigned long long)LLONG_MAX - 
             (unsigned long long)fracao) / 100ULL) {
    printf("Valor fora do limite permitido.\n");
    return 0;
}

unsigned long long total = reais * 100ULL +
                           (unsigned long long)fracao;

if (total == 0) {
    printf("O valor deve ser maior que zero.\n");
    return 0;
}

    *centavos = (long long)total;
    return 1;
}

static void preparar_pasta(void) {
#ifdef _WIN32
    if (_mkdir("data") != 0 && errno != EEXIST) {
        perror("Nao foi possivel criar a pasta data");
        exit(EXIT_FAILURE);
    }
#else
    if (mkdir("data", 0700) != 0 && errno != EEXIST) {
        perror("Nao foi possivel criar a pasta data");
        exit(EXIT_FAILURE);
    }
#endif
}

static void menu(void) {
    printf("\n====== SISTEMA BANCARIO ======\n");
    printf("1. Criar conta\n");
    printf("2. Listar contas\n");
    printf("3. Consultar saldo\n");
    printf("4. Depositar\n");
    printf("5. Sacar\n");
    printf("6. Historico de transacoes\n");
    printf("0. Sair\n");
    printf("==============================\n");
}

int main(void) {
    int opcao;

    preparar_pasta();
    inicializar_banco();

    for (;;) {
        menu();

        if (!ler_inteiro("Escolha uma opcao: ", &opcao)) {
            continue;
        }

        if (opcao == 0) {
            printf("Encerrando o sistema.\n");
            break;
        }

        if (opcao == 1) {
            char nome[MAX_NOME];

            printf("Nome do titular: ");

            if (!ler_linha(nome, sizeof(nome))) {
                printf("Nome invalido ou muito longo.\n");
                continue;
            }

            if (nome[0] == '\0') {
                printf("O nome nao pode ficar vazio.\n");
                continue;
            }

            criar_conta(nome);
        } else if (opcao == 2) {
            listar_contas();
        } else if (opcao >= 3 && opcao <= 6) {
            int numero;

            if (!ler_inteiro("Numero da conta: ", &numero)) {
                continue;
            }

            if (opcao == 3) {
                consultar_saldo(numero);
            } else if (opcao == 4 || opcao == 5) {
                long long valor;

                if (!ler_dinheiro("Valor (ex.: 150,50): ",
                                 &valor)) {
                    continue;
                }

                if (opcao == 4) {
                    depositar(numero, valor);
                } else {
                    sacar(numero, valor);
                }
            } else {
                mostrar_historico(numero);
            }
        } else {
            printf("Opcao inexistente.\n");
        }
    }

    return 0;
}
