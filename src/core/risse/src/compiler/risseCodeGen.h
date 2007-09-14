//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#ifndef risseCodeGenH
#define risseCodeGenH

#include "../risseGC.h"
#include "../risseCharUtils.h"
#include "../risseTypes.h"
#include "risseAST.h"
#include "../risseVariant.h"
#include "../risseOpCodes.h"

namespace Risse
{

class tSSABlock;
class tSSAForm;
//---------------------------------------------------------------------------
//! @brief		コードジェネレータクラス
//---------------------------------------------------------------------------
class tCodeGenerator : public tCollectee
{
	static const risse_size MaxConstSearch = 5; // 定数領域で同じ値を探す最大値

	tSSAForm * Form; //!< このコードジェネレータを作成した SSA 形式インスタンス
	tCodeGenerator * Parent; //!< 親のコードジェネレータ
	bool UseParentFrame; //!< 親のコードジェネレータのフレームを使うかどうか
	risse_size NestLevel;		//!< 関数のネストレベル
	risse_size RegisterBase; //!< レジスタの基本値

	gc_vector<risse_uint32> Code; //!< コード
	gc_vector<tVariant> Consts; //!< 定数領域
	gc_vector<risse_size> RegFreeMap; // 空きレジスタの配列
	risse_size NumUsedRegs; // 使用中のレジスタの数
	risse_size MaxNumUsedRegs; // 使用中のレジスタの最大数
public:
	typedef gc_map<tString, risse_size> tNamedRegMap;
		//!< 変数名とそれに対応するレジスタ番号のマップのtypedef
private:
	tNamedRegMap *SharedRegNameMap; //!< 共有変数名とそれに対応するレジスタ番号のマップ(一連の関数グループ内ではこれを共有する)
	risse_size SharedRegCount; //!< このコードジェネレータのネストレベルに対する共有変数の数を返す

	tNamedRegMap VariableMapForChildren; //!< 親コードジェネレータが子ジェネレータに対して提供する変数のマップ
	//! @brief		未解決のジャンプを表す構造体
	struct tPendingBlockJump : public tCollectee
	{
		const tSSABlock *	Block; //!< 基本ブロック
		risse_size			EmitPosition; //!< オフセットを入れ込む位置
		risse_size			InsnPosition; //!< 命令位置
		tPendingBlockJump(const tSSABlock * block, risse_size emit_pos, risse_size insn_pos)
			: Block(block), EmitPosition(emit_pos), InsnPosition(insn_pos) {;}
	};
	gc_vector<tPendingBlockJump> PendingBlockJumps;
			//!< 未解決のジャンプとその基本ブロックのリスト
	typedef gc_map<const tSSABlock *, risse_size> tBlockMap;
			//!< 基本ブロックとそれが対応するアドレスの typedef
	tBlockMap BlockMap; //!< 変数とそれに対応するレジスタ番号のマップ

	gc_vector<std::pair<risse_size, risse_size> > CodeBlockRelocations; // 他のコードブロックの再配置情報用配列
	gc_vector<std::pair<risse_size, risse_size> > TryIdentifierRelocations; // Try識別子の再配置情報用配列
	gc_vector<std::pair<risse_size, risse_size> > CodeToSourcePosition; //!< コード位置とソースコード上の位置の対応を表すペアの配列

public:
	//! @brief		コンストラクタ
	//! @param		form			このコードジェネレータを作成した SSA 形式インスタンス
	//! @param		parent			親コードジェネレータ
	//! @param		useparentframe	親コードジェネレータのフレームを使うかどうか
	//! @param		nestlevel		関数のネストレベル
	tCodeGenerator(tSSAForm * form, tCodeGenerator * parent, bool useparentframe, risse_size nestlevel);

public:
	//! @brief	親のコードジェネレータを得る
	//! @return	親のコードジェネレータを得る
	tCodeGenerator * GetParent() const { return Parent; }

	//! @brief	レジスタの基本値を得る @return レジスタの基本値
	risse_size GetRegisterBase() const { return RegisterBase; }

	//! @brief	レジスタの基本値を設定する(親コードジェネレータから値を得る)
	void SetRegisterBase();

	//! @brief	コード配列を得る @return コード配列
	const gc_vector<risse_uint32> & GetCode() const { return Code; }

	//! @brief	定数領域の配列を得る @return 定数領域の配列
	const gc_vector<tVariant> & GetConsts() const { return Consts; }

