#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_COMP 20
#define MAX_NOME 30
#define MAX_TIPO 20

// ---------------------------
// ESTRUTURAS
// ---------------------------

// Representa um item/componente da torre de fuga
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade; // 1 (baixa) .. 10 (altíssima)
} Componente;

// Guarda métricas de desempenho de cada ordenação
typedef struct {
    long comparacoes;
    double tempoSegundos;
} Analise;

// ---------------------------
// PROTÓTIPOS
// ---------------------------

// Utilidades gerais
void limparTela();
void lerString(char *dest, int tamanho);
void pausar();

// Entrada / exibição
void cadastrarComponente(Componente comps[], int *n);
void listarComponentes(const Componente comps[], int n);

// Ordenações
Analise bubbleSortNome(Componente comps[], int n);
Analise insertionSortTipo(Componente comps[], int n);
Analise selectionSortPrioridade(Componente comps[], int n);

// Busca binária (por nome)
int buscaBinariaPorNome(const Componente comps[], int n, const char *chave, long *comparacoes);

// Menu principal
void menuPrincipal();

// Função de comparação segura de strings (case-insensitive opcional)
int cmpStrCase(const char *a, const char *b);

// ---------------------------
// VARIÁVEIS GLOBAIS DO SISTEMA
// ---------------------------

Componente mochila[MAX_COMP];
int numComponentes = 0;

// controla se o vetor ESTÁ ordenado por nome, necessário pra busca binária
bool ordenadoPorNome = false;


// ===========================
// FUNÇÃO PRINCIPAL
// ===========================
int main() {
    menuPrincipal();
    return 0;
}

// ===========================
// IMPLEMENTAÇÕES
// ===========================

// "limpar" a tela de forma portátil (só joga várias linhas)
void limparTela() {
    for (int i = 0; i < 40; i++) {
        puts("");
    }
}

// leitura segura de string com fgets, removendo '\n'
void lerString(char *dest, int tamanho) {
    fgets(dest, tamanho, stdin);
    // remove \n se existir
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] == '\n') {
        dest[len - 1] = '\0';
    }
}

// pause estilo "pressione enter..."
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// cadastro de componente na mochila
void cadastrarComponente(Componente comps[], int *n) {
    if (*n >= MAX_COMP) {
        printf("\n⚠ Mochila cheia! Você já cadastrou %d componentes.\n", MAX_COMP);
        return;
    }

    Componente novo;

    printf("\n== Cadastro de Componente ==\n");

    printf("Nome do componente: ");
    lerString(novo.nome, MAX_NOME);

    printf("Tipo do componente (ex: controle, suporte, propulsao): ");
    lerString(novo.tipo, MAX_TIPO);

    printf("Prioridade (1 a 10): ");
    scanf("%d", &novo.prioridade);
    getchar(); // consumir \n que sobrou do scanf

    // adiciona ao vetor
    comps[*n] = novo;
    (*n)++;

    // após inserir bagunça a ordenação "por nome"
    ordenadoPorNome = false;

    printf("\n✅ Componente adicionado com sucesso!\n");
}

// imprime os componentes em formato tabular
void listarComponentes(const Componente comps[], int n) {
    if (n == 0) {
        printf("\n[Mochila vazia]\n");
        return;
    }

    printf("\n===== COMPONENTES ATUAIS =====\n");
    printf("%-3s | %-28s | %-18s | %-10s\n", "#", "Nome", "Tipo", "Prioridade");
    printf("----+------------------------------+--------------------+------------\n");

    for (int i = 0; i < n; i++) {
        printf("%-3d | %-28s | %-18s | %-10d\n",
               i + 1,
               comps[i].nome,
               comps[i].tipo,
               comps[i].prioridade);
    }
}

// comparação de strings sem diferenciar maiúsc/minúsc (a-z == A-Z)
int cmpStrCase(const char *a, const char *b) {
    // versão simples: comparar char a char normalizando pra minúsculo
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;

        if (ca < cb) return -1;
        if (ca > cb) return 1;
        a++;
        b++;
    }
    if (*a == '\0' && *b == '\0') return 0;
    if (*a == '\0') return -1;
    return 1;
}

