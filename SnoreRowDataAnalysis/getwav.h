/************************************************************************/
/* �V�X�e����   : ���ċz����											*/
/* �t�@�C����   : getwav.h												*/
/* �@�\         : input�t�@�C���̓ǂݍ���								*/
/* �ύX����     : 2017.07.12 Axia Soft Design mmura	���ō쐬			*/
/* ���ӎ���     : �Ȃ�                                                  */
/************************************************************************/

#ifndef		_GETWAV_H_			/* ��d��`�h�~ */
#define		_GETWAV_H_

#include "sys.h"

/************************************************************/
/* �}�N��													*/
/************************************************************/
#define DATA_SIZE_APNEA		(200)
#define SNORE_PARAM_SIZE	(10)
//#define SNORE_PARAM_THRE	(256)
#define SNORE_PARAM_THRE	(350)

// ���т����茋��
#define SNORE_OFF		0	// ���т��Ȃ�
#define SNORE_ON		1	// ���т�����

// OFF�����
#define SNORE_PARAM_OFF_CNT				(0)
// ON�����
#define SNORE_PARAM_ON_CNT				(8)
// ���т����� -> �Ȃ��ւ̔����
//#define SNORE_PARAM_NORMAL_CNT			(290)
#define SNORE_PARAM_NORMAL_CNT			(80)
// ���e�덷 0.4s
#define SNORE_PARAM_GOSA				(8)

#endif

/************************************************************/
/* END OF TEXT												*/
/************************************************************/