	//! @brief	ネストレベルを得る @return ネストレベル
	risse_size GetNestLevel() const { return NestLevel; }

	//! @brief	使用中のレジスタの最大数を得る @return 使用中のレジスタの最大数
	risse_size GetMaxNumUsedRegs() const { return MaxNumUsedRegs; }

	//! @brief	他のコードブロックの再配置情報用配列を得る @return 他のコードブロックの再配置情報用配列
	const gc_vector<std::pair<risse_size, risse_size> > & GetCodeBlockRelocations() const
		{ return CodeBlockRelocations; }

	//! @brief	Try識別子の再配置情報用配列を得る @return Try識別子の再配置情報用配列
	const gc_vector<std::pair<risse_size, risse_size> > & GetTryIdentifierRelocations() const
		{ return TryIdentifierRelocations; }

	//! @brief コード位置とソースコード上の位置の対応を表すペアの配列を得る @return コード位置とソースコード上の位置の対応を表すペアの配列
	const gc_vector<std::pair<risse_size, risse_size> > & GetCodeToSourcePosition() const
		{ return CodeToSourcePosition; }

protected:
	//! @param		コードを1ワード分置く
	//! @param		r コード
	void PutWord(risse_uint32 r) { Code.push_back(r); }

public:
	//! @brief		基本ブロックとアドレスのマップを追加する
	//! @param		block		基本ブロック
	//! @note		アドレスとしては現在の命令書き込み位置が用いられる
	void AddBlockMap(const tSSABlock * block);

	//! @brief		未解決のジャンプとその基本ブロックを追加する
	//! @param		block		基本ブロック
	//! @param		insn_pos	ジャンプ命令の開始位置
	//! @note		ジャンプ先アドレスを入れ込むアドレスとしては現在の命令書き込み位置が用いられる
	void AddPendingBlockJump(const tSSABlock * block, risse_size insn_pos);

	//! @brief		定数領域から値を見つけ、その値を返す
	//! @param		value		定数値
	//! @return		その定数のインデックス
	risse_size FindConst(const tVariant & value);

	//! @brief		変数からレジスタ番号を得る
	//! @param		var			変数
	//! @return		そのレジスタのインデックス
	risse_size GetRegNum(const tSSAVariable * var);

	//! @brief		使用中のレジスタの最大値を更新する
	//! @param		max		レジスタの最大値
	//! @note		UseParentFrameが真の場合は、親のコードブロックと子
	//!				のコードブロックはスタックフレームを共有するため、
	//!				このメソッドは子から親へ再帰的に呼ばれる(結果、子と親の
	//!				MaxNumUsedRegs は同一になる)
	void UpdateMaxNumUsedRegs(risse_size max);

	//! @brief		共有されたレジスタのマップを変数名で探す
	//! @param		name			変数名
	//! @param		nestlevel		そのレジスタのネストレベル
	//! @param		regnum			レジスタ番号
	//! @note		nameがマップ内に見つからなかった場合は(デバッグモード時は)
	//!				親コードジェネレータを探し、それでも見つからなければASSERTに失敗となる
	void FindSharedRegNameMap(const tString & name, risse_uint16 &nestlevel, risse_uint16 &regnum);

	//! @brief		共有されたレジスタのマップに変数名を追加する
	//! @param		name			変数名
	void AddSharedRegNameMap(const tString & name);

	//! @brief		共有されたレジスタのマップにバインディング変数名とレジスタを追加する
	//! @param		name			変数名
	//! @param		nestlevel		そのレジスタのネストレベル
	//! @param		regnum			レジスタ番号
	//! @note		このメソッドは、バインディング情報中の変数を登録するためにある。
	//!				つまり、関数グループ内の共有変数をこのメソッドで追加してはならない。
	void AddBindingRegNameMap(const tString & name, risse_uint16 nestlevel, risse_uint16 regnum);

	//! @brief		指定されたネストレベルに対する共有されたレジスタの個数を得る
	//! @return		共有されたレジスタの個数
	//! @note		SharedRegNameMap は同じ関数グループ内では共有しているが、
	//!				このメソッドが返す値は、このネストレベルに対応する共有変数の数
	//!				だけであることに注意。
	risse_size GetSharedRegCount() const;

	//! @brief		共有変数の最大のネストカウントを取得する
	//! @return		共有変数の最大のネストカウント
	risse_size QuerySharedVariableNestCount() const;


	//! @brief		VariableMapForChildren 用の変数を登録する
	//! @param		var			変数
	//! @param		name		変数名
	risse_size RegisterVariableMapForChildren(const tSSAVariable * var, const tString & name);

