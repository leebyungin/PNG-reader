#define _CRT_SECURE_NO_WARNINGS
#define KEY_Z 90
#define BUF_SIZE_LENANDTYPE 8
#define BUF_SIZE_CRC 4
#define IHDR_START 8
#define CHUNK_TYPE_FIRST 4
#define IS_CRITIC CHUNK_TYPE_FIRST
#define CHUNK_LEN 4
#include<stdio.h>
#include<conio.h>
#include<Windows.h>

void error_handling(const char* msg);

typedef union {
	unsigned int INT;
	unsigned char BYTE[4];
}BYTE4;

int main(int argc, char* argv[]) {
	system("cls");	//이스케이프 시퀀스를 사용하려면 필요함. 이거 없으면 아래 주석문이 코드에 삽입돼야함.
	/*
	#include <windows.h>

// this line is only for older versions of windows headers (pre Win 10):
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004

int main(void)
{
    // enable ANSI sequences for windows 10:
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(console, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(console, consoleMode);
	*/

	char str[1024];

	/*if (argc != 2) {
		printf("Usage : %s <PNG file name>\n", argv[0]);
		exit(1);
	}*/

	printf("input file name : ");
	scanf("%s", str);
	while (getchar() != '\n');
	
	FILE* pfile;
	unsigned long long file_size;

	unsigned char byte;
	unsigned char chunk_lenNtype[BUF_SIZE_LENANDTYPE];
	unsigned char chunk_crc[BUF_SIZE_CRC];
	BYTE4 chunk_length;
	unsigned int i,temp=0;
	char Cflag = 1;
	char Aflag = 1;

	pfile = fopen(str, "rb");
	if (pfile == NULL)
		error_handling("fopen() error!");

	fseek(pfile, 0, SEEK_END);
	file_size = ftell(pfile);
	fseek(pfile, 0, SEEK_SET);

	printf("\033[41m%");

	for (i=0; i < IHDR_START; i++) {
		fread(&byte, sizeof(byte), 1, pfile);
		printf("%02x ", byte);
	}
	printf("\033[0m");

	while (fread(chunk_lenNtype,sizeof(chunk_lenNtype),1,pfile)==1) {
// 청크의 길이와 타입을 출력합니다.
		for (temp = 0; temp < CHUNK_LEN; temp++)
			chunk_length.BYTE[CHUNK_LEN-temp-1] = chunk_lenNtype[temp];

		if (chunk_lenNtype[IS_CRITIC] > KEY_Z) {	//보조타입 청크와 크리티컬타입 청크를 구분하고 색을 정합니다
			if (Aflag == 1) {
				printf("\033[0m");
				printf("\033[33m");
			}
			else {
				printf("\033[0m");
				printf("\033[36m");
			}

			Aflag *= -1;
		}
		else {
			if (Cflag == 1) {
				printf("\033[0m");
				printf("\033[44m");
			}
			else  {
				printf("\033[0m");
				printf("\033[45m");
			}
			Cflag *= -1;
		}

		for (temp = 0; temp < BUF_SIZE_LENANDTYPE; temp++) {
			printf("%02x ", chunk_lenNtype[temp]);
		}

// 청크의 데이터를 읽고 출력합니다.
		for (temp = 0; temp < chunk_length.INT; temp++) {
			if(fread(&byte, sizeof(byte), 1, pfile) == 0)
				error_handling("fread(IDAT) error!");

			printf("%02x ", byte);
		}

// 청크의 CRC를 읽고 출력합니다.
		if(fread(chunk_crc, sizeof(chunk_crc), 1, pfile)==0)
			error_handling("fread(crc) error!");

		for (temp = 0; temp < BUF_SIZE_CRC; temp++) {
			printf("%02x ", chunk_crc[temp]);
		}

	}
	
	printf("\033[0m\n");
	printf("file size : %d\n", file_size);

	fclose(pfile);

	printf("input any key to exit\n");
	_getch();
}

void error_handling(const char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	printf("input any key to exit\n");
	_getch();
	exit(1);
}