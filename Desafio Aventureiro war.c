#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[50];
    char cor_exercito[20];
    int num_tropas;
} Territorio;

/* Prototipos */
Territorio *criarMapa(int n);
void liberarMapa(Territorio *mapa);
void cadastrarTerritorios(Territorio *mapa, int n);
void mostrarMapa(Territorio *mapa, int n);
void limpaBuffer(void);
void simularAtaque(Territorio *mapa, int n, int atk_idx, int def_idx);

int main(void) {
    int n = 5; // número de territórios (conforme pedido: 1 a 5)
    int opc;
    int atk, def;
    Territorio *mapa;

    /* Inicializa gerador de números aleatórios */
    srand((unsigned) time(NULL));

    /* cria mapa dinamicamente com calloc (inicializa zeros) */
    mapa = criarMapa(n);
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        return 1;
    }

    printf("=== Nível Aventureiro: Batalhas Estratégicas ===\n\n");

    /* Cadastro inicial */
    cadastrarTerritorios(mapa, n);

    /* Loop interativo */
    do {
        printf("\n--- Menu ---\n");
        printf("1 - Mostrar mapa\n");
        printf("2 - Simular ataque\n");
        printf("3 - Re-cadastrar territórios (sobrescrever)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opc) != 1) {
            printf("Entrada invalida. Saindo.\n");
            break;
        }

        switch (opc) {
            case 1:
                mostrarMapa(mapa, n);
                break;
            case 2:
                mostrarMapa(mapa, n);
                printf("\nDigite o numero do territorio atacante (1 a %d): ", n);
                if (scanf("%d", &atk) != 1) { limpaBuffer(); printf("Entrada invalida.\n"); break; }
                printf("Digite o numero do territorio defensor (1 a %d): ", n);
                if (scanf("%d", &def) != 1) { limpaBuffer(); printf("Entrada invalida.\n"); break; }

                /* converter para indices (0-based) e validar */
                atk--; def--;
                if (atk < 0 || atk >= n || def < 0 || def >= n) {
                    printf("Indices invalidos. Tente novamente.\n");
                    break;
                }
                if (atk == def) {
                    printf("Atacante e defensor devem ser diferentes.\n");
                    break;
                }
                if (mapa[atk].num_tropas <= 0) {
                    printf("Territorio atacante não tem tropas suficientes para atacar.\n");
                    break;
                }
                if (mapa[def].num_tropas <= 0) {
                    printf("Territorio defensor já está sem tropas (talvez ja foi conquistado).\n");
                    break;
                }

                simularAtaque(mapa, n, atk, def);
                break;
            case 3:
                cadastrarTerritorios(mapa, n);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opc != 0);

    liberarMapa(mapa);
    return 0;
}

/* Aloca dinamicamente o mapa com calloc e retorna ponteiro */
Territorio *criarMapa(int n) {
    Territorio *m = (Territorio *) calloc(n, sizeof(Territorio));
    return m;
}

/* Libera memoria do mapa */
void liberarMapa(Territorio *mapa) {
    free(mapa);
}

/* Limpa buffer de entrada até newline */
void limpaBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Função para cadastrar territórios (nome, cor e tropas) */
void cadastrarTerritorios(Territorio *mapa, int n) {
    int i;
    printf("\n--- Cadastro dos Territorios (%d) ---\n", n);
    limpaBuffer(); // garante buffer limpo antes dos fgets

    for (i = 0; i < n; i++) {
        printf("\nTerritorio %d de %d\n", i + 1, n);

        printf("Digite o nome do territorio: ");
        if (fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin) == NULL) {
            strncpy(mapa[i].nome, "Territorio", sizeof(mapa[i].nome)-1);
            mapa[i].nome[sizeof(mapa[i].nome)-1] = '\0';
        } else {
            mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';
        }

        printf("Digite a cor do exercito dominante: ");
        if (fgets(mapa[i].cor_exercito, sizeof(mapa[i].cor_exercito), stdin) == NULL) {
            strncpy(mapa[i].cor_exercito, "Neutro", sizeof(mapa[i].cor_exercito)-1);
            mapa[i].cor_exercito[sizeof(mapa[i].cor_exercito)-1] = '\0';
        } else {
            mapa[i].cor_exercito[strcspn(mapa[i].cor_exercito, "\n")] = '\0';
        }

        /* Ler numero de tropas (int) de forma segura */
        printf("Digite o numero de tropas (>=0): ");
        while (1) {
            if (scanf("%d", &mapa[i].num_tropas) != 1) {
                printf("Entrada invalida. Digite um numero inteiro: ");
                limpaBuffer();
            } else {
                if (mapa[i].num_tropas < 0) mapa[i].num_tropas = 0;
                limpaBuffer(); // remove '\n' restante
                break;
            }
        }
    }
}