// ---------------------------
// BUBBLE SORT por NOME
// ---------------------------
// - Ordem alfabética crescente de nome
// - Conta comparações entre nomes
// - Mede tempo
Analise bubbleSortNome(Componente comps[], int n) {
    Analise a;
    a.comparacoes = 0;
    a.tempoSegundos = 0.0;

    if (n < 2) return a;

    clock_t inicio = clock();

    for (int i = 0; i < n - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < n - 1 - i; j++) {
            a.comparacoes++;
            if (cmpStrCase(comps[j].nome, comps[j + 1].nome) > 0) {
                Componente tmp = comps[j];
                comps[j] = comps[j + 1];
                comps[j + 1] = tmp;
                trocou = true;
            }
        }
        if (!trocou) break; // otimização
    }

    clock_t fim = clock();
    a.tempoSegundos = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // após ordenar por nome
    ordenadoPorNome = true;

    return a;
}

// ---------------------------
// INSERTION SORT por TIPO
// ---------------------------
// - Ordem alfabética crescente do campo tipo
// - Conta comparações de strings tipo
Analise insertionSortTipo(Componente comps[], int n) {
    Analise a;
    a.comparacoes = 0;
    a.tempoSegundos = 0.0;

    if (n < 2) return a;

    clock_t inicio = clock();

    for (int i = 1; i < n; i++) {
        Componente chave = comps[i];
        int j = i - 1;

        // movimenta enquanto comps[j].tipo > chave.tipo
        while (j >= 0) {
            a.comparacoes++;
            if (cmpStrCase(comps[j].tipo, chave.tipo) > 0) {
                comps[j + 1] = comps[j];
                j--;
            } else {
                break;
            }
        }
        comps[j + 1] = chave;
    }

    clock_t fim = clock();
    a.tempoSegundos = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // agora não temos mais garantia de ordenação por nome
    ordenadoPorNome = false;

    return a;
}

// ---------------------------
// SELECTION SORT por PRIORIDADE
// ---------------------------
// - Ordem decrescente de prioridade (10 primeiro, depois 9, ...)
// - Conta comparações numéricas
Analise selectionSortPrioridade(Componente comps[], int n) {
    Analise a;
    a.comparacoes = 0;
    a.tempoSegundos = 0.0;

    if (n < 2) return a;

    clock_t inicio = clock();

    for (int i = 0; i < n - 1; i++) {
        int idxMaior = i;
        for (int j = i + 1; j < n; j++) {
            a.comparacoes++;
            if (comps[j].prioridade > comps[idxMaior].prioridade) {
                idxMaior = j;
            }
        }
        if (idxMaior != i) {
            Componente tmp = comps[i];
            comps[i] = comps[idxMaior];
            comps[idxMaior] = tmp;
        }
    }

    clock_t fim = clock();
    a.tempoSegundos = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // perdeu a ordenação por nome
    ordenadoPorNome = false;

    return a;
}

// ---------------------------
// BUSCA BINÁRIA POR NOME
// ---------------------------
// Requer: vetor ordenado (crescente) por nome
// Retorna índice do item encontrado ou -1
int buscaBinariaPorNome(const Componente comps[], int n, const char *chave, long *comparacoes) {
    int ini = 0;
    int fim = n - 1;
    *comparacoes = 0;

    while (ini <= fim) {
        int meio = (ini + fim) / 2;
        (*comparacoes)++;

        int cmp = cmpStrCase(chave, comps[meio].nome);

        if (cmp == 0) {
            return meio; // achou
        } else if (cmp < 0) {
            fim = meio - 1;
        } else {
            ini = meio + 1;
        }
    }

    return -1; // não achou
}

