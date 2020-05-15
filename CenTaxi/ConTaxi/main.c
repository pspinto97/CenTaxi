#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "stdio.h"
#include "stdlib.h"
#include <fcntl.h>
#include <io.h>
#include <tchar.h>

#include "..\CenTaxi\structs.h"


void EscreveMensagemCentral(dados_threads* t, const TCHAR* msg, TCHAR* resp) {
	WaitForSingleObject(t->MTX, INFINITE);

	//escrever mensagem
	_tcscpy(t->share->opcao, msg);

	SetEvent(t->eventoNovaMensagem);

	WaitForSingleObject(t->eventoEsperaResposta, INFINITE);

	//le resposta
	_tcscpy(resp, t->share->resposta);
	ReleaseMutex(t->MTX);
}

unsigned __stdcall THREADTAXI(void* p) {

	dados_threads* t = (dados_threads*)p;
	TCHAR resposta[100];
	TCHAR mensagem[100];

	//geras a matricula aleatoriamente
	int pota = rand();

	_stprintf(mensagem, TEXT("MAT : %d"), &pota);

	EscreveMensagemCentral(t, mensagem, resposta);


	_tprintf(TEXT("Mensagem recebida: %s\n"), resposta);



	return 0;
}

int _tmain( void ) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	char opcao[25];
	HANDLE MEMTAXI;
	HANDLE EVENTO;
	HANDLE EVENTOLEITURA;
	HANDLE MUTEX;
	srand(NULL);


	// Memória Partilhada
	MEMTAXI = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		TEXT("SharedTaxi")
	);

	// Verifica Memória Partilhada
	if (MEMTAXI == NULL) {
		_tprintf(TEXT("ERRO NA MEMORIA PARTILHADA"));
		return -1;
	}

	// Cria Evento
	EVENTO = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		(TEXT("EVENTOTAXI"))
	);

	// Verifica cria evento
	if (EVENTO == NULL) {
		_tprintf(TEXT("ERRO NO CreateEvent"));
		return -1;
	}

	// Cria evento para leitura
	EVENTOLEITURA = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		(TEXT("EVENTOLEITURATAXI"))
	);

	// Verifica evento de leitura
	if (EVENTOLEITURA == NULL) {
		_tprintf(TEXT("ERRO NO CreateEvent"));
		return -1;
	}


	HANDLE fileTaxi;
	cmd* share = NULL;

	// Cria memória partilhada com o tamanho da struct
	fileTaxi = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(cmd),
		TEXT("SharedTaxi")
	);

	// Inicializa
	share = (cmd*)MapViewOfFile(
		fileTaxi,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
	);


	// Verifica anterior
	if (share == NULL) {
		_tprintf(TEXT("ERRO NO MAPVIEWOFFILE"));
		return -1;
	}

	// Cria o Mutex para o Taxi
	MUTEX = CreateMutex(NULL, FALSE, TEXT("THREADMUTEXTAXI"));

	// Verifica Mutex
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