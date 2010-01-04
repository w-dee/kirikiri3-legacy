//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
class tSSABlock;
class tSSAForm;
class tCompiler;
//---------------------------------------------------------------------------
/**
 * ローカル変数にアクセスがあったかどうかを記録するためのマップクラス
 */
class tSSAVariableAccessMap : public tCollectee
{
	/**
	 * 名前に関する情報
	 */
	struct tInfo : public tCollectee
	{
		tInfo() { Read = false; Write = false; TempVariable = NULL; } //!< コンストラクタ
		bool Read;		//!< この変数に対する読み込みが発生したかどうか(使用フラグ)
		bool Write;		//!< この変数に対する書き込みが発生したかどうか(使用フラグ)
		tString Id; //!< 番号・アクセスマップID付きの名前
		tSSAVariable * TempVariable; //!< この変数に割り当てられた childwrite 用の一時変数
	};
	typedef gc_map<tString, tInfo> tMap; //!< 変数名(番号・アクセスマップIDあり)→情報のマップのtypedef

	tMap Map; //!< 変数名(番号・アクセスマップIDあり)→情報のマップ

	tSSAVariable * Variable; //!< このアクセスマップを表すSSA変数

public:
	/**
	 * コンストラクタ
	 * @param form	SSA形式インスタンス
	 * @param pos	スクリプト上の位置
	 */
	tSSAVariableAccessMap(tSSAForm * form, risse_size pos);

	/**
	 * このアクセスマップを表すSSA変数を返す
	 * @return	このアクセスマップを表すSSA変数
	 */
	tSSAVariable * GetVariable() const { return Variable; }

	/**
	 * アクセスマップに追加する
	 * @param name		変数名(番号なし)
	 * @param q_name	変数名(番号・アクセスマップID付き)
	 * @param write		その変数に対するアクセスが書き込みか(真)、読み込みか(偽)
	 */
	void SetUsed(const tString & name, const tString & q_name, bool write);

	/**
	 * 遅延評価ブロック中で「書き込み」が発生した変数に対して読み込みを行う文を作成する
	 * @param form	SSA形式インスタンス
	 * @param pos	スクリプト上の位置
	 */
	void GenerateChildWrite(tSSAForm * form, risse_size pos);

	/**
	 * 遅延評価ブロック中で「読み込み」が発生した変数に対して読み込みを行う文を作成する
	 * @param form	SSA形式インスタンス
	 * @param pos	スクリプト上の位置
	 */
	void GenerateChildRead(tSSAForm * form, risse_size pos);

	/**
	 * アクセスマップの終了を表す ocEndAccessMap を追加する
	 * @param form	SSA形式インスタンス
	 * @param pos	スクリプト上の位置
	 */
	void GenerateEndAccessMap(tSSAForm * form, risse_size pos);

	/**
	 * ID文字列を得る
	 */
	tString GetIdString() const { return Variable->GetQualifiedName(); }

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ローカル変数用の階層化された名前空間管理クラス
 */
class tSSALocalNamespace : public tCollectee
{
	tCompiler * Compiler; //!< この名前空間に結びつけられている基本ブロック
				//!< たとえ Block が null でも、 ユニークな値を得るために必要。
	tSSABlock * Block; //!< この名前空間に結びつけられている基本ブロック
	tSSAVariableAccessMap * AccessMap;
		//!< この名前空間内に見つからなかった読み込みあるいは書き込みをチェックするためのマップ
		//!< この名前空間内に見つからなかった変数は親名前空間内で検索される。見つかった場合は
		//!< AccessMapが NULL の場合は、親名前空間内で共有されるが、
		//!< AccessMapが非 NULL の場合は共有されずに AccessMap にマッピングが追加される
	tSSALocalNamespace * Parent; //!< チェーンされた親名前空間
public:
	typedef gc_map<tString, tSSAVariable *> tVariableMap;
		//!< 変数名(番号付き)→変数オブジェクトのマップのtypedef
	typedef gc_map<tString, tString> tAliasMap;
		//!< 変数名(番号なし)→変数名(番号付き)のマップのtypedef
private:

