/************************************************************************/
/* �V�X�e����   : ���ċz����											*/
/* �t�@�C����   : apnea_param.h											*/
/* �@�\         : ���ċz���Z�p�����[�^									*/
/* �ύX����     : 2018.05.16 Axia Soft Design mmura	���ō쐬			*/
/* ���ӎ���     : �Ȃ�                                                  */
/************************************************************************/

#ifndef		_APNEA_PARAM_H_			/* ��d��`�h�~ */
#define		_APNEA_PARAM_H_

/************************************************************/
/* �}�N��													*/
/************************************************************/
// ���f�[�^�␳
//#define APNEA_PARAM_RAW			20000.0f
#define APNEA_PARAM_RAW					1.0f

// �ړ����σf�[�^��
#define APNEA_PARAM_AVE_NUM				5

// �s�[�N�Ԋu50%臒l
#define APNEA_PARAM_PEAK_THRE			0.003f

// �ړ����ςɗp����f�[�^�̐� 0.05 * 90 = 4.5s���̃f�[�^
#define APNEA_PARAM_AVE_CNT				90
// �ړ�����50%臒l
#define APNEA_PARAM_AVE_THRE			0.0015f
// 2�l��50%臒l
#define APNEA_PARAM_BIN_THRE			0.002f

// ���т�臒l
#define APNEA_PARAM_SNORE				0.0125f

// ���S���ċz����臒l
#define APNEA_PARAM_APNEA_THRE			(35 / APNEA_PARAM_RAW)

#endif

/************************************************************/
/* END OF TEXT												*/
/************************************************************/

