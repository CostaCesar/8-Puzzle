#include "jogo.h"

/*
    Configuração final:
    -------------
    | 1 | 2 | 3 |
    -------------
    | 4 | 5 | 6 |
    -------------
    | 7 | 8 |   |
    -------------
*/

// Posição vetorial do valor
int obter_lugar(Grafos_mat *jogo, int valor)
{
    for(int i = 0; i < 9; i++)
	{
		if(jogo->adj[i][i] == valor)
			return i;
	}
    return -1;
}

// Movimento direcional
void fazer_movimento(Grafos_mat *jogo, Direcao direc)
{
	int i = obter_lugar(jogo, 0), j = 0;
    // Trocando
	switch (direc)
	{
	case 0:
		if(i/3 == 0) return; // Vazio no topo, não fazer nada
		j = i-3;
		break;
	case 1:
		if(i%3 == 2) return; // Vazio na direita, não fazer nada
		j = i+1;
		break;
	case 2:
		if(i/3 == 2) return; // Vazio no fundo, não fazer nada
		j = i+3;
		break;
	case 3:
		if(i%3 == 0) return; // Vazio na esquerda, não fazer nada
		j = i-1;
		break;
	}
	jogo->adj[i][i] = jogo->adj[j][j];
	jogo->adj[j][j] = 0;
}

int distancia_Manhattan(Grafos_mat *jogo)
{
    int distancia = 0;
    int esperado = 0;
    Grafos_mat *solucao = matriz_referencia();

    for (int i = 0; i < 9; i++)
    {
        int esperado = jogo->adj[i][i];
        esperado++;
        if(esperado == 9)
            esperado = 0;
        if (esperado != 0 && esperado != esperado)
        {
            int soluLinha = obter_lugar(solucao, esperado) / 3, soluColun = obter_lugar(solucao, esperado) % 3;
            int realLinha = obter_lugar(jogo, esperado) / 3, realColun = obter_lugar(jogo, esperado) % 3;
            distancia += abs(soluColun - realColun) + abs(soluLinha - realLinha);
        }
    }
    return distancia;
}

Grafos_mat *jogo_resolver_A(Grafos_mat *jogo)
{
    clock_t tempo_inicio, tempo_fim;
    double tempo_gasto = 0;

    tempo_inicio = clock();
    if(verifica_se_acabou(jogo) == TRUE)
        return TRUE;

    Lista *testados = NULL;
    Fila *testar = Fila_cria();
    Grafos_mat *buffer = aloca_grafo_m();

    jogo->dist_local = 0, jogo->dist_global = distancia_Manhattan(jogo);
    Fila_insere(testar, jogo);

    while(testar->inicio != NULL)
    {
        Grafos_mat *atual = Fila_retira(testar);
        int cod_atual = obter_codigo(atual), cod_buffer = 0;
        testados = lista_insere(testados, atual);
        
        if (verifica_se_acabou(atual))
        {
            lista_libera(testados);
            Fila_libera(testar);
            libera_grafo_m(buffer);
            tempo_fim = clock();
            tempo_gasto = ((double) (tempo_fim - tempo_inicio)) / CLOCKS_PER_SEC;
            return atual;
        }

        for(int i = 0; i < 4; i++)
        {
            grafo_copia(buffer, atual);
            fazer_movimento(buffer, i);
            cod_buffer = obter_codigo(buffer); 
            if(cod_atual != cod_buffer)
            {
                int possivelDist = atual->dist_local + 1;
                buffer->parente = atual;
                buffer->dist_local = possivelDist;
                buffer->dist_global = buffer->dist_local + distancia_Manhattan(buffer);
                if(lista_procura(testados, cod_buffer) == FALSE)
                    Fila_insere(testar, buffer);
            }
        }
    }
}