// ---------------------------
// MENU DE OPERAÇÕES
// ---------------------------
void menuPrincipal() {
    int opc = -1;
    while (opc != 0) {
        limparTela();
        printf("=======================================\n");
        printf("        TORRE DE RESGATE - FASE FINAL  \n");
        printf("=======================================\n");
        printf("Componentes cadastrados: %d / %d\n", numComponentes, MAX_COMP);
        printf("Estado para busca binaria por nome: %s\n",
               ordenadoPorNome ? "PRONTO (ordenado por nome)" : "NAO ORDENADO");
        printf("---------------------------------------\n");
        printf("1. Cadastrar componente\n");
        printf("2. Listar componentes\n");
        printf("3. Ordenar por NOME (Bubble Sort)\n");
        printf("4. Ordenar por TIPO (Insertion Sort)\n");
        printf("5. Ordenar por PRIORIDADE (Selection Sort)\n");
        printf("6. Buscar componente por NOME (busca binaria)\n");
        printf("0. Sair\n");
        printf("---------------------------------------\n");
        printf("Escolha sua jogada: ");
        scanf("%d", &opc);
        getchar(); // consome \n

        switch (opc) {
            case 1: {
                cadastrarComponente(mochila, &numComponentes);
                pausar();
                break;
            }
            case 2: {
                listarComponentes(mochila, numComponentes);
                pausar();
                break;
            }
            case 3: { // Bubble sort por nome
                Analise r = bubbleSortNome(mochila, numComponentes);
                printf("\n🔥 Ordenado por NOME (Bubble Sort)!\n");
                printf("Comparacoes: %ld\n", r.comparacoes);
                printf("Tempo: %.6f segundos\n", r.tempoSegundos);
                listarComponentes(mochila, numComponentes);
                pausar();
                break;
            }
            case 4: { // Insertion sort por tipo
                Analise r = insertionSortTipo(mochila, numComponentes);
                printf("\n⚙ Ordenado por TIPO (Insertion Sort)!\n");
                printf("Comparacoes: %ld\n", r.comparacoes);
                printf("Tempo: %.6f segundos\n", r.tempoSegundos);
                listarComponentes(mochila, numComponentes);
                pausar();
                break;
            }
            case 5: { // Selection sort por prioridade
                Analise r = selectionSortPrioridade(mochila, numComponentes);
                printf("\n🚨 Ordenado por PRIORIDADE (Selection Sort)!\n");
                printf("Comparacoes: %ld\n", r.comparacoes);
                printf("Tempo: %.6f segundos\n", r.tempoSegundos);
                listarComponentes(mochila, numComponentes);
                pausar();
                break;
            }
            case 6: { // Busca binária por nome
                if (!ordenadoPorNome) {
                    printf("\n❌ Não posso fazer busca binária agora.\n");
                    printf("   Primeiro ordene por NOME (opção 3).\n");
                } else {
                    char chave[MAX_NOME];
                    printf("\nDigite o nome exato do componente que voce quer localizar: ");
                    lerString(chave, MAX_NOME);

                    long compsFeitas = 0;
                    int idx = buscaBinariaPorNome(mochila, numComponentes, chave, &compsFeitas);

                    if (idx >= 0) {
                        printf("\n✅ Componente encontrado!\n");
                        printf("Nome: %s\n", mochila[idx].nome);
                        printf("Tipo: %s\n", mochila[idx].tipo);
                        printf("Prioridade: %d\n", mochila[idx].prioridade);
                        printf("Comparacoes realizadas na busca: %ld\n", compsFeitas);
                        printf("\n>> CHAVE LOCALIZADA. Torre de resgate pode ser ativada! <<\n");
                    } else {
                        printf("\n⚠ Componente '%s' nao encontrado.\n", chave);
                        printf("Comparacoes realizadas na busca: %ld\n", compsFeitas);
                    }
                }
                pausar();
                break;
            }
            case 0: {
                printf("\nSaindo... Boa sorte na extração, sobrevivente. 🪂\n");
                break;
            }
            default: {
                printf("\nOpcao invalida.\n");
                pausar();
                break;
            }
        }
    }
}
