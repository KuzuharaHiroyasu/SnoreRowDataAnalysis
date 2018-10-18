// ApneaRowDataAnalysis.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
/*	define��`																	*/
/*==============================================================================*/
#define DATA_SIZE				200	// 10�b�ԁA50ms��1��f�[�^�擾������
#define BUF_SIZE				256
#define RIREKI					3

/*==============================================================================*/
/*	�O���[�o���ϐ�																*/
/*==============================================================================*/
double	pdata[BUF_SIZE];
double  raw_[DATA_SIZE];
double	dc_[DATA_SIZE];
double	movave_[DATA_SIZE];
int		temp_int_buf0[BUF_SIZE];
char    path_[17] = "./";

int		len = 128;
int		snore_ = SNORE_OFF;		// ���т�

static B	SnoreTime_[RIREKI];
static UB	SnoreFlg_; // ON�J�E���g�� or OFF�J�E���g��
static UB	SnoreCnt_; // ON�A����, OFF�A���� ���p

/*==============================================================================*/
/*	�v���g�^�C�v�錾															*/
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
	double* ptest1 = NULL;
	char folder[15] = { '\0' };
	time_t timer = 0;
	int i = 0;

	ptest1 = (double*)calloc(len, sizeof(double));

	if (!ptest1) {
		return 1;
	}

	if (fopen_s(&fp, "./movave.txt", "r")) {
		printf("�t�@�C�����J�����Ƃ��o���܂���ł����B\n");
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
			fscanf_s(fp, "%lf", &(movave_[i]));     /*  1�s�ǂ�  */
		}
	}

	for (int ii = 0; ii<len; ++ii) {
		ptest1[ii] = movave_[ii] / APNEA_PARAM_RAW;
	}

	calc_snore_init();

	getwav_snore(ptest1, len, APNEA_PARAM_SNORE);
	
	double tmpsnore = (double)snore_;
	debug_out("snore_", &tmpsnore, 1, path_);

	free(ptest1);
	fclose(fp);
}

/************************************************************************/
/* �֐�     : calc_snore_init											*/
/* �֐���   : ����������												*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2017.07.12 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
/* �֐�     : getwav_snore												*/
/* �֐���   : ���т����Z����											*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2017.07.12 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
void getwav_snore(const double* pData, int DSize, double Param)
{
	int ii;
	int jj;
	int loop;
	int size;
	int pos = 0;

	// 臒l�𒴂����񐔂̈ړ��݌v���Ƃ�
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
	debug_out_int("snore_Thre", temp_int_buf0, loop, path_);

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
/* �֐�     : proc_on													*/
/* �֐���   : ���т�ON������											*/
/* ����     : int Data : �g�`�f�[�^										*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static int proc_on(int Pos)
{
	int ii;
	int loop = DATA_SIZE_APNEA - SNORE_PARAM_SIZE;
	int pos = loop;

	// OFF�m�肵�Ă���ꏊ����肷��
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
/* �֐�     : proc_off													*/
/* �֐���   : ���т�OFF������											*/
/* ����     : int Data : �g�`�f�[�^										*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
/************************************************************************/
static int proc_off(int Pos)
{
	int ii;
	int loop = DATA_SIZE_APNEA - SNORE_PARAM_SIZE;
	int pos = loop;

	// ON�m�肵�Ă���ꏊ����肷��
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
/* �֐�     : Save														*/
/* �֐���   : ���т����Ԃ�ۑ�											*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
/* �֐�     : Judge														*/
/* �֐���   : ���т�����												*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
/* �֐�     : Reset														*/
/* �֐���   : �ʏ�ċz�ւ̕��A����										*/
/* ����     : �Ȃ�														*/
/* �߂�l   : �Ȃ�														*/
/* �ύX���� : 2018.07.25 Axia Soft Design mmura	���ō쐬				*/
/************************************************************************/
/* �@�\ :																*/
/************************************************************************/
/* ���ӎ��� :															*/
/* �Ȃ�																	*/
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
