//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 日本語ローカライズメッセージ
//---------------------------------------------------------------------------

RISSE_MSG_DECL(RisseInternalError, RISSE_WS("内部エラーが発生しました"))
RISSE_MSG_DECL(RisseWarning, RISSE_WS("警告: "))
RISSE_MSG_DECL(RisseWarnEvalOperator, RISSE_WS("グローバルでない場所で後置 ! 演算子が使われています(この演算子の挙動はRisse version 2.4.1 で変わりましたのでご注意ください)"))
RISSE_MSG_DECL(RisseNarrowToWideConversionError, RISSE_WS("ANSI 文字列を UNICODE 文字列に変換できません。現在のコードページで解釈できない文字が含まれてます。正しいデータが指定されているかを確認してください。データが破損している可能性もあります"))
RISSE_MSG_DECL(RisseVariantConvertError, RISSE_WS("%1 から %2 へ型を変換できません"))
RISSE_MSG_DECL(RisseVariantConvertErrorToObject, RISSE_WS("%1 から Object へ型を変換できません。Object 型が要求される文脈で Object 型以外の値が渡されるとこのエラーが発生します"))
RISSE_MSG_DECL(RisseIDExpected, RISSE_WS("識別子を指定してください"))
RISSE_MSG_DECL(RisseSubstitutionInBooleanContext, RISSE_WS("論理値が求められている場所で = 演算子が使用されています(== 演算子の間違いですか？代入した上でゼロと値を比較したい場合は、(A=B) != 0 の形式を使うことをお勧めします)"));
RISSE_MSG_DECL(RisseCannotModifyLHS, RISSE_WS("不正な代入か不正な式の操作です"))
RISSE_MSG_DECL(RisseInsufficientMem, RISSE_WS("メモリが足りません"))
RISSE_MSG_DECL(RisseCannotGetResult, RISSE_WS("この式からは値を得ることができません"))
RISSE_MSG_DECL(RisseNullAccess, RISSE_WS("null オブジェクトにアクセスしようとしました"))
RISSE_MSG_DECL(RisseMemberNotFound, RISSE_WS("メンバ \"%1\" が見つかりません"))
RISSE_MSG_DECL(RisseMemberNotFoundNoNameGiven, RISSE_WS("メンバが見つかりません"))
RISSE_MSG_DECL(RisseNotImplemented, RISSE_WS("呼び出そうとした機能は未実装です"))
RISSE_MSG_DECL(RisseInvalidParam, RISSE_WS("不正な引数です"))
RISSE_MSG_DECL(RisseBadParamCount, RISSE_WS("引数の数が不正です"))
RISSE_MSG_DECL(RisseInvalidType, RISSE_WS("関数ではないかプロパティの種類が違います"))
RISSE_MSG_DECL(RisseSpecifyDicOrArray, RISSE_WS("Dictionary または Array クラスのオブジェクトを指定してください"))
RISSE_MSG_DECL(RisseSpecifyArray, RISSE_WS("Array クラスのオブジェクトを指定してください"))
RISSE_MSG_DECL(RisseStringDeallocError, RISSE_WS("文字列メモリブロックを解放できません"))
RISSE_MSG_DECL(RisseStringAllocError, RISSE_WS("文字列メモリブロックを確保できません"))
RISSE_MSG_DECL(RisseMisplacedBreakContinue, RISSE_WS("\"break\" または \"continue\" はここに書くことはできません"))
RISSE_MSG_DECL(RisseMisplacedCase, RISSE_WS("\"case\" はここに書くことはできません"))
RISSE_MSG_DECL(RisseMisplacedReturn, RISSE_WS("\"return\" はここに書くことはできません"))
RISSE_MSG_DECL(RisseStringParseError, RISSE_WS("文字列定数/正規表現/オクテット即値が終わらないままスクリプトの終端に達しました"))
RISSE_MSG_DECL(RisseNumberError, RISSE_WS("数値として解釈できません"))
RISSE_MSG_DECL(RisseUnclosedComment, RISSE_WS("コメントが終わらないままスクリプトの終端に達しました"))
RISSE_MSG_DECL(RisseInvalidChar, RISSE_WS("不正な文字です : \'%1\'"))
RISSE_MSG_DECL(RisseExpected, RISSE_WS("%1 がありません"))
RISSE_MSG_DECL(RisseSyntaxError, RISSE_WS("文法エラーです(%1)"))
RISSE_MSG_DECL(RissePPError, RISSE_WS("条件コンパイル式にエラーがあります"))
RISSE_MSG_DECL(RisseCannotGetSuper, RISSE_WS("スーパークラスが存在しないかスーパークラスを特定できません"))
RISSE_MSG_DECL(RisseInvalidOpecode, RISSE_WS("不正な VM コードです"))
RISSE_MSG_DECL(RisseRangeError, RISSE_WS("値が範囲外です"))
RISSE_MSG_DECL(RisseAccessDenyed, RISSE_WS("読み込み専用あるいは書き込み専用プロパティに対して行えない操作をしようとしました"))
RISSE_MSG_DECL(RisseNativeClassCrash, RISSE_WS("実行コンテキストが違います"))
RISSE_MSG_DECL(RisseInvalidObject, RISSE_WS("オブジェクトはすでに無効化されています"))
RISSE_MSG_DECL(RisseCannotOmit, RISSE_WS("\"...\" は関数外では使えません"))
RISSE_MSG_DECL(RisseCannotParseDate, RISSE_WS("不正な日付文字列の形式です"))
RISSE_MSG_DECL(RisseInvalidValueForTimestamp, RISSE_WS("不正な日付・時刻です"))
RISSE_MSG_DECL(RisseExceptionNotFound, RISSE_WS("\"Exception\" が存在しないため例外オブジェクトを作成できません"))
RISSE_MSG_DECL(RisseInvalidFormatString, RISSE_WS("不正な書式文字列です"))
RISSE_MSG_DECL(RisseDivideByZero, RISSE_WS("0 で除算をしようとしました"))
RISSE_MSG_DECL(RisseNotReconstructiveRandomizeData, RISSE_WS("乱数系列を初期化できません(おそらく不正なデータが渡されました)"))
RISSE_MSG_DECL(RisseSymbol, RISSE_WS("識別子"))
RISSE_MSG_DECL(RisseCallHistoryIsFromOutOfRisseScript, RISSE_WS("[Risseスクリプト管理外]"))
RISSE_MSG_DECL(RisseNObjectsWasNotFreed, RISSE_WS("合計 %1 個のオブジェクトが解放されていません"))
#ifdef RISSE_TEXT_OUT_CRLF
RISSE_MSG_DECL(RisseObjectCreationHistoryDelimiter, RISSE_WS("\r\n                     "))
#else
RISSE_MSG_DECL(RisseObjectCreationHistoryDelimiter, RISSE_WS("\n                     "))
#endif
#ifdef RISSE_TEXT_OUT_CRLF
RISSE_MSG_DECL(RisseObjectWasNotFreed,
				 RISSE_WS("オブジェクト %1 [%2] が解放されていません。オブジェクト作成時の呼び出し履歴は以下の通りです:\r\n                     %3"))
