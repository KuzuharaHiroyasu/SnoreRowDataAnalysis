/************************************************************************/
/* システム名   : 無呼吸判定											*/
/* ファイル名   : getwav.h												*/
/* 機能         : inputファイルの読み込み								*/
/* 変更履歴     : 2017.07.12 Axia Soft Design mmura	初版作成			*/
/* 注意事項     : なし                                                  */
/************************************************************************/

#ifdef __cplusplus
#define DLLEXPORT extern "C" __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllexport)
#endif

#ifndef		_GETWAV_H_			/* 二重定義防止 */
#define		_GETWAV_H_

#include "sys.h"


/************************************************************/
/* マクロ													*/
/************************************************************/
#define DATA_SIZE_APNEA		(200)
#define SNORE_PARAM_SIZE	(10)
#define SNORE_PARAM_THRE	(256)

// いびき判定結果
#define SNORE_OFF		0	// いびきなし
#define SNORE_ON		1	// いびきあり

// OFF判定回数
#define SNORE_PARAM_OFF_CNT				(0)
// ON判定回数
#define SNORE_PARAM_ON_CNT				(8)
// いびきあり -> なしへの判定回数
#define SNORE_PARAM_NORMAL_CNT			(290)
// 許容誤差 0.4s
#define SNORE_PARAM_GOSA				(8)

/************************************************************/
/* 型定義													*/
/************************************************************/

/************************************************************/
/* 外部参照宣言												*/
/************************************************************/
// 演算実行
DLLEXPORT void __stdcall getwav_init(int* data, int len, char* ppath, int* psnore);
// DC成分除去データ
DLLEXPORT void __stdcall getwav_dc(double* pdata);

// 無呼吸演算
DLLEXPORT void __stdcall get_apnea_ave(double* pdata);
DLLEXPORT void __stdcall get_apnea_eval(int* pdata);
DLLEXPORT void __stdcall get_apnea_rms(double* pdata);
DLLEXPORT void __stdcall get_apnea_point(double* pdata);
// いびき演算
DLLEXPORT void __stdcall get_snore_xy2(double* pdata);
DLLEXPORT void __stdcall get_snore_interval(double* pdata);
// 結果
DLLEXPORT int  __stdcall get_state(void);
#endif

/************************************************************/
/* END OF TEXT												*/
/************************************************************/

