/************************************************************************/
/* システム名   : 無呼吸判定											*/
/* ファイル名   : getwav.h												*/
/* 機能         : inputファイルの読み込み								*/
/* 変更履歴     : 2017.07.12 Axia Soft Design mmura	初版作成			*/
/* 注意事項     : なし                                                  */
/************************************************************************/

#ifndef		_GETWAV_H_			/* 二重定義防止 */
#define		_GETWAV_H_

#include "sys.h"

/************************************************************/
/* マクロ													*/
/************************************************************/
#define DATA_SIZE_APNEA		(200)
#define SNORE_PARAM_SIZE	(10)
//#define SNORE_PARAM_THRE	(256)
#define SNORE_PARAM_THRE	(350)

// いびき判定結果
#define SNORE_OFF		0	// いびきなし
#define SNORE_ON		1	// いびきあり

// OFF判定回数
#define SNORE_PARAM_OFF_CNT				(0)
// ON判定回数
#define SNORE_PARAM_ON_CNT				(8)
// いびきあり -> なしへの判定回数
//#define SNORE_PARAM_NORMAL_CNT			(290)
#define SNORE_PARAM_NORMAL_CNT			(80)
// 許容誤差 0.4s
#define SNORE_PARAM_GOSA				(8)

#endif

/************************************************************/
/* END OF TEXT												*/
/************************************************************/

