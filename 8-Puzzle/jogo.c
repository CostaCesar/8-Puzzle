#include "jogo.h"

Grafos_mat* matriz_referencia(void) {

	Grafos_mat* grafo_jogo = aloca_grafo_m();

	// Insere o valor dos vertices;
	for (int i = 0; i < grafo_jogo->vert; i++)
	{
		grafo_jogo->adj[i][i] = i + 1;
	}
	grafo_jogo->adj[8][8] = NULL;


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
Grafos_mat* matriz_embaralhada(void) {
	
	Grafos_mat* grafo_jogo = matriz_referencia();
	int i, j, buffer;

	srand(time(NULL));
	
	do {

		for (int k = 0; k < NUM_DE_EMBARALHAMENTOS; k++)
		{
			i = rand() % 9;
			j = rand() % 9;

			buffer = grafo_jogo->adj[i][i];
			grafo_jogo->adj[i][i] = grafo_jogo->adj[j][j];
			grafo_jogo->adj[j][j] = buffer;

		}
	} while (possivel_resolver(grafo_jogo) == FALSE);



	return grafo_jogo;
}

int possivel_resolver(Grafos_mat* grafo_jogo) {

	int inv_count = 0;
	for (int i = 0; i < 9 - 1; i++) {
		for (int j = i + 1; j < 9; j++) {
			if (grafo_jogo->adj[j][j] && grafo_jogo->adj[i][i] && grafo_jogo->adj[i][i] > grafo_jogo->adj[j][j]) {
				inv_count++;
			}
		}
	}

	return (inv_count % 2 == 0);
}

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

void inicia_jogo(void) {

	Grafos_mat* grafo_jogo = matriz_embaralhada();
	char resposta = 'S';

	printf("Bem-Vindo(a) ao Jogo Dos Oito!!!\n");

	printf("Regras:\nDado um tabuleiro 3x3 com 8 pe�as (cada pe�a tem um n�mero de 1 a 8) e um espa�o vazio.\n O objetivo � colocar os n�meros nas pe�as em ordem de forma com a que fique igual configura��o final usando o espa�o vazio. \nPodemos deslizar quatro pe�as adjacentes (esquerda, direita, acima e abaixo) no espa�o vazio.");
	printf("\nO espa�o vazio � representado como 0 para facilitar a visualiza��o.");

	printf("\nConfigura��o final:\n");
	printf("\n\t1\t2\t3\n\t4\t5\t6\n\t7\t8\t0\n\n");

	printf("Deseja come�ar o jogo: S/N ");
	scanf("%c", &resposta);

	if (resposta == 'S' || resposta == 's') {
		while (verifica_se_acabou(grafo_jogo) == FALSE)
		{
			imprime_jogo(grafo_jogo);
			faz_jogada(grafo_jogo);
		}

		imprime_jogo(grafo_jogo);

		printf("\n!!!PARABENS VOC� VENCEU O JOGO DOS OITO!!!");
	}

	

	libera_grafo_m(grafo_jogo);
}


int verifica_se_acabou(Grafos_mat* grafo_jogo) {
	Grafos_mat* ref = matriz_referencia();

	for (int i = 0; i < ref->vert; i++)
	{
		if (ref->adj[i][i] != grafo_jogo->adj[i][i]) {
			return FALSE;
		}
	}

	return TRUE;
}


void imprime_jogo(Grafos_mat* grafo_jogo) {

	printf("\n\n\t%i\t", grafo_jogo->adj[0][0]);
	printf("%i\t",     grafo_jogo->adj[1][1]);
	printf("%i\n",     grafo_jogo->adj[2][2]);
	printf("\t%i\t",   grafo_jogo->adj[3][3]);
	printf("%i\t",     grafo_jogo->adj[4][4]);
	printf("%i\t\n",   grafo_jogo->adj[5][5]);
	printf("\t%i\t",   grafo_jogo->adj[6][6]);
	printf("%i\t",     grafo_jogo->adj[7][7]);
	printf("%i\t\n",   grafo_jogo->adj[8][8]);


	
}


void faz_jogada(Grafos_mat *grafo_jogo) {
	int movimento;

	printf("\n\nQual pe�a voc� deseja mover: \n");
	scanf("%i", &movimento);

	// Procura pelo campo vazio;
	for (int i = 0; i < grafo_jogo->vert; i++){
		if (grafo_jogo->adj[i][i] == NULL) {

			// Procura pelo pe�a indicada;
			for (int j = 0; j < grafo_jogo->vert; j++) {
				if (grafo_jogo->adj[j][j] == movimento) {


					// Verifica se � poss�vel fazer aquele movimento;
					if (grafo_jogo->adj[i][j] == 1) {

						// Troca de lugar o espa�o vazio;
						grafo_jogo->adj[i][i] = grafo_jogo->adj[j][j];
						grafo_jogo->adj[j][j] = NULL;

						// Terminou a troca;
						return;
					}
				}
			}

			
		}
	}
	printf("\nEsse movimento � imposs�vel!\n");

}