#else
RISSE_MSG_DECL(RisseObjectWasNotFreed,
				 RISSE_WS("オブジェクト %1 [%2] が解放されていません。オブジェクト作成時の呼び出し履歴は以下の通りです:\n                     %3"))
#endif
RISSE_MSG_DECL(RisseGroupByObjectTypeAndHistory, RISSE_WS("オブジェクトのタイプとオブジェクト作成時の履歴による分類"))
RISSE_MSG_DECL(RisseGroupByObjectType, RISSE_WS("オブジェクトのタイプによる分類"))
#ifdef RISSE_TEXT_OUT_CRLF
RISSE_MSG_DECL(RisseObjectCountingMessageGroupByObjectTypeAndHistory,
				 RISSE_WS("%1 個 : [%2]\r\n                     %3"))
#else
RISSE_MSG_DECL(RisseObjectCountingMessageGroupByObjectTypeAndHistory,
				 RISSE_WS("%1 個 : [%2]\n                     %3"))
#endif
RISSE_MSG_DECL(RisseObjectCountingMessageRisseGroupByObjectType, RISSE_WS("%1 個 : [%2]"))
#ifdef RISSE_TEXT_OUT_CRLF
RISSE_MSG_DECL(RisseWarnRunningCodeOnDeletingObject, RISSE_WS("%4: 削除中のオブジェクト %1[%2] 上でコードが実行されています。このオブジェクトの作成時の呼び出し履歴は以下の通りです:\r\n                     %3"))
#else
RISSE_MSG_DECL(RisseWarnRunningCodeOnDeletingObject, RISSE_WS("%4: 削除中のオブジェクト %1[%2] 上でコードが実行されています。このオブジェクトの作成時の呼び出し履歴は以下の通りです:\n                     %3"))
#endif
RISSE_MSG_DECL(RisseWriteError, RISSE_WS("書き込みエラーが発生しました"))
RISSE_MSG_DECL(RisseReadError, RISSE_WS("読み込みエラーが発生しました。ファイルが破損している可能性や、デバイスからの読み込みに失敗した可能性があります"))
RISSE_MSG_DECL(RisseSeekError, RISSE_WS("シークエラーが発生しました。ファイルが破損している可能性や、デバイスからの読み込みに失敗した可能性があります"))
//---------------------------------------------------------------------------
