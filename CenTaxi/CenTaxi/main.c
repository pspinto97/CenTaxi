#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "stdio.h"
#include "stdlib.h"
#include <fcntl.h>
#include <io.h>
#include <tchar.h>

#include "..\CenTaxi\structs.h"

#define Limite 20


int processaMensagem(TCHAR* opcao, TCHAR* resposta, int* ntaxis, matr* todosTaxis) {

	TCHAR opt[20];
	int mat;

	_stscanf(opcao, TEXT("%s : %d"), opt, &mat);

	if (_tcscmp(opt, TEXT("MAT")) == 0) {
		_tprintf(TEXT("Pedido de matricula registadO: %d"), mat);
		_tcscpy(resposta, TEXT("OK"));
		if (*ntaxis > 10) {
			_tprintf(TEXT("Numero de matriculas excedido"));
			_tcscpy(resposta, TEXT("CenTaxi cheio!"));
			return -1;
		}
		todosTaxis->matricula[(*ntaxis)++] = mat;
	}

	return 0;
}

void EsperaNovaMensagem(dados_threads* t, int* ntaxis, matr* todosTaxis) {
	WaitForSingleObject(t->eventoNovaMensagem, INFINITE);

	//_tprintf(TEXT("Mensagem recebida taxi: %s\n"), t->share->opcao);

	processaMensagem(t->share->opcao, t->share->resposta, ntaxis, todosTaxis);

	//_tcscpy(t->share->resposta, TEXT("PotasV2"));

	SetEvent(t->eventoEsperaResposta);
}

unsigned __stdcall THREADTAXI(void* p) {

	dados_threads* t = (dados_threads*)p;
	int ntaxis = 0;
	matr todosTaxis;

	while (TRUE) {

		EsperaNovaMensagem(t, &ntaxis, &todosTaxis);

	}

	return 0;
}

int _tmain( void ) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	dados info;
	FILE* f;
	int tam1, tam2;
	HANDLE fileMap;
	HANDLE MEMTAXI;
	HANDLE EVENTO;
	HANDLE MUTEX;
	HANDLE EVENTOLEITURA;
	TCHAR opcao[25];
	int matriculas[10];


	f = fopen("mapa.txt", "r");

	if (!f) {
		_tprintf(TEXT("Erro na abertura!"));
		return -1;
	}

	fscanf(f, "%d", &tam1);
	fscanf(f, "%d", &tam2);
	fileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(dd) + sizeof(int) * tam1 * tam2,
		TEXT("SharedMemory")
	);

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("Já existe um!"));
		return 1;
	}

	info = MapViewOfFile(
		fileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
	);


	int* ptr = (int*)((dados)(info + 1));
	for (int i = 0; i < tam1; i++) {
		for (int o = 0; o < tam2; o++) {
			char c;
			fscanf(f, "%c", &c);

			if (c == 'X')
				ptr[i * tam2 + o] = 0;
			else
				ptr[i * tam2 + o] = 1;
		}
	}
	HANDLE fileTaxi;
	cmd* share = NULL;
	fileTaxi = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(cmd),
		TEXT("SharedTaxi")
	);
	if (fileTaxi == NULL) {
		_tprintf(TEXT("ERRO NO CREATEFILEMAPPING"));
		return -1;
	}
	share = (cmd*)MapViewOfFile(
		fileTaxi,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
	);
	if (share == NULL) {
		_tprintf(TEXT("ERRO NO MAPVIEWOFFILE"));
		return -1;
	}

	EVENTO = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		(TEXT("EVENTOTAXI"))
	);
	if (EVENTO == NULL) {
		_tprintf(TEXT("ERRO NO CreateEvent"));
		return -1;
	}
	EVENTOLEITURA = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		(TEXT("EVENTOLEITURATAXI"))
	);
	if (EVENTOLEITURA == NULL) {
		_tprintf(TEXT("ERRO NO CreateEvent"));
		return -1;
	}
	MUTEX = CreateMutex(NULL, FALSE, TEXT("THREADMUTEXTAXI"));

	if (MUTEX == NULL) {
		_tprintf(TEXT("ERRO NA CRIACAO DO MUTEX"));
		return -1;
	}

	dados_threads DADOTHREAD;
	DADOTHREAD.eventoNovaMensagem = EVENTOLEITURA;
	DADOTHREAD.eventoEsperaResposta = EVENTO;
	DADOTHREAD.MTX = MUTEX;
	DADOTHREAD.share = share;

	HANDLE th = (HANDLE)_beginthreadex(
		NULL,
		0,
		THREADTAXI,
		(LPVOID)&DADOTHREAD,
		0,
		NULL
	);

	if (th == 0) {

		return 1;
	}
	while (1) {
		_tprintf(TEXT("Insira um comando--> "));
		_tscanf(TEXT(" %s"), opcao);
		if (_tcscmp(opcao, TEXT("Sair")) == 0) {
			break;
		}
	}




}