Grafos_mat* matriz_referencia(void) {

    // Aloca o Grafo
    Grafos_mat* grafo_jogo = aloca_grafo_m();

    // Insere o valor dos vertices;
    for (int i = 0; i < grafo_jogo->vert; i++)
    {
        grafo_jogo->adj[i][i] = i + 1;
    }
    grafo_jogo->adj[8][8] = NULL;

    // Insere os arcos, 0 - indica que o movimento é impossível,
    insere_arco_m(grafo_jogo, 0, 1);
    insere_arco_m(grafo_jogo, 0, 3);
    insere_arco_m(grafo_jogo, 1, 2);
    insere_arco_m(grafo_jogo, 1, 4);
    insere_arco_m(grafo_jogo, 2, 5);
    insere_arco_m(grafo_jogo, 3, 4);
    insere_arco_m(grafo_jogo, 3, 6);
    insere_arco_m(grafo_jogo, 4, 5);
    insere_arco_m(grafo_jogo, 4, 7);
    insere_arco_m(grafo_jogo, 5, 8);
    insere_arco_m(grafo_jogo, 6, 7);
    insere_arco_m(grafo_jogo, 7, 8);

    return grafo_jogo;
}

// Gera a matriz inicial do jogo, ou seja, uma matriz três por três aleatória;
Grafos_mat* matriz_embaralhada(void) {
    
    // Cria um grafo com a configuração final do tabuleiro;
    Grafos_mat* grafo_jogo = matriz_referencia();
    int i, j, buffer;

    srand(time(NULL)); // Gera uma seed aleatória para o rand;
    
    do {

        // Define uma quantidade de vezes que o tabuleiro será embaralhado;
        for (int k = 0; k < NUM_DE_EMBARALHAMENTOS; k++)
        {
            i = rand() % 9; // Gera dois números aleatórios entre 0 e 8;
            j = rand() % 9;

            buffer = grafo_jogo->adj[i][i];
            grafo_jogo->adj[i][i] = grafo_jogo->adj[j][j]; // Troca as peças com os indices i e j;
            grafo_jogo->adj[j][j] = buffer;
        }

    } while (possivel_resolver(grafo_jogo) == FALSE); // Verifica se é possível resolver o jogo e se não for embaralha de novo;


    // Retorna o grafo embaralhado;
    return grafo_jogo;
}

// Função que verifica se é possível resolver o jogo;
int possivel_resolver(Grafos_mat* grafo_jogo) {

    int inv_count = 0; // Variável que conta o número de inversões;
                       // Inversões são quando em uma sequêcia de números um número precede outro menor que ele;
                       // Ex: 1, 2, 3, 5, 4; uma inversão 5 - 4;

    for (int i = 0; i < 9 - 1; i++) {  // Faz um laço para contar o número de inversões do jogo;
        for (int j = i + 1; j < 9; j++) {
            if (grafo_jogo->adj[j][j] && grafo_jogo->adj[i][i] && grafo_jogo->adj[i][i] > grafo_jogo->adj[j][j]) {
                inv_count++;
            }
        }
    }

    // Retorna se o número de inversões é par, caso seja, quer dizer que o jogo é solucionável;
    return (inv_count % 2 == 0);
}

// Gera um tabuleiro de exemplo de um caso insolucionável;
Grafos_mat* jogo_impossivel_exemplo(void) {
    Grafos_mat* g = matriz_referencia();

    g->adj[0][0] = 8;
    g->adj[1][1] = 1;
    g->adj[2][2] = 2;
    g->adj[3][3] = 0;
    g->adj[4][4] = 4;
    g->adj[5][5] = 3;
    g->adj[6][6] = 7;
    g->adj[7][7] = 6;
    g->adj[8][8] = 5;

    return g;
}


