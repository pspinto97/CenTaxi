#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {

	TCHAR matricula;
	int posx;
	int posy;

} tx, * taxi;


typedef struct {

	taxi lista;
	TCHAR opcao[100];
	TCHAR resposta[100];

} cmd, * command;


typedef struct {
	int x;
	int y;
} dd, * dados;


typedef struct {

	char info[20][20];

} mp, * mapa;


typedef struct {

	HANDLE eventoNovaMensagem;
	HANDLE eventoEsperaResposta;
	HANDLE MTX;
	cmd* share;

} dados_threads;

typedef struct {

	int matricula[10];

} matr, * matriculas;
#endif /* STRUCTS_H */