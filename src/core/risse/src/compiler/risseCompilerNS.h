//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ用名前空間
//---------------------------------------------------------------------------
#ifndef risseCompilerNSH
#define risseCompilerNSH

#include "risseCompilerNS.h"
#include "risseSSAVariable.h"

namespace Risse
{
class tRisseSSABlock;
class tRisseSSAForm;
//---------------------------------------------------------------------------
//! @brief	ローカル変数にアクセスがあったかどうかを記録するためのマップクラス
//---------------------------------------------------------------------------
class tRisseSSAVariableAccessMap : public tRisseCollectee
{
	//! @brief		名前に関する情報
	struct tInfo : public tRisseCollectee
	{
		tInfo() { Read = false; Write = false; } //!< コンストラクタ
		bool Read;		//!< この変数に対する読み込みが発生したかどうか(使用フラグ)
		bool Write;		//!< この変数に対する書き込みが発生したかどうか(使用フラグ)
	};
	typedef gc_map<tRisseString, tInfo> tMap; //!< 変数名(番号なし)→情報のマップのtypedef

	tMap Map; //!< 変数名(番号なし)→情報のマップ

public:
	//! @brief		コンストラクタ
	tRisseSSAVariableAccessMap() {;}

	//! @brief		アクセスマップに追加する
	//! @param		name		変数名(番号なし)
	//! @param		write		その変数に対するアクセスが書き込みか(真)、読み込みか(偽)
	void SetUsed(const tRisseString & name, bool write);

	//! @param		遅延評価ブロック中で「読み込み」が発生した変数に対して読み込みを行う文を作成する
	//! @param		form		SSA形式インスタンス
	//! @param		pos			スクリプト上の位置
	//! @param		block_var	遅延評価ブロックを表す変数
	void GenerateChildRead(tRisseSSAForm * form, risse_size pos,
		tRisseSSAVariable* block_var);

	//! @param		遅延評価ブロック中で「書き込み」が発生した変数に対して読み込みを行う文を作成する
	//! @param		form		SSA形式インスタンス
	//! @param		pos			スクリプト上の位置
	//! @param		block_var	遅延評価ブロックを表す変数
	void GenerateChildWrite(tRisseSSAForm * form, risse_size pos,
		tRisseSSAVariable* block_var);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	ローカル変数用の階層化された名前空間管理クラス
//---------------------------------------------------------------------------
class tRisseSSALocalNamespace : public tRisseCollectee
{
	tRisseSSABlock * Block; //!< この名前空間に結びつけられている基本ブロック
	tRisseSSAVariableAccessMap * AccessMap;
		//!< この名前空間内に見つからなかった読み込みあるいは書き込みをチェックするためのマップ
		//!< この名前空間内に見つからなかった変数は親名前空間内で検索される。見つかった場合は
		//!< AccessMapが NULL の場合は、親名前空間内で pin されるが、
		//!< AccessMapが非 NULL の場合はピンされずに AccessMap にマッピングが追加される
	tRisseSSALocalNamespace * Parent; //!< チェーンされた親名前空間
	typedef gc_map<tRisseString, tRisseSSAVariable *> tVariableMap;
		//!< 変数名(番号付き)→変数オブジェクトのマップのtypedef
	typedef gc_map<tRisseString, tRisseString> tAliasMap;
		//!< 変数名(番号なし)→変数名(番号付き)のマップのtypedef

	//! @brief		名前空間の一つのスコープを表す構造体
	struct tScope : public tRisseCollectee
	{
		tVariableMap VariableMap; //!< 変数名(番号付き)→変数オブジェクトのマップ
		tAliasMap AliasMap; //!< 変数名(番号なし)→変数名(番号付き)のマップ
	};
	typedef gc_vector<tScope *> tScopes; //!< スコープの typedefs
	tScopes Scopes; //!< 名前空間のスコープ

public:
	//! @brief		コンストラクタ
	tRisseSSALocalNamespace();

	//! @brief		コピーコンストラクタ
	tRisseSSALocalNamespace(const tRisseSSALocalNamespace &ref);

	//! @brief		チェーンされた親名前空間を設定する
	//! @param		parent		チェーンされた親名前空間
	void SetParent(tRisseSSALocalNamespace * parent) { Parent = parent; }

	//! @brief		番号 付き変数名を得る
	//! @param		name		変数名
	//! @param		num			番号
	//! @return		"name#num" 形式の文字列
	static tRisseString GetNumberedName(const tRisseString & name, risse_int num);

	//! @brief		この名前空間に結びつけられる基本ブロックを設定する
	//! @param		block	この名前空間に結びつけられる基本ブロック
	void SetBlock(tRisseSSABlock * block) { Block = block; }