/* Exibe o estado atual do mapa */
void mostrarMapa(Territorio *mapa, int n) {
    int i;
    printf("\n--- Estado Atual do Mapa ---\n");
    for (i = 0; i < n; i++) {
        printf("Territorio %d: Nome: %s | Cor: %s | Tropas: %d\n",
               i + 1,
               mapa[i].nome[0] ? mapa[i].nome : "(vazio)",
               mapa[i].cor_exercito[0] ? mapa[i].cor_exercito : "(neutro)",
               mapa[i].num_tropas);
    }
}

/* Simula um ataque entre dois territorios
   Regras implementadas:
   - Gera um valor de ataque e defesa com rand() influenciado pelo numero de tropas
   - Se ataque > defesa: defensor perde 1 tropa. Se tropas do defensor chegam a 0 -> territorio é conquistado:
       * cor_exercito do defensor passa a ser a do atacante
       * atacante perde 1 tropa (move 1 tropa para o novo territorio), e defensor fica com 1 tropa
   - Se defesa > ataque: atacante perde 1 tropa
   - Se empate: nada acontece (empate)
*/
void simularAtaque(Territorio *mapa, int n, int atk_idx, int def_idx) {
    int atk_tropas = mapa[atk_idx].num_tropas;
    int def_tropas = mapa[def_idx].num_tropas;

    /* segurança: tropas > 0 já garantido antes da chamada, mas reforçamos */
    if (atk_tropas <= 0 || def_tropas <= 0) {
        printf("Erro: uma das partes não tem tropas.\n");
        return;
    }

    /* Gerar números aleatórios de batalha.
       - Faz com que número máximo dependa das tropas (maior exército tende a gerar valores maiores).
       - Evita mod 0: usamos (tropas) como divisor; como tropas>=1, ok.
    */
    int ataque = (rand() % atk_tropas) + 1;   // 1 .. atk_tropas
    int defesa = (rand() % def_tropas) + 1;   // 1 .. def_tropas

    /* Para variar mais, somamos um d6 extra às roladas (opcional para maior aleatoriedade) */
    ataque += (rand() % 6) + 1; // +1..6
    defesa += (rand() % 6) + 1;

    printf("\n--- Batalha ---\n");
    printf("Atacante: %s (Cor: %s, Tropas: %d)\n", mapa[atk_idx].nome, mapa[atk_idx].cor_exercito, atk_tropas);
    printf("Defensor:  %s (Cor: %s, Tropas: %d)\n", mapa[def_idx].nome, mapa[def_idx].cor_exercito, def_tropas);
    printf("Valor de ataque sorteado: %d\n", ataque);
    printf("Valor de defesa sorteado:  %d\n", defesa);

    if (ataque > defesa) {
        /* atacante vence a rodada: defensor perde 1 tropa */
        mapa[def_idx].num_tropas -= 1;
        if (mapa[def_idx].num_tropas < 0) mapa[def_idx].num_tropas = 0;
        printf("Resultado: Atacante vence! Defensor perde 1 tropa. Tropas defensor agora: %d\n", mapa[def_idx].num_tropas);

        /* Se defensor ficou sem tropas -> conquista */
        if (mapa[def_idx].num_tropas == 0) {
            printf("Territorio %s foi conquistado por %s!\n", mapa[def_idx].nome, mapa[atk_idx].nome);

            /* Conquista: cor_exercito do defensor vira a do atacante.
               Transferimos 1 tropa do atacante para o territorio conquistado (se possível).
               Se atacante tiver apenas 1 tropa, ainda permite conquista (ataque sai com 0 tropas no original).
            */
            strncpy(mapa[def_idx].cor_exercito, mapa[atk_idx].cor_exercito, sizeof(mapa[def_idx].cor_exercito)-1);
            mapa[def_idx].cor_exercito[sizeof(mapa[def_idx].cor_exercito)-1] = '\0';

            /* Ajuste de tropas: atacante cede 1 tropa para ocupar (se tiver pelo menos 1) */
            if (mapa[atk_idx].num_tropas > 0) {
                mapa[atk_idx].num_tropas -= 1;
                mapa[def_idx].num_tropas = 1;
                printf("%s move 1 tropa para o territorio conquistado.\n", mapa[atk_idx].nome);
            } else {
                /* caso raro: atacante sem tropas após combate: território fica com 1 tropa neutra da captura */
                mapa[def_idx].num_tropas = 1;
            }

            printf("Tropas apos conquista -> Atacante (%s): %d | Novo dono (%s): %d\n",
                   mapa[atk_idx].nome, mapa[atk_idx].num_tropas, mapa[def_idx].nome, mapa[def_idx].num_tropas);
        }

    } else if (defesa > ataque) {
        /* defensor vence: atacante perde 1 tropa */
        mapa[atk_idx].num_tropas -= 1;
        if (mapa[atk_idx].num_tropas < 0) mapa[atk_idx].num_tropas = 0;
        printf("Resultado: Defensor vence! Atacante perde 1 tropa. Tropas atacante agora: %d\n", mapa[atk_idx].num_tropas);
    } else {
        /* empate: nenhuma alteração */
        printf("Resultado: Empate! Nenhuma tropa perdida.\n");
    }
}