	/**
	 * 名前空間の一つのスコープを表す構造体
	 */
	struct tScope : public tCollectee
	{
		tVariableMap VariableMap; //!< 変数名(番号付き)→変数オブジェクトのマップ
		tAliasMap AliasMap; //!< 変数名(番号なし)→変数名(番号付き)のマップ
	};
	typedef gc_vector<tScope *> tScopes; //!< スコープの typedefs
	tScopes Scopes; //!< 名前空間のスコープ

public:
	/**
	 * コンストラクタ
	 */
	tSSALocalNamespace();

	/**
	 * コピーコンストラクタ
	 */
	tSSALocalNamespace(const tSSALocalNamespace &ref);

	/**
	 * チェーンされた親名前空間を設定する
	 * @param parent	チェーンされた親名前空間
	 */
	void SetParent(tSSALocalNamespace * parent) { Parent = parent; }

	/**
	 * 番号 付き変数名を得る
	 * @param name	変数名
	 * @param num	番号
	 * @return	"name#num" 形式の文字列
	 */
	static tString GetNumberedName(const tString & name, risse_int num);

	/**
	 * この名前空間に結びつけられる基本ブロックを設定する
	 * @param block	この名前空間に結びつけられる基本ブロック
	 * @note	このメソッドはついでに Compiler も設定するので注意。
	 */
	void SetBlock(tSSABlock * block);

	/**
	 * この名前空間に結びつけられているコンパイラインスタンスを設定する
	 * @param compiler	この名前空間に結びつけられている基本ブロック
	 */
	void SetCompiler(tCompiler * compiler);

	/**
	 * 名前空間を push する
	 */
	void Push();

	/**
	 * 名前空間を pop する
	 */
	void Pop();

	/**
	 * スコープを持っているかどうかを得る
	 * @return	スコープを持っているかどうか
	 * @note	スコープを持っていないということはグローバル位置にスクリプトがあるという
	 *			ことを意味する @n
	 *			一番浅い位置にある名前空間は相手にしない。一番浅い位置の名前空間は自動的に
	 *			作成されるが、特殊な変数のみ(superなど)がそこに記録される。
	 */
	bool GetHasScope() const { return Scopes.size() > 1; }

	/**
	 * 変数を定義する
	 * @param name	変数名
	 * @param where	その変数を表す SSA 変数表現
	 */
	void Add(const tString & name, tSSAVariable * where);

	/**
	 * 変数を探す
	 * @param name		変数名
	 * @param is_num	name は番号付きの名前か(真)、なしのなまえか(偽)
	 * @param n_name	is_num が偽の場合、番号付きの名前が欲しい場合はここにそれを受け取る
	 *					変数を指定する
	 * @param var		その変数を表す SSA 変数表現 を格納する先(NULL = 要らない)
	 * @return	変数が見つかったかどうか
	 * @note	変数が見つからなかった場合は *var にはなにも書き込まれない.
	 *			name は番号なしの変数名であると見なされる.
	 */
	bool Find(const tString & name, bool is_num = false, tString *n_name = NULL, tSSAVariable *** var = NULL) const;

	/**
	 * 変数が存在するかどうかを調べる
	 * @param name	変数名
	 * @return	変数が見つかったかどうか
	 *			name は番号なしの変数名であると見なされる.
	 *			このメソッドは「チェーンされた」名前空間も検索し、そこに変数があるならば
	 *			真を返す
	 */
	bool IsAvailable(const tString & name) const;

	/**
	 * 変数を削除する
	 * @param name	変数名
	 * @return	変数の削除に成功したかどうか
	 * @note	name は番号なしの変数名であると見なされる
	 */
	bool Delete(const tString & name);

	/**
	 * 必要ならばφ関数を作成する
	 * @param pos		スクリプト上の位置
	 * @param name		変数名
	 * @param n_name	番号付き変数名
	 * @return	見つかった変数、あるいはφ関数の戻り値へのポインタ
	 *			(NULL=ローカル変数に見つからなかった)
	 */
	tSSAVariable * MakePhiFunction(risse_size pos,
		const tString & name, const tString & n_name = tString::GetEmptyString());

