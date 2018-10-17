// ApneaRowDataAnalysis.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include	"stdafx.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<time.h>
#include	<direct.h>
#include	<string.h>

/*==============================================================================*/
/*	define定義																	*/
/*==============================================================================*/
#define BUF_SIZE					128

/*==============================================================================*/
/*	グローバル変数																*/
/*==============================================================================*/
double	pdata[BUF_SIZE];
char    path_[17] = "./";
int len = 128;

/*==============================================================================*/
/*	プロトタイプ宣言															*/
/*==============================================================================*/
void debug_out(char *f, const double d[], int size, const char* ppath, int no);
void debug_out_int(char *f, const int d[], int size, const char* ppath, int no);

/*==============================================================================*/
/*	main																		*/
/*==============================================================================*/
int main()
{
	FILE *fp;
	struct tm timeptr;
	char folder[15] = { '\0' };
	time_t timer = 0;
	int i = 0;

	if (fopen_s(&fp, "./raw.txt", "r")) {
		printf("ファイルを開くことが出来ませんでした。\n");
		return 1;
	}

	timer = time(NULL);
	if (localtime_s(&timeptr, &timer))
	{
		fclose(fp);
		return 1;
	}

	strftime(folder, 15, "%Y%m%d%H%M%S", &timeptr);

	if (!strcat_s(path_, sizeof path_, folder)) {
		_mkdir(path_);

		for (i = 0; i < len; i++) {
			fscanf_s(fp, "%lf", &(pdata[i]));     /*  1行読む  */
		}
	}
	fclose(fp);
}

/*==============================================================================*/
/*	debug_out																	*/
/*==============================================================================*/
void	debug_out(char *f, const double d[], int size, const char* ppath, int no)
{
	FILE		*fp;
	char		b[1024];

	sprintf_s(b, sizeof(b), "%s\\%s(%d).txt", ppath, f, no);


	if (fopen_s(&fp, b, "w"))
	{
		printf("file open error [debug_out]\n");
		exit(0);
	}

	for (int i = 0; i < size; i++)
	{
		fprintf(fp, "%lf\n", d[i]);
	}
	fclose(fp);
}

/*==============================================================================*/
/*	debug_out_int																*/
/*==============================================================================*/
void	debug_out_int(char *f, const int d[], int size, const char* ppath, int no)
{
	FILE		*fp;
	char		b[1024];

	sprintf_s(b, sizeof(b), "%s\\%s(%d).txt", ppath, f, no);


	if (fopen_s(&fp, b, "w"))
	{
		printf("file open error [debug_out]\n");
		exit(0);
	}

	for (int i = 0; i < size; i++)
	{
		fprintf(fp, "%d\n", d[i]);
	}
	fclose(fp);
}
/* EOF */