	//! @brief		名前空間を push する
	void Push();

	//! @brief		名前空間を pop する
	void Pop();

	//! @brief		変数を定義する
	//! @param		name		変数名
	//! @param		where		その変数を表す SSA 変数表現
	void Add(const tRisseString & name, tRisseSSAVariable * where);

	//! @brief		変数を探す
	//! @param		name		変数名
	//! @param		is_num		name は番号付きの名前か(真)、なしのなまえか(偽)
	//! @param		n_name		is_num が偽の場合、番号付きの名前が欲しい場合はここにそれを受け取る
	//!							変数を指定する
	//! @param		var 		その変数を表す SSA 変数表現 を格納する先(NULL = 要らない)
	//! @return		変数が見つかったかどうか
	//! @note		変数が見つからなかった場合は *var にはなにも書き込まれない.
	//!				name は番号なしの変数名であると見なされる.
	bool Find(const tRisseString & name, bool is_num = false, tRisseString *n_name = NULL, tRisseSSAVariable *** var = NULL) const;

	//! @brief		変数が存在するかどうかを調べる
	//! @param		name		変数名
	//! @return		変数が見つかったかどうか
	//!				name は番号なしの変数名であると見なされる.
	//!				このメソッドは「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				真を返す
	bool IsAvailable(const tRisseString & name) const;

	//! @brief		変数を削除する
	//! @param		name		変数名
	//! @return		変数の削除に成功したかどうか
	//!	@note		name は番号なしの変数名であると見なされる
	bool Delete(const tRisseString & name);

	//! @brief		必要ならばφ関数を作成する
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @param		n_name	番号付き変数名
	//! @return		見つかった変数、あるいはφ関数の戻り値へのポインタ
	//!				(NULL=ローカル変数に見つからなかった)
	tRisseSSAVariable * MakePhiFunction(risse_size pos,
		const tRisseString & name, const tRisseString & n_name = tRisseString());

	//! @brief		必要ならばφ関数を作成する
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @param		n_name	番号付き変数名
	//! @return		見つかった変数、あるいはφ関数の戻り値へのポインタ
	//!				(NULL=ローカル変数に見つからなかった)
	tRisseSSAVariable * AddPhiFunctionForBlock(risse_size pos,
		const tRisseString & name, const tRisseString & n_name = tRisseString());

	//! @brief		変数をすべて「φ関数を参照のこと」としてマークする
	//! @note		このメソッドは、Scopes のすべてのマップの値を
	//!				NULL に設定する(NULL=変数は存在しているがφ関数を作成する必要がある
	//!				という意味)
	void MarkToCreatePhi();

	//! @brief		変数に読み込みのためのアクセスをする(必要ならばφ関数などが作成される)
	//! @param		form	SSA形式インスタンス
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @return		見つかった変数、あるいはφ関数の戻り値など (NULL=ローカル変数に見つからなかった)
	//! @note		このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				チェーンされた名前空間へアクセスするための文を作成する
	tRisseSSAVariable * Read(tRisseSSAForm * form, risse_size pos, const tRisseString & name);

	//! @brief		変数に書き込みのためのアクセスをする(必要ならばφ関数などが作成される)
	//! @param		form	SSA形式インスタンス
	//! @param		pos		スクリプト上の位置
	//! @param		name	変数名
	//! @param		value	書き込む値を保持しているSSA変数
	//! @return		書き込み成功したか(変数が見つからないなど、書き込みに失敗した場合は偽)
	//! @note		このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	//!				チェーンされた名前空間へアクセスするための文を作成する
	bool Write(tRisseSSAForm * form, risse_size pos, const tRisseString & name,
				tRisseSSAVariable * value);

	//! @brief		子名前空間から呼ばれ、変数に読み込み/書き込みのためのアクセスをする
	//! @param		name		変数名
	//! @param		access		読み(偽)か書き(真)か
	//! @param		should_pin	見つかった変数を「ピン」すべきかどうか
	//! @param		child		子名前空間
	//! @param		ret_n_name	見つかった番号付き変数名を格納する先 (NULL = いらない)
	//! @return		変数が見つかったかどうか
	bool AccessFromChild(const tRisseString & name, bool access,
		bool should_pin, tRisseSSALocalNamespace * child,
		tRisseString * ret_n_name = NULL);

	//! @brief		AccessMap を作成する (すでに存在する場合でも新規に作成する)
	//! @return		新しく作成した AcecssMap
	tRisseSSAVariableAccessMap * CreateAccessMap();

	//! @brief		AccessMap を取得する @return AcecssMap
	tRisseSSAVariableAccessMap * GetAccessMap() const { return AccessMap; }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

