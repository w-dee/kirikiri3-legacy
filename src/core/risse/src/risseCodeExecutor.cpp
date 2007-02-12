//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコード実行クラスのインターフェースとインタプリタの実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCodeExecutor.h"
#include "risseOpCodes.h"
#include "risseExceptionClass.h"
#include "risseScriptBlockBase.h"
#include "risseModuleClass.h"
#include "risseClassClass.h"
#include "risseFunctionClass.h"
#include "rissePropertyClass.h"
#include "risseScriptEngine.h"
#include "risseThisProxy.h"
/*
	このソースは、実行スピード重視の、いわばダーティーな実装を行う。
	ダーティーな実装は極力コメントを残し、わかりやすくしておくこと。
*/


namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(38733,31388,53292,19613,29887,64791,9160,61431);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseCodeInterpreter::tRisseCodeInterpreter(tRisseCodeBlock *cb) :
	tRisseCodeExecutor(cb)
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeInterpreter::Execute(
		const tRisseMethodArgument & args,
		const tRisseVariant & This,
		tRisseVariant * frame, tRisseSharedVariableFrames * shared,
		tRisseVariant * result)
{
	// context でスタックフレームが指定されていない場合、スタックを割り当てる
	// TODO: スタックフレームの再利用など
	// 毎回スタックを new で割り当てるのは効率が悪い？
	if(frame == NULL)
		frame = new tRisseVariant[CodeBlock->GetNumRegs()];

	// 共有変数領域の割り当て
	if(shared == NULL)
	{
		RISSE_ASSERT(CodeBlock->GetNestLevel() == 0);
		shared = new tRisseSharedVariableFrames(CodeBlock->GetMaxNestLevel());
	}

	tRisseVariant * prev_shared_frame = 
		CodeBlock->GetNumSharedVars() ? 
		shared->Set(CodeBlock->GetNestLevel(), CodeBlock->GetNumSharedVars()) : NULL;

	// ローカル変数に値を持ってくる
	// いくつかのローカル変数は ASSERT が有効になっていなければ
	// 必要ないので、#ifdef ～ #endif で場合分けをする。
#ifdef RISSE_ASSERT_ENABLED
	risse_size framesize = CodeBlock->GetNumRegs();
#endif
	const risse_uint32 * code = CodeBlock->GetCode();
	const risse_uint32 * code_origin = CodeBlock->GetCode();
#ifdef RISSE_ASSERT_ENABLED
	risse_size codesize = CodeBlock->GetCodeSize();
#endif
	const tRisseVariant * consts = CodeBlock->GetConsts();
#ifdef RISSE_ASSERT_ENABLED
	risse_size constssize = CodeBlock->GetConstsSize();
#endif

	try
	{
		/*
		スタックフレームと定数領域へのアクセスなど以下のマクロを使うこと。
		frame[num] のように書くと num に毎回 sizeof(frame[0]) の乗算が発生するため、
		将来的に、あらかじめ num は乗算を済ましておき、
		*(tRisseVariant*)((risse_uint8*)frame + (num)) のようなマクロに置き換える
		可能性がある。
		*/
		//! @brief		スタックフレームにアクセス
		#define AR(num) (frame[(num)])
		//! @brief		定数領域にアクセス
		#define AC(num) (consts[(num)])
		//! @brief		レジスタのオペランド -> レジスタ/定数インデックスへの変換
		#define CI(num) (num)

		// this-proxy 用の領域
		// 毎回これをnewするのはどうかと思うので
		// スタック上に配置する。ただし、必要ない場合はこれを
		// 生成する必要はないため、この時点ではストレージを確保しておくだけにする。
		// (まぁそんなこといってもこいつの生成コストなどたかがしれているはずなのだが)
		union tThisProxy
		{
			unsigned long dummy; // 強制的にこの共用体のアラインメントを合わせるために
			char Storage[sizeof(tRisseThisProxy)];
		} ThisProxy;

		// ループ
		while(true)
		{
			switch(*code)
			{
			case ocNoOperation	: // nop	 なにもしない
				code += 1;
				break;

			case ocAssign			: // cp		 = (ローカル変数の代入
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) = AR(code[2]);
				code += 3;
				break;

			case ocAssignConstant	: // const	 = 定数の代入
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < constssize);
				AR(code[1]) = AC(code[2]);
				code += 3;
				break;

			case ocAssignThis		: // this	 = thisの代入
				RISSE_ASSERT(CI(code[1]) < framesize);
				AR(code[1]) = This;
				code += 2;
				break;

			case ocAssignThisProxy		: // this	 = this-proxyの代入
				RISSE_ASSERT(CI(code[1]) < framesize);
				AR(code[1]) = tRisseVariant(
					new((tRisseThisProxy*)(&ThisProxy.Storage[0])) tRisseThisProxy(
						const_cast<tRisseVariant&>(This),
						CodeBlock->GetScriptBlock()->GetScriptEngine()->GetGlobalObject()));
				code += 2;
				break;

			case ocAssignSuper	: // super	 = superの代入
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				code += 2;
				break;

			case ocAssignGlobal	: // global	 = globalの代入
				RISSE_ASSERT(CI(code[1]) < framesize);
				AR(code[1]) = CodeBlock->GetScriptBlock()->GetScriptEngine()->GetGlobalObject();
				code += 2;
				break;

			case ocAssignNewArray	: // array	 = 新しい配列オブジェクトの代入
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				code += 2;
				break;

			case ocAssignNewDict	: // dict	 = 新しい辞書配列オブジェクトの代入
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				code += 2;
				break;

			case ocAssignNewRegExp: // regexp	 = 新しい正規表現オブジェクトの代入 (引数2つ)
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				code += 4;
				break;

			case ocAssignNewFunction: // function	 = 新しい関数インスタンスの代入 (引数=呼び出し先メソッドオブジェクト)
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) =
					tRisseVariant(tRisseFunctionClass::GetPointer()).
								New(0, tRisseMethodArgument::New(AR(code[2])));
				code += 3;
				break;

			case ocAssignNewProperty: // property	 = 新しいプロパティインスタンスの代入 (引数=ゲッタ+セッタ)
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) =
					tRisseVariant(tRissePropertyClass::GetPointer()).
								New(0, tRisseMethodArgument::New(AR(code[2]), AR(code[3])));
				code += 4;
				break;

			case ocAssignNewClass: // class	 = 新しいクラスインスタンスの代入 (引数=親クラス+クラス名)
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) =
					tRisseVariant(tRisseClassClass::GetPointer()).
								New(0, tRisseMethodArgument::New(AR(code[2]), AR(code[3])));
				code += 4;
				break;

			case ocAssignNewModule: // module	 = 新しいモジュールインスタンスの代入 (引数=モジュール名)
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) =
					tRisseVariant(tRisseModuleClass::GetPointer()).
								New(0, tRisseMethodArgument::New(AR(code[2])));
				code += 3;
				break;

			case ocAssignParam: // getpar	= (S番目の関数引数を代入)
				RISSE_ASSERT(CI(code[1]) < framesize);
				if(code[2] >= args.GetArgumentCount())
					AR(code[1]).Clear(); // 引数の範囲を超えているのでvoidを代入
				else
					AR(code[1]) = args[code[2]];
				code += 3;
				break;

			case ocAssignBlockParam: // getbpar	= (S番目の関数ブロック引数を代入)
				RISSE_ASSERT(CI(code[1]) < framesize);
				if(code[2] >= args.GetBlockArgumentCount())
					AR(code[1]).Clear(); // 引数の範囲を超えているのでvoidを代入
				else
					AR(code[1]) = args.GetBlockArgument(code[2]);
				code += 3;
				break;

			case ocWrite: // swrite	 共有空間への書き込み
				{
					RISSE_ASSERT(shared);
					risse_uint16 nest_level = (code[1] >> 16) & 0xffff; // 上位16ビット
					risse_uint16 num = (code[1]) & 0xffff; // 下位16ビット
					RISSE_ASSERT(CI(code[2]) < framesize);
					shared->At(nest_level, num) = AR(code[2]);
					code += 3;
					break;
				}

			case ocRead: // sread	共有空間からの読み込み
				{
					RISSE_ASSERT(shared);
					risse_int16 nest_level = (code[2] >> 16) & 0xffff; // 上位16ビット
					risse_int16 num = (code[2]) & 0xffff; // 下位16ビット
					RISSE_ASSERT(CI(code[1]) < framesize);
					AR(code[1]) = shared->At(nest_level, num);
					code += 3;
					break;
				}

			case ocNew			: // new	 "new"
				/* incomplete */
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(CI(code[2]) < framesize);
					// code[1] = 結果格納先 RisseInvalidRegNum の場合は結果は要らない
					// code[2] = メソッドオブジェクト
					// code[3] = フラグ
					// code[4] = 引数の数
					// code[5] ～   引数
					// TODO: 引数展開、引数の省略など
					RISSE_ASSERT(code[4] < RisseMaxArgCount); // 引数は最大RisseMaxArgCount個まで
					tRisseMethodArgument & args = tRisseMethodArgument::Allocate(code[4]);

					for(risse_uint32 i = 0; i < code[4]; i++)
						args.SetArgument(i, AR(code[i+5]));
					tRisseVariant new_obj;
					new_obj = AR(code[2]).New(0, args);
					if(code[1]!=RisseInvalidRegNum) AR(code[1]) = new_obj;
					code += code[4] + 5;
					break;
				}

			case ocTryFuncCall		: // trycall	 try function call
				/* incomplete */
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(CI(code[2]) < framesize);

					// code[1] = 結果格納先 RisseInvalidRegNum の場合は結果は要らない
					// code[2] = メソッドオブジェクト
					// code[3] = フラグ
					// code[4] = 引数の数
					// code[5] = ブロック引数の数
					// code[6] ～   引数
					// TODO: 引数展開、引数の省略など
					RISSE_ASSERT(code[4] < RisseMaxArgCount); // 引数は最大RisseMaxArgCount個まで
					tRisseMethodArgument & args = tRisseMethodArgument::Allocate(code[4], code[5]);

					for(risse_uint32 i = 0; i < code[4]; i++)
						args.SetArgument(i, AR(code[i+6]));
					for(risse_uint32 i = 0; i < code[5]; i++)
						args.SetBlockArgument(i, AR(code[i+6+code[4]]));

					if(code[1]!=RisseInvalidRegNum) AR(code[1]).Clear();
					bool raised = false;
					tRisseVariant val;
					try
					{
						AR(code[2]).FuncCall(&val, 0, args, This);
					}
					catch(const eRisseScriptException &e)
					{
						val = e.GetValue();
						raised = true;
					}
					catch(...)
					{
						raised = true;
					}
					if(code[1]!=RisseInvalidRegNum)
						AR(code[1]) = new tRisseTryFuncCallReturnObject(val, raised);
					code += code[4] + code[5] + 6;
					break;
				}

			case ocFuncCall		: // call	 function call
				/* incomplete */
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(CI(code[2]) < framesize);
					// code[1] = 結果格納先 RisseInvalidRegNum の場合は結果は要らない
					// code[2] = メソッドオブジェクト
					// code[3] = フラグ
					// code[4] = 引数の数
					// code[5] ～   引数
					// TODO: 引数展開、引数の省略など
					RISSE_ASSERT(code[4] < RisseMaxArgCount); // 引数は最大RisseMaxArgCount個まで
					tRisseMethodArgument & args = tRisseMethodArgument::Allocate(code[4]);

					for(risse_uint32 i = 0; i < code[4]; i++)
						args.SetArgument(i, AR(code[i+5]));

					AR(code[2]).FuncCall(code[1]==RisseInvalidRegNum?NULL:&AR(code[1]),
						0, args, This);
					code += code[4] + 5;
					break;
				}

			case ocFuncCallBlock	: // callb	 function call with lazyblock
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				/* incomplete */
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(CI(code[2]) < framesize);
					// code[1] = 結果格納先 RisseInvalidRegNum の場合は結果は要らない
					// code[2] = メソッドオブジェクト
					// code[3] = フラグ
					// code[4] = 引数の数
					// code[5] = ブロック引数の数
					// code[6] ～   引数
					// TODO: 引数展開、引数の省略など
					RISSE_ASSERT(code[4] < RisseMaxArgCount); // 引数は最大RisseMaxArgCount個まで
					tRisseMethodArgument & args = tRisseMethodArgument::Allocate(code[4], code[5]);

					for(risse_uint32 i = 0; i < code[4]; i++)
						args.SetArgument(i, AR(code[i+6]));
					for(risse_uint32 i = 0; i < code[5]; i++)
						args.SetBlockArgument(i, AR(code[i+6+code[4]]));

					AR(code[2]).FuncCall(code[1]==RisseInvalidRegNum?NULL:&AR(code[1]),
						0, args, This);
					code += code[4] + code[5] + 6;
					break;
				}

			case ocSetFrame		: // sfrm	 thisとスタックフレームと共有空間を設定する
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(AR(code[1]).GetType() == tRisseVariant::vtObject);

					tRisseCodeBlock * codeblock =
						reinterpret_cast<tRisseCodeBlock*>(AR(code[1]).GetObjectInterface());
					RISSE_ASSERT(dynamic_cast<tRisseCodeBlock*>(codeblock) != NULL);
					tRisseCodeBlockStackAdapter * adapter =
						new tRisseCodeBlockStackAdapter(codeblock, frame, *shared);
					AR(code[1]) = tRisseVariant(adapter, new tRisseVariant(This));
					code += 2;
				}
				break;

			case ocSetShare		: // sshare	 共有空間のみ設定する(thisは設定しないので注意)
				{
					RISSE_ASSERT(CI(code[1]) < framesize);
					RISSE_ASSERT(AR(code[1]).GetType() == tRisseVariant::vtObject);

					tRisseCodeBlock * codeblock =
						reinterpret_cast<tRisseCodeBlock*>(AR(code[1]).GetObjectInterface());
					RISSE_ASSERT(dynamic_cast<tRisseCodeBlock*>(codeblock) != NULL);
					tRisseCodeBlockStackAdapter * adapter =
						new tRisseCodeBlockStackAdapter(codeblock, NULL, *shared);
					AR(code[1]) = tRisseVariant(adapter);
					code += 2;
				}
				break;

			case ocJump			: // jump	 単純なジャンプ
				// アドレスはジャンプコードの開始番地に対する相対指定
				code += static_cast<risse_int32>(code[1]);
				break;

			case ocBranch			: // branch	 分岐
				RISSE_ASSERT(CI(code[1]) < framesize);
				if((bool)AR(code[1]))
					code += static_cast<risse_int32>(code[2]);
				else
					code += static_cast<risse_int32>(code[3]);
				break;

			case ocCatchBranch		: // cbranch 例外catch用の分岐
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < constssize);
				RISSE_ASSERT(AC(code[2]).GetType() == tRisseVariant::vtObject);
				RISSE_ASSERT(AC(code[2]).GetObjectInterface() != NULL);
				RISSE_ASSERT(code[3] >= 2);

				{
					// code[1] ( = ocTryFuncCall で作成されたオブジェクト ) から例外が
					// 発生したかどうかとその値を受け取る
					tRisseTryFuncCallReturnObject * try_ret =
						reinterpret_cast<tRisseTryFuncCallReturnObject*>(
										AR(code[1]).GetObjectInterface());

					bool raised = try_ret->GetRaised();
					AR(code[1]) = try_ret->GetValue(); // 値はレジスタに書き戻す

					// 分岐ターゲットのインデックスを決定する
					risse_uint32 target_index;
					tRisseVariant::tType except_obj_type = AR(code[1]).GetType();
					if(!raised)
					{
						// 直前の ocTryFuncCall では例外は発生しなかった
						target_index = 0; // 例外が発生しなかった場合の飛び先
					}
					else if(except_obj_type == tRisseVariant::vtObject)
					{
						// オブジェクト型
						// (暫定実装)
						// オブジェクト型の getExitTryRecord プロパティを読む
						try
						{
							tRisseVariant v;
							AR(code[1]).GetObjectInterface()->Do(ocDGet, &v, RISSE_WS("getExitTryRecord"));
							RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
							RISSE_ASSERT(v.GetObjectInterface() != NULL);
							tRisseExitTryExceptionClass * record =
								reinterpret_cast<tRisseExitTryExceptionClass*>(v.GetObjectInterface());
							// レコードに記録されている try 識別子が この branch の try 識別子と
							// 一致するかどうかを調べる
							if(AC(code[2]).GetObjectInterface() == record->GetIdentifier())
							{
								// 一致した
								target_index = record->GetBranchTargetIndex();
							}
							else
							{
								// 一致しなかった
								// この例外は再び投げる
								target_index = static_cast<risse_uint32>(-1L);
							}
						}
						catch(...)
						{
							target_index = 1; // 例外が発生した(たぶんプロパティを読み込めなかった)
						}

						if(target_index == static_cast<risse_uint32>(-1L))
								eRisseScriptException::Throw(RISSE_WS("script exception"),
									CodeBlock->GetScriptBlock(), CodeBlock->CodePositionToSourcePosition(code - code_origin),
									AR(code[1]));
					}
					else
					{
						target_index = 1; // 例外が発生していた
					}
					code += static_cast<risse_int32>(code[4 + target_index]);
				}
				break;
	#if 0
			case ocEnterTryBlock	: //!< 例外保護ブロックに入る(VMのみで使用)
				/* incomplete */
				code += 4;
				break;

			case ocExitTryBlock		: //!< 例外保護ブロックから抜ける(VMのみで使用)
				return; // 呼び出し元にそのまま戻る
	#endif
			case ocReturn			: // ret	 return ステートメント
				RISSE_ASSERT(code[1] == RisseInvalidRegNum || CI(code[1]) < framesize);
				if(code[1] != RisseInvalidRegNum && result) *result = AR(code[1]);
				//code += 2;
				return;

			case ocExitTryException	: // returne	return 例外を発生させる
				RISSE_ASSERT(CI(code[1]) == RisseInvalidRegNum || CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < constssize);
				{
					// 暫定実装
					const tRisseVariant & try_id = AC(code[2]);
					RISSE_ASSERT(try_id.GetType() == tRisseVariant::vtObject);
					RISSE_ASSERT(try_id.GetObjectInterface() != NULL);
					tRisseVariant val(new tRisseExitTryExceptionClass(try_id.GetObjectInterface(),
										code[3],
										CI(code[1]) == RisseInvalidRegNum ? NULL : &AR(code[1])));
					eRisseScriptException::Throw(RISSE_WS("return by exit try exception"),
						CodeBlock->GetScriptBlock(), CodeBlock->CodePositionToSourcePosition(code - code_origin), val);
				}
				break;

			case ocGetExitTryValue	: // exitval	Try脱出用例外オブジェクトから値を得る
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				{
					// 暫定実装
					// code[2] は tRisseExitExceptionClass であると見なして良い
					tRisseVariant v;
					RISSE_ASSERT(AR(code[2]).GetType() == tRisseVariant::vtObject);
					AR(code[2]).GetObjectInterface()->Do(ocDGet, &v, RISSE_WS("getExitTryRecord"));
					RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
					RISSE_ASSERT(v.GetObjectInterface() != NULL);
					tRisseExitTryExceptionClass * record =
						reinterpret_cast<tRisseExitTryExceptionClass*>(v.GetObjectInterface());
					RISSE_ASSERT(record->GetValue() != NULL);
					AR(code[1]) = *record->GetValue();
					code += 3;
				}
				break;

			case ocDebugger		: // dbg	 debugger ステートメント
				// とりあえず現在のローカル変数をダンプしてみる
				{
					risse_size framesize = CodeBlock->GetNumRegs();
					for(risse_size n = 0; n < framesize; n++)
					{
						RisseFPrint(stderr, 
							tRisseString(RISSE_WS("%$1 = $2\n"),
								tRisseString::AsString((risse_int64)n),
								AR(n).AsHumanReadable()
									).c_str());
					}
				}
				code += 1;
				break;

			case ocThrow			: // throw	 throw ステートメント
				/* incomplete */
				RISSE_ASSERT(CI(code[1]) < framesize);
				eRisseScriptException::Throw(RISSE_WS("script exception"),
					CodeBlock->GetScriptBlock(), CodeBlock->CodePositionToSourcePosition(code - code_origin), AR(code[1]));
				code += 2;
				break;

			case ocLogNot			: // lnot	 "!" logical not
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) = !AR(code[2]);
				code += 3;
				break;

			case ocBitNot			: // bnot	 "~" bit not
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) = ~AR(code[2]);
				code += 3;
				break;

			case ocDecAssign		: // ERR	 "--" decrement
				/* incomplete */
				code += 3;
				break;

			case ocIncAssign		: // ERR	 "++" increment
				/* incomplete */
				code += 3;
				break;

			case ocPlus			: // plus	 "+"
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) = +AR(code[2]);
				code += 3;
				break;

			case ocMinus			: // minus	 "-"
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]) = -AR(code[2]);
				code += 3;
				break;

			case ocLogOr			: // lor	 ||
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) || AR(code[3]);
				code += 4;
				break;

			case ocLogAnd			: // land	 &&
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) && AR(code[3]);
				code += 4;
				break;

			case ocBitOr			: // bor	 |
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) | AR(code[3]);
				code += 4;
				break;

			case ocBitXor			: // bxor	 ^
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) ^ AR(code[3]);
				code += 4;
				break;

			case ocBitAnd			: // band	 &
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) & AR(code[3]);
				code += 4;
				break;

			case ocNotEqual		: // ne		 !=
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) != AR(code[3]);
				code += 4;
				break;

			case ocEqual			: // eq		 ==
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) == AR(code[3]);
				code += 4;
				break;

			case ocDiscNotEqual	: // dne	 !==
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]).DiscNotEqual(AR(code[3]));
				code += 4;
				break;

			case ocDiscEqual		: // deq	 ===
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]).DiscEqual(AR(code[3]));
				code += 4;
				break;

			case ocLesser			: // lt		 <
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) < AR(code[3]);
				code += 4;
				break;

			case ocGreater		: // gt		 >
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) > AR(code[3]);
				code += 4;
				break;

			case ocLesserOrEqual	: // lte	 <=
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) <= AR(code[3]);
				code += 4;
				break;

			case ocGreaterOrEqual	: // gte	 >=
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) >= AR(code[3]);
				code += 4;
				break;

			case ocRBitShift		: // rbs	 >>>
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]).RBitShift(AR(code[3]));
				code += 4;
				break;

			case ocLShift			: // ls		 <<
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) << AR(code[3]);
				code += 4;
				break;

			case ocRShift			: // rs		 >>
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) >> AR(code[3]);
				code += 4;
				break;

			case ocMod			: // mod	 %
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) % AR(code[3]);
				code += 4;
				break;

			case ocDiv			: // div	 /
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) / AR(code[3]);
				code += 4;
				break;

			case ocIdiv			: // idiv	 \ (integer div)
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]).Idiv(AR(code[3]));
				code += 4;
				break;

			case ocMul			: // mul	 *
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) * AR(code[3]);
				code += 4;
				break;

			case ocAdd			: // add	 +
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) + AR(code[3]);
				code += 4;
				break;

			case ocSub			: // sub	 -
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]) - AR(code[3]);
				code += 4;
				break;

			case ocIncontextOf	: // chgc	 incontextof
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]);
				if(AR(code[1]).GetType() == tRisseVariant::vtObject)
				{
					// 今のところ AR(code[2]) が vtObject でなかった場合は
					// この操作は単に無視される
					AR(code[1]).SetContext(new tRisseVariant(AR(code[3])));
				}
				code += 4;
				break;

			case ocDGet			: // dget	 get .  
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[2]).Do(ocDGet, &AR(code[1]), AR(code[3]), 0);
				code += 4;
				break;

			case ocDGetF		: // dget	 get . with flags
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[2]).Do(ocDGet, &AR(code[1]), AR(code[3]), code[4]);
				code += 5;
				break;

			case ocIGet			: // iget	 get [ ]
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]) = AR(code[2]).IGet(AR(code[3]));
				code += 4;
				break;

			case ocDDelete		: // ddel	 delete .
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				/* incomplete */
				code += 4;
				break;

			case ocIDelete		: // idel	 delete [ ]
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				/* incomplete */
				code += 4;
				break;

			case ocDSetAttrib		: // dseta	set member attribute
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				AR(code[1]).Do(ocDSetAttrib, NULL, AR(code[2]), code[3]);
				code += 4;
				break;

			case ocDSet			: // dset	 set .
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]).Do(ocDSet, NULL, AR(code[2]),
					tRisseOperateFlags::ofMemberEnsure, tRisseMethodArgument::New(AR(code[3])));
				code += 4;
				break;

			case ocDSetF		: // dset	 set . with flags
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]).Do(ocDSet, NULL, AR(code[2]), code[4], tRisseMethodArgument::New(AR(code[3])));
				code += 5;
				break;

			case ocISet			: // iset	 set [ ]
				RISSE_ASSERT(CI(code[1]) < framesize);
				RISSE_ASSERT(CI(code[2]) < framesize);
				RISSE_ASSERT(CI(code[3]) < framesize);
				AR(code[1]).ISet(AR(code[2]), AR(code[3]));
				code += 4;
				break;

			default:
				// TODO: 本当はASSERTではなくて例外を発生した方がいい
				RISSE_ASSERT(!"unknown instruction code");
			}
			RISSE_ASSERT((risse_size)(code - code_origin) < codesize);
		}

	} // try
	catch(const eRisseScriptError & e)
	{
		// この例外はすでに位置情報を持っているので改めて位置情報を追加してやる必要はない。
		if(prev_shared_frame) shared->Set(CodeBlock->GetNestLevel(), prev_shared_frame);
		throw; // そのまま投げる
	}
	catch(const eRisse & e)
	{
		// この例外は位置情報を持っていない。
		if(prev_shared_frame) shared->Set(CodeBlock->GetNestLevel(), prev_shared_frame);
		// 位置情報を持つことができる eRisseScriptError に、例外を変換する。
		eRisseScriptError::Throw(e.GetMessageString(),
			CodeBlock->GetScriptBlock(), CodeBlock->CodePositionToSourcePosition(code - code_origin));
	}
	catch(...)
	{
		if(prev_shared_frame) shared->Set(CodeBlock->GetNestLevel(), prev_shared_frame);
		throw;
	}
}
//---------------------------------------------------------------------------


} // namespace Risse