	/**
	 * 必要ならばφ関数を作成する
	 * @param pos		スクリプト上の位置
	 * @param name		変数名
	 * @param n_name	番号付き変数名
	 * @return	見つかった変数、あるいはφ関数の戻り値へのポインタ
	 *			(NULL=ローカル変数に見つからなかった)
	 */
	tSSAVariable * AddPhiFunctionForBlock(risse_size pos,
		const tString & name, const tString & n_name = tString::GetEmptyString());

	/**
	 * 変数をすべて「φ関数を参照のこと」としてマークする
	 * @note	このメソッドは、Scopes のすべてのマップの値を
	 *			NULL に設定する(NULL=変数は存在しているがφ関数を作成する必要がある
	 *			という意味)
	 */
	void MarkToCreatePhi();

	/**
	 * 変数に読み込みのためのアクセスをする(必要ならばφ関数などが作成される)
	 * @param pos	スクリプト上の位置
	 * @param name	変数名
	 * @return	見つかった変数、あるいはφ関数の戻り値など (NULL=ローカル変数に見つからなかった)
	 * @note	このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	 *			チェーンされた名前空間へアクセスするための文を作成する
	 */
	tSSAVariable * Read(risse_size pos, const tString & name);

	/**
	 * 変数に書き込みのためのアクセスをする(必要ならばφ関数などが作成される)
	 * @param form	SSA形式インスタンス
	 * @param pos	スクリプト上の位置
	 * @param name	変数名
	 * @param value	書き込む値を保持しているSSA変数
	 * @return	書き込み成功したか(変数が見つからないなど、書き込みに失敗した場合は偽)
	 * @note	このメソッドは、「チェーンされた」名前空間も検索し、そこに変数があるならば
	 *			チェーンされた名前空間へアクセスするための文を作成する
	 */
	bool Write(risse_size pos, const tString & name,
				tSSAVariable * value);

	/**
	 * 子名前空間から呼ばれ、変数に読み込み/書き込みのためのアクセスをする
	 * @param name			変数名
	 * @param access		読み(偽)か書き(真)か
	 * @param should_share	見つかった変数を共有すべきかどうか
	 * @param child			子名前空間
	 * @param ret_n_name	見つかった番号付き変数名を格納する先 (NULL = いらない)
	 * @param is_shared		実際に見つかった変数を共有した場合真、それ以外偽が入る(NULL = この情報はいらない)
	 * @return	変数が見つかったかどうか
	 */
	bool AccessFromChild(const tString & name, bool access,
		bool should_share, tSSALocalNamespace * child,
		tString * ret_n_name = NULL, bool * is_shared = NULL);

	/**
	 * AccessMap を設定する
	 * @param map	AcecssMap
	 */
	void SetAccessMap(tSSAVariableAccessMap * map) { AccessMap = map; }

	/**
	 * AccessMap を取得する @return AcecssMap
	 */
	tSSAVariableAccessMap * GetAccessMap() const { return AccessMap; }

private:
	/**
	 * すべての「可視な」番号付き変数名をリストアップする
	 * @param map	格納先
	 * @note	この関数は、親名前空間がある場合、親名前空間に対して再帰する。
	 */
	void InternalListAllVisibleVariableNumberedNames(tAliasMap & map) const;

public:
	/**
	 * すべての「可視な」番号付き変数名をリストアップする
	 * @param dest	格納先(内容はクリアされる)
	 * @note	ここで得られる変数名は、番号付きの名前である。
	 */
	void ListAllVisibleVariableNumberedNames(tAliasMap & dest) const;


private:
	/**
	 * すべての「可視な」変数を共有変数としてマークする(内部関数)
	 * @param names	既にマークした関数のマップ
	 */
	void InternalShareAllVisibleVariableNames(tAliasMap & names) const;

public:
	/**
	 * すべての「可視な」変数を共有変数としてマークする
	 */
	void ShareAllVisibleVariableNames() const;

	/**
	 * 内容のダンプを行う
	 * @return	ダンプされた文字列
	 */
	tString Dump() const;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
#endif

