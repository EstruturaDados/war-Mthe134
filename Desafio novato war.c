#include <stdio.h>
#include <string.h>

// Struct Territorio
typedef struct {
    char nome[50];
    char cor_exercito[20];
    int num_tropas;
} Territorio;

// Limpa buffer após scanf
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    Territorio mapa[5];
    int i;

    printf("--- Cadastro Inicial dos Territorios ---\n");

    for (i = 0; i < 5; i++) {
        printf("\nCadastro do Territorio %d de 5:\n", i + 1);

        limpaBuffer();

        printf("Digite o nome do territorio: ");
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Digite a cor do exercito dominante: ");
        fgets(mapa[i].cor_exercito, sizeof(mapa[i].cor_exercito), stdin);
        mapa[i].cor_exercito[strcspn(mapa[i].cor_exercito, "\n")] = '\0';

        printf("Digite o numero de tropas: ");
        scanf("%d", &mapa[i].num_tropas);
    }

    printf("\n--- Estado Atual do Mapa ---\n");
    for (i = 0; i < 5; i++) {
        printf("\nTerritorio %d:\n", i + 1);
        printf("Nome: %s\n", mapa[i].nome);
        printf("Cor do Exercito: %s\n", mapa[i].cor_exercito);
        printf("Numero de Tropas: %d\n", mapa[i].num_tropas);
    }

    return 0;
}
