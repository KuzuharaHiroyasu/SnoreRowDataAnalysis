// ApneaRowDataAnalysis.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include	"stdafx.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<time.h>
#include	<direct.h>
#include	<string.h>
#include	"getwav.h"
#include	"apnea_param.h"

#ifdef __cplusplus
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllexport)
#endif

/*==============================================================================*/
/*	define定義																	*/
/*==============================================================================*/
#define DATA_SIZE				200	// 10秒間、50msに1回データ取得した数
#define BUF_SIZE				256
#define RIREKI					3
#define CalcDataNumApnea		200

/*==============================================================================*/
/*	グローバル変数																*/
/*==============================================================================*/
double	pdata[BUF_SIZE];
double  raw_[DATA_SIZE];
double	dc_[DATA_SIZE];
int		temp_int_buf0[BUF_SIZE];
char    path_[BUF_SIZE] = "C:/ax/apnea/";
char	tempPath_[BUF_SIZE] = { '\0' };

int		len = CalcDataNumApnea;
int		snore_ = SNORE_OFF;		// いびき

static B	SnoreTime_[RIREKI] = { -1, -1, -1 };
static UB	SnoreFlg_ = OFF; // ONカウント中 or OFFカウント中
static int	SnoreCnt_ = 0; // ON連続回数, OFF連続回数 兼用

/*==============================================================================*/
/*	プロトタイプ宣言															*/
/*==============================================================================*/
void calc_snore_init(void);
void getwav_snore(const double* pData, int DSize, double Param);
static int proc_on(int Pos);
static int proc_off(int Pos);
static void Save(void);
static void Reset(void);
static void Judge(void);
void	debug_out(char *f, const double d[], int size, const char* ppath);
void	debug_out_int(char *f, const int d[], int size, const char* ppath);

/*==============================================================================*/
/*	main																		*/
/*==============================================================================*/
int main()
{
	struct tm timeptr;
	FILE *fp;
	char folder[BUF_SIZE] = { '\0' };
	char dataPath[BUF_SIZE] = { '\0' };
	char str[BUF_SIZE] = { '\0' };
	time_t timer = 0;
	int i, ii = 0;

	//現在時刻取得
	timer = time(NULL);
	if (localtime_s(&timeptr, &timer))
	{
		return 1;
	}

	//現在時刻でフォルダ作成
	strftime(folder, BUF_SIZE, "%Y%m%d%H%M%S", &timeptr);
	if (!strcat_s(path_, sizeof path_, folder)) {
		_mkdir(path_);
	}

	//データフォルダあるだけループ(暫定で2000)
	for (ii = 0; ii < 2000; ii++)
	{
		strcpy_s(dataPath, sizeof dataPath, ".");
		sprintf_s(str, BUF_SIZE, "/%d/", ii);
		strcat_s(dataPath, sizeof dataPath, str);
		strcat_s(dataPath, sizeof dataPath, "rawsnore.txt");

		if (fopen_s(&fp, dataPath, "r")) {
			printf("ファイルを開くことが出来ませんでした。\n");
			break;
		}

		for (i = 0; i < len; i++) {
			fscanf_s(fp, "%lf", &(raw_[i]));     /*  1行読む  */
		}

		//初期化
//		calc_snore_init();
		strcpy_s(tempPath_, sizeof tempPath_, path_);
		strcat_s(tempPath_, sizeof tempPath_, str);
		_mkdir(tempPath_);

		getwav_snore(raw_, len, APNEA_PARAM_SNORE);

		double tmpsnore = (double)snore_;
		
		debug_out("snore_", &tmpsnore, 1, tempPath_);

		fclose(fp);
	}
}