// Função que inicia o jogo dos oito;
void inicia_jogo(void) {

    // Gera uma matriz embaralhada que será usada no jogo;
    Grafos_mat* grafo_jogo = matriz_embaralhada();
    Grafos_mat* resolut = jogo_resolver_A(grafo_jogo);
    for(Grafos_mat* step = resolut; step != NULL; step = step->parente)
    {
        imprime_jogo(step);
        _sleep(3000);
        system("cls");
    }
    char resposta = 'S';

    printf("\e[0;47;40m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[1;36;40m╔══════════════════════════════════╗\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[1;36;40m║           \e[1;31;34mJOGO DOS OITO\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[1;36;40m╚══════════════════════════════════╝\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┠──────────────────────────────────────────────────────────────────────────────────────────────────────────────┨\033[0;0m\n");
    printf("\e[0;47;40m┃\e[1;31;34mREGRAS:\e[0;47;40m                                                                                                       ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
    printf("\e[0;47;40m┃\e[0;36;40mDado um tabuleiro 3x3 com 8 peças (cada peça tem um número de 1 a 8) e um espaço vazio.\e[0;47;40m                       ┃\033[0;0m\n");
    printf("\e[0;47;40m┃\e[0;36;40mO objetivo é colocar os números nas peças em ordem de forma que fique igual à configuração final.\e[0;47;40m             ┃\033[0;0m\n");
    printf("\e[0;47;40m┃\e[0;36;40mPodemos deslizar quatro peças adjacentes (esquerda, direita, acima e abaixo) no espaço vazio.\e[0;47;40m                 ┃\033[0;0m\n");
    printf("\e[0;47;40m┃\e[0;36;40mO espaço vazio é representado como 0 para facilitar a visualização.\e[0;47;40m                                           ┃\033[0;0m\n");
	printf("\e[0;47;40m┃\e[0;36;40mVocê pode apertar Ctrl+C para encerrar o programa.\e[0;47;40m                                                            ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
    printf("\e[0;47;40m┠──────────────────────────────────────────────────────────────────────────────────────────────────────────────┨\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m╔══════════════════════════════════╗\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║        \e[1;31;34mConfiguração final:\e[0;36;40m       ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m┌───┬───┬───┐\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m│ 1 │ 2 │ 3 │\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m├───┼───┼───┤\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m│ 4 │ 5 │ 6 │\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m├───┼───┼───┤\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m│ 7 │ 8 │ 0 │\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m║          \033[1;35m└───┴───┴───┘\e[0;36;40m           ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                     \e[0;36;40m╚══════════════════════════════════╝\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┠──────────────────────────────────────────────────────────────────────────────────────────────────────────────┨\033[0;0m\n");
    printf("\e[0;47;40m┃\e[0;36;40mDeseja começar o jogo: S/N\e[0;47;40m                                                                                    ┃\033[0;0m\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    scanf(" %c", &resposta);

    if (resposta == 'S' || resposta == 's') {
		printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");

        while (verifica_se_acabou(grafo_jogo) == FALSE) // Faz um laço que faça com que o usuário possa fazer uma nova jogada até resolver o jogo;
        {
            imprime_jogo(grafo_jogo); //  Imprime o jogo atual da rodada;
            faz_jogada(grafo_jogo);  // Função que permite o usuário fazer uma jogada;
        }

        imprime_jogo(grafo_jogo); // Imprime o jogo final;

		printf("\e[0;47;40m┠──────────────────────────────────────────────────────────────────────────────────────────────────────────────┨\033[0;0m\n");
		printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
        printf("\e[0;47;40m┃                                  \e[0;36;40m!!!PARABÉNS VOCÊ VENCEU O JOGO DOS OITO!!!\e[0;47;40m                                  ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⣉⡥⠶⢶⣿⣿⣿⣿⣷⣆⠉⠛⠿⣿⣿⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⡿⢡⡞⠁⠀⠀⠤⠈⠿⠿⠿⠿⣿⠀⢻⣦⡈⠻⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⡇⠘⡁⠀⢀⣀⣀⣀⣈⣁⣐⡒⠢⢤⡈⠛⢿⡄⠻⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⡇⠀⢀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣶⣄⠉⠐⠄⡈⢀⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⠇⢠⣿⣿⣿⣿⡿⢿⣿⣿⣿⠁⢈⣿⡄⠀⢀⣀⠸⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⡿⠟⣡⣶⣶⣬⣭⣥⣴⠀⣾⣿⣿⣿⣶⣾⣿⣧⠀⣼⣿⣷⣌⡻⢿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⠟⣋⣴⣾⣿⣿⣿⣿⣿⣿⣿⡇⢿⣿⣿⣿⣿⣿⣿⡿⢸⣿⣿⣿⣿⣷⠄⢻                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⡏⠰⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⢂⣭⣿⣿⣿⣿⣿⠇⠘⠛⠛⢉⣉⣠⣴⣾                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣷⣦⣬⣍⣉⣉⣛⣛⣉⠉⣤⣶⣾⣿⣿⣿⣿⣿⣿⡿⢰⣿⣿⣿⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡘⣿⣿⣿⣿⣿⣿⣿⣿⡇⣼⣿⣿⣿⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                        ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⢸⣿⣿⣿⣿⣿⣿⣿⠁⣿⣿⣿⣿⣿⣿⣿⣿⣿                                        ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
		printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
		printf("\e[0;47;40m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\033[0;0m\n");
    }else{
		printf("\e[0;47;40m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\033[0;0m\n");
	}

    libera_grafo_m(grafo_jogo); // Libera o grafo que foi alocado para o jogo;
}

// Verifica se a configuração passada foi igual a configuração final;
int verifica_se_acabou(Grafos_mat* grafo_jogo) {
    Grafos_mat* ref = matriz_referencia();

    for (int i = 0; i < ref->vert; i++)
    {
        if (ref->adj[i][i] != grafo_jogo->adj[i][i]) {  // Caso algum indice não seja igual retorna que as matrizes são diferentes, ou seja, o jogo ainda não terminou;
            return FALSE;
        }
    }

    return TRUE;
}

// Imprime o jogo passado;
void imprime_jogo(Grafos_mat* grafo_jogo) {

    printf("\e[0;47;40m┃                                      \e[0;36;40m╔═════════════════════════════════╗\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m┌───┬───┬───┐\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m│ %d │ %d │ %d │\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n", grafo_jogo->adj[0][0], grafo_jogo->adj[1][1], grafo_jogo->adj[2][2]);
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m├───┼───┼───┤\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m│ %d │ %d │ %d │\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n", grafo_jogo->adj[3][3], grafo_jogo->adj[4][4], grafo_jogo->adj[5][5]);
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m├───┼───┼───┤\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m│ %d │ %d │ %d │\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n", grafo_jogo->adj[6][6], grafo_jogo->adj[7][7], grafo_jogo->adj[8][8]);
    printf("\e[0;47;40m┃                                      \e[0;36;40m║          \033[1;35m└───┴───┴───┘\e[0;36;40m          ║\e[0;47;40m                                     ┃\033[0;0m\n");
    printf("\e[0;47;40m┃                                      \e[0;36;40m╚═════════════════════════════════╝\e[0;47;40m                                     ┃\033[0;0m\n");
    
}


void faz_jogada(Grafos_mat *grafo_jogo) {
    int movimento;

	printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
    printf("\e[0;47;40m┠──────────────────────────────────────────────────────────────────────────────────────────────────────────────┨\033[0;0m\n");
	printf("\e[0;47;40m┃\e[0;36;40mQual peça você deseja mover:\e[0;47;40m                                                                                  ┃\033[0;0m\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    scanf(" %i", &movimento);
	printf("\e[0;47;40m┃                                                                                                              ┃\033[0;0m\n");
    
	// Procura pelo campo vazio;
    for (int i = 0; i < grafo_jogo->vert; i++){
        if (grafo_jogo->adj[i][i] == 0) {

            // Procura pela peça indicada;
            for (int j = 0; j < grafo_jogo->vert; j++) {
                if (grafo_jogo->adj[j][j] == movimento) {


                    // Verifica se é possível fazer aquele movimento;
                    if (grafo_jogo->adj[i][j] == 1) {

                        // Troca de lugar o espaço vazio;
                        grafo_jogo->adj[i][i] = grafo_jogo->adj[j][j];
                        grafo_jogo->adj[j][j] = 0;

                        // Terminou a troca;
                        return;
                    }
                }
            }

            
        }
    }
	printf("\e[0;47;40m┃\e[0;36;40mEsse movimento é impossível!\e[0;47;40m                                                                                  ┃\033[0;0m\n");
}
