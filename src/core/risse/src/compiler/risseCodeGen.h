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

class tRisseSSABlock;
//---------------------------------------------------------------------------
//! @brief		コードジェネレータクラス
//---------------------------------------------------------------------------
class tRisseCodeGenerator : public tRisseCollectee
{
	static const risse_size MaxConstSearch = 5; // 定数領域で同じ値を探す最大値

	tRisseCodeGenerator * Parent; //!< 親のコードジェネレータ
	bool UseParentFrame; //!< 親のコードジェネレータのフレームを使うかどうか
	risse_size NestLevel;		//!< 関数のネストレベル
	risse_size RegisterBase; //!< レジスタの基本値

	gc_vector<risse_uint32> Code; //!< コード
	gc_vector<tRisseVariant> Consts; //!< 定数領域
	gc_vector<risse_size> RegFreeMap; // 空きレジスタの配列
	risse_size NumUsedRegs; // 使用中のレジスタの数
	risse_size MaxNumUsedRegs; // 使用中のレジスタの最大数
public:
	typedef gc_map<tRisseString, risse_size> tNamedRegMap;
		//!< 変数名とそれに対応するレジスタ番号のマップのtypedef
	typedef gc_map<const tRisseSSAVariable *, risse_size> tRegMap;
		//!< 変数とそれに対応するレジスタ番号のマップのtypedef
private:
	tNamedRegMap SharedRegNameMap; //!< 共有変数名とそれに対応するレジスタ番号のマップ
	tNamedRegMap VariableMapForChildren; //!< 親コードジェネレータが子ジェネレータに対して提供する変数のマップ
	tRegMap RegMap; //!< 変数とそれに対応するレジスタ番号のマップ
	//! @brief		未解決のジャンプを表す構造体
	struct tPendingBlockJump
	{
		const tRisseSSABlock *	Block; //!< 基本ブロック
		risse_size			EmitPosition; //!< オフセットを入れ込む位置
		risse_size			InsnPosition; //!< 命令位置
		tPendingBlockJump(const tRisseSSABlock * block, risse_size emit_pos, risse_size insn_pos)
			: Block(block), EmitPosition(emit_pos), InsnPosition(insn_pos) {;}
	};
	gc_vector<tPendingBlockJump> PendingBlockJumps;
			//!< 未解決のジャンプとその基本ブロックのリスト
	typedef gc_map<const tRisseSSABlock *, risse_size> tBlockMap;
			//!< 基本ブロックとそれが対応するアドレスの typedef
	tBlockMap BlockMap; //!< 変数とそれに対応するレジスタ番号のマップ

	gc_vector<std::pair<risse_size, risse_size> > CodeBlockRelocations; // 他のコードブロックの再配置情報用配列
	gc_vector<std::pair<risse_size, risse_size> > TryIdentifierRelocations; // Try識別子の再配置情報用配列
	gc_vector<std::pair<risse_size, risse_size> > CodeToSourcePosition; //!< コード位置とソースコード上の位置の対応を表すペアの配列

public:
	//! @brief		コンストラクタ
	//! @param		parent			親コードジェネレータ
	//! @param		useparentframe	親コードジェネレータのフレームを使うかどうか
	//! @param		nestlevel		関数のネストレベル
	tRisseCodeGenerator(tRisseCodeGenerator * parent, bool useparentframe, risse_size nestlevel);

public:
	//! @brief	親のコードジェネレータを得る
	//! @return	親のコードジェネレータを得る
	tRisseCodeGenerator * GetParent() const { return Parent; }

	//! @brief	レジスタの基本値を得る @return レジスタの基本値
	risse_size GetRegisterBase() const { return RegisterBase; }

	//! @brief	レジスタの基本値を設定する(親コードジェネレータから値を得る)
	void SetRegisterBase();

	//! @brief	コード配列を得る @return コード配列
	const gc_vector<risse_uint32> & GetCode() const { return Code; }

	//! @brief	定数領域の配列を得る @return 定数領域の配列
	const gc_vector<tRisseVariant> & GetConsts() const { return Consts; }

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
	void AddBlockMap(const tRisseSSABlock * block);

	//! @brief		未解決のジャンプとその基本ブロックを追加する
	//! @param		block		基本ブロック
	//! @param		insn_pos	ジャンプ命令の開始位置
	//! @note		ジャンプ先アドレスを入れ込むアドレスとしては現在の命令書き込み位置が用いられる
	void AddPendingBlockJump(const tRisseSSABlock * block, risse_size insn_pos);

	//! @brief		定数領域から値を見つけ、その値を返す
	//! @param		value		定数値
	//! @return		その定数のインデックス
	risse_size FindConst(const tRisseVariant & value);

	//! @brief		レジスタのマップを変数で探す
	//! @param		var			変数
	//! @return		そのレジスタのインデックス
	//! @note		varがマップ内に見つからなかったときはレジスタを割り当て
	//!				そのレジスタのインデックスを返す
	risse_size FindRegMap(const tRisseSSAVariable * var);