/************************************************************************/
/* 関数     : calc_snore_init											*/
/* 関数名   : 初期化処理												*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2017.07.12 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void calc_snore_init(void)
{
	int ii;

	SnoreFlg_ = OFF;
	SnoreCnt_ = 0;
	snore_ = SNORE_OFF;
	for (ii = 0; ii<RIREKI; ++ii) {
		SnoreTime_[ii] = -1;
	}
}

/************************************************************************/
/* 関数     : getwav_snore												*/
/* 関数名   : いびき演算処理											*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2017.07.12 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
void getwav_snore(const double* pData, int DSize, double Param)
{
	int ii;
	int jj;
	int loop;
	int size;
	int pos = 0;

	// 閾値を超えた回数の移動累計をとる
	loop = DATA_SIZE_APNEA - SNORE_PARAM_SIZE;
	for (ii = 0; ii<loop; ++ii) {
		temp_int_buf0[ii] = 0;
		size = ii + SNORE_PARAM_SIZE;
		for (jj = ii; jj<size; ++jj) {
			if (pData[jj] >= SNORE_PARAM_THRE) {
				temp_int_buf0[ii] += 1;
			}
		}
	}
	debug_out_int("snore_Thre", temp_int_buf0, loop, tempPath_);

	while (pos < loop) {
		switch (SnoreFlg_) {
		case ON:
			pos = proc_on(pos);
			break;
		case OFF:
			pos = proc_off(pos);
			break;
		default:
			calc_snore_init();
			return;
		}
	}
}

/************************************************************************/
/* 関数     : proc_on													*/
/* 関数名   : いびきON時処理											*/
/* 引数     : int Data : 波形データ										*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static int proc_on(int Pos)
{
	int ii;
	int loop = DATA_SIZE_APNEA - SNORE_PARAM_SIZE;
	int pos = loop;

	// OFF確定している場所を特定する
	for (ii = Pos; ii<loop; ++ii) {
		if (temp_int_buf0[ii] <= SNORE_PARAM_OFF_CNT) {
			SnoreFlg_ = OFF;
			pos = ii;
			Save();
			Judge();
			break;
		}
		else {
			SnoreCnt_ += 1;
		}
	}

	return pos;
}

/************************************************************************/
/* 関数     : proc_off													*/
/* 関数名   : いびきOFF時処理											*/
/* 引数     : int Data : 波形データ										*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static int proc_off(int Pos)
{
	int ii;
	int loop = DATA_SIZE_APNEA - SNORE_PARAM_SIZE;
	int pos = loop;

	// ON確定している場所を特定する
	for (ii = Pos; ii<loop; ++ii) {
		if (temp_int_buf0[ii] >= SNORE_PARAM_ON_CNT) {
			SnoreFlg_ = ON;
			SnoreCnt_ = 0;
			pos = ii;
			break;
		}
		else {
			SnoreCnt_ += 1;
			if (SnoreCnt_ >= SNORE_PARAM_NORMAL_CNT) {
				Reset();
			}
		}
	}

	return pos;
}

/************************************************************************/
/* 関数     : Save														*/
/* 関数名   : いびき時間を保存											*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void Save(void)
{
	int ii;

	for (ii = 1; ii<RIREKI; ++ii) {
		SnoreTime_[RIREKI - ii] = SnoreTime_[RIREKI - ii - 1];
	}
	SnoreTime_[0] = SnoreCnt_;
	SnoreCnt_ = 0;
}

/************************************************************************/
/* 関数     : Judge														*/
/* 関数名   : いびき判定												*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void Judge(void)
{
	int ii;

	for (ii = 0; ii<RIREKI; ++ii) {
		if (SnoreTime_[ii] == -1) {
			return;
		}
	}

	for (ii = 0; ii<RIREKI - 1; ++ii) {
		if (abs(SnoreTime_[0] - SnoreTime_[ii + 1]) > SNORE_PARAM_GOSA) {
			return;
		}
	}
	snore_ = SNORE_ON;
}

/************************************************************************/
/* 関数     : Reset														*/
/* 関数名   : 通常呼吸への復帰処理										*/
/* 引数     : なし														*/
/* 戻り値   : なし														*/
/* 変更履歴 : 2018.07.25 Axia Soft Design mmura	初版作成				*/
/************************************************************************/
/* 機能 :																*/
/************************************************************************/
/* 注意事項 :															*/
/* なし																	*/
/************************************************************************/
static void Reset(void)
{
	int ii;

	for (ii = 0; ii<RIREKI; ++ii) {
		SnoreTime_[ii] = -1;
	}
	snore_ = SNORE_OFF;
}

/*==============================================================================*/
/*	debug_out																	*/
/*==============================================================================*/
void	debug_out(char *f, const double d[], int size, const char* ppath)
{
	FILE		*fp;
	char		b[1024];
	errno_t error;

	sprintf_s(b, 1024, "%s\\%s.txt", ppath, f);

	error = fopen_s(&fp, b, "w");
	if (error != 0)
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
void	debug_out_int(char *f, const int d[], int size, const char* ppath)
{
	FILE		*fp;
	char		b[1024];

	sprintf_s(b, sizeof(b), "%s\\%s.txt", ppath, f);


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
