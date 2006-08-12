//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
	risse_size RegisterBase; //!< レジスタの基本値

	gc_vector<risse_uint32> Code; //!< コード
	gc_vector<tRisseVariant> Consts; //!< 定数領域
	gc_vector<risse_size> RegFreeMap; // 空きレジスタの配列
	risse_size NumUsedRegs; // 使用中のレジスタの数
	risse_size MaxNumUsedRegs; // 使用中のレジスタの最大数
	typedef gc_map<tRisseString, risse_size> tNamedRegMap;
		//!< 変数名とそれに対応するレジスタ番号のマップのtypedef
	typedef gc_map<const tRisseSSAVariable *, risse_size> tRegMap;
		//!< 変数とそれに対応するレジスタ番号のマップのtypedef
	tNamedRegMap *SharedRegNameMap; //!< 共有された変数の変数名とそれに対応するレジスタ番号のマップ
	tNamedRegMap ParentVariableMap; //!< 親コードジェネレータが子ジェネレータに対して提供する変数のマップ
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

	gc_vector<std::pair<risse_size, risse_size> > Relocations; // 他のコードブロックの再配置情報用配列

public:
	//! @brief		コンストラクタ
	//! @param		parent			親コードジェネレータ
	//! @param		useparentframe	親コードジェネレータのフレームを使うかどうか
	tRisseCodeGenerator(tRisseCodeGenerator * parent = NULL, bool useparentframe = false);

public:

	//! @brief	レジスタの基本値を得る @return レジスタの基本値
	risse_size GetRegisterBase() const { return RegisterBase; }

	//! @brief	レジスタの基本値を設定する(親コードジェネレータから値を得る)
	void SetRegisterBase();

	//! @brief	コード配列を得る @return コード配列
	const gc_vector<risse_uint32> & GetCode() const { return Code; }

	//! @brief	定数領域の配列を得る @return 定数領域の配列
	const gc_vector<tRisseVariant> & GetConsts() const { return Consts; }

	//! @brief	使用中のレジスタの最大数を得る @return 使用中のレジスタの最大数
	risse_size GetMaxNumUsedRegs() const { return MaxNumUsedRegs; }

	//! @brief	他のコードブロックの再配置情報用配列を得る @return 他のコードブロックの再配置情報用配列
	const gc_vector<std::pair<risse_size, risse_size> > & GetRelocations() const
		{ return Relocations; }

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
	//! @return		そのレジスタのインデックス
	//! @note		nameがマップ内に見つからなかった場合は(デバッグモード時は)
	//!				ASSERTに失敗となる
	risse_size FindSharedRegNameMap(const tRisseString & name);

	//! @brief		共有されたレジスタのマップに変数名とレジスタを追加する
	//! @param		name			変数名
	void AddSharedRegNameMap(const tRisseString & name);

	//! @brief		共有されたレジスタの個数を得る @return 共有されたレジスタの個数
	risse_size GetSharedRegCount() const { return SharedRegNameMap->size(); }

	//! @brief		ParentVariableMap 内で変数を探す (親から子に対して呼ばれる)
	//! @param		name		変数名
	//! @return		そのレジスタのインデックス
	//! @note		nameがマップ内に見つからなかったときは *親から* レジスタを割り当て
	//!				そのレジスタのインデックスを返す
	risse_size FindParentVariableMap(const tRisseString & name);

	//! @brief		ParentVariableMap にある変数をすべて開放する
	//! @note		変数は開放するが、マップそのものはクリアしない。
	void FreeParentVariableMapVariables();

	//! @brief		コードを確定する(未解決のジャンプなどを解決する)
	void FixCode();

public:
	//--------------------------------------------------------
	// コード生成用関数群
	// コードの種類別に関数を用意するのは無駄にみえるが、バイトコードのための
	// 知識をこのクラス内で囲っておくために必要

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
	void PutRelocatee(const tRisseSSAVariable * dest, risse_size index);

	//! @brief		スタックフレームと共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetFrame(const tRisseSSAVariable * dest);

	//! @brief		共有空間の書き換え用コードを置く
	//! @param		dest	書き換え先変数
	void PutSetShare(const tRisseSSAVariable * dest);

	//! @brief		FuncCall あるいは New コードを置く
	//! @param		dest	関数結果格納先
	//! @param		func	関数を表す変数
	//! @param		is_new	newか(真), 関数呼び出しか(偽)
	//! @param		omit	引数省略かどうか
	//! @param		expbit	それぞれの引数が展開を行うかどうかを表すビット列
	//! @param		args	引数
	//! @param		blocks	遅延評価ブロック
	void PutFunctionCall(const tRisseSSAVariable * dest,
		const tRisseSSAVariable * func,
		bool is_new, bool omit, risse_uint32 expbit,
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

	//! @brief		Debugger コードを置く
	void PutDebugger();

	//! @brief		Throw コードを置く
	//! @param		throwee		投げる例外オブジェクトが入っている変数
	void PutThrow(const tRisseSSAVariable * throwee);

	//! @brief		Return コードを置く
	//! @param		value		返す値が入っている変数
	void PutReturn(const tRisseSSAVariable * value);

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

	//! @brief		DSetとISetのコードを置く
	//! @param		op		オペレーションコード
	//! @param		obj		オブジェクトを表す変数
	//! @param		name	メンバ名を表す変数
	//! @param		value	格納する変数
	void PutSet(tRisseOpCode op, const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, const tRisseSSAVariable * value);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
#endif