	//! @brief		使用中のレジスタの最大値を更新する
	//! @param		max		レジスタの最大値
	//! @note		UseParentFrameが真の場合は、親のコードブロックと子
	//!				のコードブロックはスタックフレームを共有するため、
	//!				このメソッドは子から親へ再帰的に呼ばれる(結果、子と親の
	//!				MaxNumUsedRegs は同一になる)
	void UpdateMaxNumUsedRegs(risse_size max);

	//! @brief		レジスタを一つ割り当てる
	//! @return		割り当てられたレジスタ
	//! @note		レジスタを割り当てたら FreeRegister で開放すること
	risse_size AllocateRegister();

	//! @brief		レジスタを一つ開放する(レジスタインデックスより)
	//! @param		reg		AllocateRegister() で割り当てたレジスタ
	void FreeRegister(risse_size reg);

	//! @brief		レジスタを一つ解放する(変数インスタンスより)
	//! @param		var		変数
	//! @note		変数はレジスタマップ内に存在しないとならない
	void FreeRegister(const tRisseSSAVariable *var);

	//! @brief		共有されたレジスタのマップを変数名で探す
	//! @param		name			変数名
	//! @param		nest_level		そのレジスタのネストレベル
	//! @param		regnum			レジスタ番号
	//! @note		nameがマップ内に見つからなかった場合は(デバッグモード時は)
	//!				親コードジェネレータを探し、それでも見つからなければASSERTに失敗となる
	void FindSharedRegNameMap(const tRisseString & name, risse_uint16 &nestlevel, risse_uint16 &regnum);

	//! @brief		共有されたレジスタのマップに変数名とレジスタを追加する
	//! @param		name			変数名
	void AddSharedRegNameMap(const tRisseString & name);

	//! @brief		指定されたネストレベルに対する共有されたレジスタの個数を得る
	//! @return		共有されたレジスタの個数
	risse_size GetSharedRegCount() const;

	//! @brief		VariableMapForChildren 内で変数を探す
	//! @param		name		変数名
	//! @return		そのレジスタのインデックス
	//! @note		nameがマップ内に見つからなかったときはレジスタを割り当て
	//!				そのレジスタのインデックスを返す
	risse_size FindOrRegisterVariableMapForChildren(const tRisseString & name);

	//! @brief		VariableMapForChildren 内で変数を探す
	//! @param		name		変数名
	//! @return		そのレジスタのインデックス
	//! @note		nameがマップ内に見つからなかったときは(デバッグ時は)
	//!				ASSERTに失敗する
	risse_size FindVariableMapForChildren(const tRisseString & name);

	//! @brief		VariableMapForChildren にある変数をすべて開放する
	//! @note		変数は開放するが、マップそのものはクリアしない。
	void FreeVariableMapForChildren();

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

	//! @brief		Nopを置く
	void PutNoOperation();

	//! @brief		Assignコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(const tRisseSSAVariable * dest, const tRisseSSAVariable * src);

	//! @brief		Assignコードを置く(このメソッドは削除の予定;使わないこと)
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(risse_size dest, const tRisseSSAVariable * src);

	//! @brief		Assignコードを置く(このメソッドは削除の予定;使わないこと)
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutAssign(const tRisseSSAVariable * dest, risse_size src);

	//! @brief		AssignConstantコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		value	値
	void PutAssign(const tRisseSSAVariable * dest, const tRisseVariant & value);

	//! @brief		オブジェクトを Assign するコードを置く(AssignThis, AssignSuper等)
	//! @param		dest	変数コピー先変数
	//! @param		code	オペレーションコード
	void PutAssign(const tRisseSSAVariable * dest, tRisseOpCode code);

	//! @brief		新しい正規表現オブジェクトを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		pattern	正規表現パターンを表す変数
	//! @param		flags	正規表現フラグを表す変数
	void PutAssignNewRegExp(const tRisseSSAVariable * dest, const tRisseSSAVariable * pattern, const tRisseSSAVariable * flags);

	//! @brief		新しい関数インスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		body	「裸の」関数インスタンス
	void PutAssignNewFunction(const tRisseSSAVariable * dest, const tRisseSSAVariable * body);

	//! @brief		新しいプロパティインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		getter	ゲッタを表す変数
	//! @param		setter	セッタを表す変数
	void PutAssignNewProperty(const tRisseSSAVariable * dest, const tRisseSSAVariable * getter, const tRisseSSAVariable * setter);

	//! @brief		新しいクラスインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		super	親クラスを表す変数
	//! @param		name	クラス名を表す変数
	void PutAssignNewClass(const tRisseSSAVariable * dest, const tRisseSSAVariable * super, const tRisseSSAVariable * name);

	//! @brief		新しいモジュールインスタンスを Assign するコードを置く
	//! @param		dest	Assign先変数
	//! @param		name	モジュール名を表す変数
	void PutAssignNewModule(const tRisseSSAVariable * dest, const tRisseSSAVariable * name);