	//! @brief		VariableMapForChildren 内で変数を探す
	//! @param		name		変数名
	//! @return		そのレジスタのインデックス
	//! @note		nameがマップ内に見つからなかったときは(デバッグ時は)
	//!				ASSERTに失敗する
	risse_size FindVariableMapForChildren(const tString & name);

	//! @brief		コードを確定する(未解決のジャンプなどを解決する)
	void FixCode();

public:
	//--------------------------------------------------------
	// コード生成用関数群
	// コードの種類別に関数を用意するのは無駄にみえるが、バイトコードのための
	// 知識をこのクラス内で囲っておくために必要

	//! @param		これから置く命令コードがソースコード上のどの位置に対応するかを通知する
	//! @param		pos		ソースコード上の位置
	void SetSourceCodePosition(risse_size pos);

	//! @param		これから置く命令コードがソースコード上のどの位置に対応するかを得る
	//! @return		ソースコード上の位置(まだ位置が特定されていない場合は risse_size_max が帰る)
	risse_size GetSourceCodePosition() const;

	//! @brief		Nopを置く
	void PutNoOperation();

	//! @brief		Assignコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(const tSSAVariable * dest, const tSSAVariable * src);

	//! @brief		Assignコードを置く(このメソッドは削除の予定;使わないこと)
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(risse_size dest, const tSSAVariable * src);

	//! @brief		Assignコードを置く(このメソッドは削除の予定;使わないこと)
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(const tSSAVariable * dest, risse_size src);

	//! @brief		AssignConstantコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		value	値
	void PutAssign(const tSSAVariable * dest, const tVariant & value);

	//! @brief		オブジェクトを Assign するコードを置く(AssignThis, AssignSuper等)
	//! @param		dest	変数コピー先変数
	//! @param		code	オペレーションコード
	void PutAssign(const tSSAVariable * dest, tOpCode code);

	//! @brief		新しい正規表現オブジェクトを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		pattern	正規表現パターンを表す変数
	//! @param		flags	正規表現フラグを表す変数
	void PutAssignNewRegExp(const tSSAVariable * dest, const tSSAVariable * pattern, const tSSAVariable * flags);

	//! @brief		新しい関数インスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		body	「裸の」関数インスタンス
	void PutAssignNewFunction(const tSSAVariable * dest, const tSSAVariable * body);

	//! @brief		新しいプロパティインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		getter	ゲッタを表す変数
	//! @param		setter	セッタを表す変数
	void PutAssignNewProperty(const tSSAVariable * dest, const tSSAVariable * getter, const tSSAVariable * setter);

	//! @brief		新しいクラスインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		super	親クラスを表す変数
	//! @param		name	クラス名を表す変数
	void PutAssignNewClass(const tSSAVariable * dest, const tSSAVariable * super, const tSSAVariable * name);

	//! @brief		新しいモジュールインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		name	モジュール名を表す変数
	void PutAssignNewModule(const tSSAVariable * dest, const tSSAVariable * name);

	//! @brief		メソッドへの引数を assign するコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		index	引数インデックス
	void PutAssignParam(const tSSAVariable * dest, risse_size index);

	//! @brief		メソッドへのブロック引数を assign するコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		index	ブロック引数インデックス
	void PutAssignBlockParam(const tSSAVariable * dest, risse_size index);

	//! @brief		ローカル変数のバインディング情報を追加するコードを置く
	//! @param		map		マップ
	//! @param		name	変数名(装飾無し)を表すSSA形式変数
	//! @param		nname	共有変数名(番号付き)
	void PutAddBindingMap(const tSSAVariable * map, const tSSAVariable *name,
					const tString &nname);

	//! @brief		Writeコード(共有空間への書き込み)を置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutWrite(const tString & dest, const tSSAVariable * src);

	//! @brief		Readコード(共有空間からの読み込み)を置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutRead(const tSSAVariable * dest, const tString & src);

	//! @brief		他のコードブロックへの再配置用コードを置く
	//! @param		dest	格納先変数
	//! @param		index	コードブロックのインデックス
	void PutCodeBlockRelocatee(const tSSAVariable * dest, risse_size index);

	//! @brief		スタックフレームと共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetFrame(const tSSAVariable * dest);

	//! @brief		共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetShare(const tSSAVariable * dest);

	//! @brief		FuncCall あるいは New コード または TryFuncCall を置く
	//! @param		dest	関数結果格納先
	//! @param		func	関数を表す変数
	//! @param		code	コード(種別)
	//! @param		expbit	それぞれの引数が展開を行うかどうかを表すビット列
	//! @param		args	引数
	//! @param		blocks	遅延評価ブロック
	void PutFunctionCall(const tSSAVariable * dest,
		const tSSAVariable * func,
		tOpCode code, risse_uint32 expbit,
		const gc_vector<const tSSAVariable *> & args,
		const gc_vector<const tSSAVariable *> & blocks);

	//! @brief		sync コードを奥
	//! @param		dest	sync ブロックの結果格納先
	//! @param		func	sync ブロックを表す変数
	//! @param		lockee	sync するためのオブジェクト
	void PutSync(const tSSAVariable * dest,
		const tSSAVariable * func, const tSSAVariable * lockee);

	//! @brief		Jump コードを置く
	//! @param		target	ジャンプ先基本ブロック
	void PutJump(const tSSABlock * target);

	//! @brief		Branch コードを置く
	//! @param		ref		調べる変数
	//! @param		truetarget	真の時にジャンプする先
	//! @param		falsetarget	偽の時にジャンプする先
	void PutBranch(const tSSAVariable * ref,
		const tSSABlock * truetarget, const tSSABlock * falsetarget);

	//! @brief		CatchBranch コードを置く
	//! @param		ref			調べる変数
	//! @param		try_id_idx	try識別子のインデックス
	//! @param		targets		ジャンプ先配列
	void PutCatchBranch(const tSSAVariable * ref,
		risse_size try_id_idx,
		const gc_vector<tSSABlock *> & targets);

	//! @brief		Debugger コードを置く
	void PutDebugger();

	//! @brief		Throw コードを置く
	//! @param		throwee		投げる例外オブジェクトが入っている変数
	void PutThrow(const tSSAVariable * throwee);

	//! @brief		Return コードを置く
	//! @param		value		返す値が入っている変数
	void PutReturn(const tSSAVariable * value);

	//! @brief		例外によるtry脱出コードを置く
	//! @param		value		返す値が入っている変数(NULL=値はない場合)
	//! @param		try_id_idx	Try 識別子番号
	//! @param		idx			分岐先ID
	void PutExitTryException(const tSSAVariable * value,
		risse_size try_id_idx, risse_size idx);

	//! @brief		例外による脱出系の例外オブジェクトから「値」を取り出す
	//! @param		dest		「値」を格納する先の変数
	//! @param		src			「値」を得る例外オブジェクトが入ってる変数
	void PutGetExitTryValue(const tSSAVariable * dest, const tSSAVariable * src);

	//! @brief		dest = op(arg1) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ
	void PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1);

	//! @brief		dest = op(arg1, arg2) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ1
	//! @param		arg2	パラメータ2
	void PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1, const tSSAVariable * arg2);

	//! @brief		dest = op(arg1, arg2, arg3) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ1
	//! @param		arg2	パラメータ2
	//! @param		arg3	パラメータ3
	void PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1, const tSSAVariable * arg2, const tSSAVariable * arg3);

	//! @brief		InContextOf コードを置く
	//! @param		dest	結果を格納する変数
	//! @param		instance	インスタンスを表す変数
	//! @param		context		コンテキストを表す変数(NULLの場合はdynamicコンテキスト)
	void PutInContextOf(const tSSAVariable * dest,
		const tSSAVariable * instance, const tSSAVariable * context);

	//! @brief		DGetとIGetのコードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		flags	アクセスフラグ
	void PutGet(tOpCode op, const tSSAVariable * obj,
		const tSSAVariable * name, const tSSAVariable * value, risse_uint32 flags);

	//! @brief		DSetとISetのコードを置く
	//! @param		op		オペレーションコード
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		value	格納する変数
	//! @param		flags	アクセスフラグ
	void PutSet(tOpCode op, const tSSAVariable * obj,
		const tSSAVariable * name, const tSSAVariable * value, risse_uint32 flags);

	//! @brief		DSetAttribのコードを置く
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		attrib	属性
	void PutSetAttribute(const tSSAVariable * obj,
		const tSSAVariable * name, risse_uint32 attrib);

	//! @brief		Assertion のコードを置く
	//! @param		cond	条件を表す変数
	//! @param		msg		条件が偽の時に投げるメッセージ
	void PutAssert(const tSSAVariable *cond, const tString & msg);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