	//! @brief		メソッドへの引数を assign するコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		index	引数インデックス
	void PutAssignParam(const tRisseSSAVariable * dest, risse_size index);

	//! @brief		メソッドへのブロック引数を assign するコードを置く
	//! @param		dest	変数コピー先変数
	//! @param		index	ブロック引数インデックス
	void PutAssignBlockParam(const tRisseSSAVariable * dest, risse_size index);

	//! @brief		Writeコード(共有空間への書き込み)を置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutWrite(const tRisseString & dest, const tRisseSSAVariable * src);

	//! @brief		Readコード(共有空間からの読み込み)を置く
	//! @param		dest	変数コピー先変数
	//! @param		src		変数コピー元変数
	void PutRead(const tRisseSSAVariable * dest, const tRisseString & src);

	//! @brief		他のコードブロックへの再配置用コードを置く
	//! @param		dest	格納先変数
	//! @param		index	コードブロックのインデックス
	void PutCodeBlockRelocatee(const tRisseSSAVariable * dest, risse_size index);

	//! @brief		スタックフレームと共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetFrame(const tRisseSSAVariable * dest);

	//! @brief		共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetShare(const tRisseSSAVariable * dest);

	//! @brief		FuncCall あるいは New コード または TryFuncCall を置く
	//! @param		dest	関数結果格納先
	//! @param		func	関数を表す変数
	//! @param		code	コード(種別)
	//! @param		expbit	それぞれの引数が展開を行うかどうかを表すビット列
	//! @param		args	引数
	//! @param		blocks	遅延評価ブロック
	void PutFunctionCall(const tRisseSSAVariable * dest,
		const tRisseSSAVariable * func,
		tRisseOpCode code, risse_uint32 expbit,
		const gc_vector<const tRisseSSAVariable *> & args,
		const gc_vector<const tRisseSSAVariable *> & blocks);

	//! @brief		Jump コードを置く
	//! @param		target	ジャンプ先基本ブロック
	void PutJump(const tRisseSSABlock * target);

	//! @brief		Branch コードを置く
	//! @param		ref		調べる変数
	//! @param		truetarget	真の時にジャンプする先
	//! @param		falsetarget	偽の時にジャンプする先
	void PutBranch(const tRisseSSAVariable * ref,
		const tRisseSSABlock * truetarget, const tRisseSSABlock * falsetarget);

	//! @brief		CatchBranch コードを置く
	//! @param		ref			調べる変数
	//! @param		try_id_idx	try識別子のインデックス
	//! @param		targets		ジャンプ先配列
	void PutCatchBranch(const tRisseSSAVariable * ref,
		risse_size try_id_idx,
		const gc_vector<tRisseSSABlock *> & targets);

	//! @brief		Debugger コードを置く
	void PutDebugger();

	//! @brief		Throw コードを置く
	//! @param		throwee		投げる例外オブジェクトが入っている変数
	void PutThrow(const tRisseSSAVariable * throwee);

	//! @brief		Return コードを置く
	//! @param		value		返す値が入っている変数
	void PutReturn(const tRisseSSAVariable * value);

	//! @brief		例外によるtry脱出コードを置く
	//! @param		value		返す値が入っている変数(NULL=値はない場合)
	//! @param		try_id_idx	Try 識別子番号
	//! @param		idx			分岐先ID
	void PutExitTryException(const tRisseSSAVariable * value,
		risse_size try_id_idx, risse_size idx);

	//! @brief		例外による脱出系の例外オブジェクトから「値」を取り出す
	//! @param		dest		「値」を格納する先の変数
	//! @param		src			「値」を得る例外オブジェクトが入ってる変数
	void PutGetExitTryValue(const tRisseSSAVariable * dest, const tRisseSSAVariable * src);

	//! @brief		dest = op(arg1) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ
	void PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1);

	//! @brief		dest = op(arg1, arg2) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ1
	//! @param		arg2	パラメータ2
	void PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1, const tRisseSSAVariable * arg2);

	//! @brief		dest = op(arg1, arg2, arg3) 系コードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変数
	//! @param		arg1	パラメータ1
	//! @param		arg2	パラメータ2
	//! @param		arg3	パラメータ3
	void PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1, const tRisseSSAVariable * arg2, const tRisseSSAVariable * arg3);

	//! @brief		DGetとIGetのコードを置く
	//! @param		op		オペレーションコード
	//! @param		dest	結果を格納する変
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		flags	アクセスフラグ
	void PutGet(tRisseOpCode op, const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, const tRisseSSAVariable * value, risse_uint32 flags);

	//! @brief		DSetとISetのコードを置く
	//! @param		op		オペレーションコード
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		value	格納する変数
	//! @param		flags	アクセスフラグ
	void PutSet(tRisseOpCode op, const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, const tRisseSSAVariable * value, risse_uint32 flags);

	//! @brief		DSetAttribのコードを置く
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		attrib	属性
	void PutSetAttribute(const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, risse_uint32 attrib);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

