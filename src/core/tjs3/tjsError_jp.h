//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 日本語ローカライズメッセージ
//---------------------------------------------------------------------------

TJS_MSG_DECL(TJSInternalError, TJS_WS("内部エラーが発生しました"))
TJS_MSG_DECL(TJSWarning, TJS_WS("警告: "))
TJS_MSG_DECL(TJSWarnEvalOperator, TJS_WS("グローバルでない場所で後置 ! 演算子が使われています(この演算子の挙動はTJS3 version 2.4.1 で変わりましたのでご注意ください)"))
TJS_MSG_DECL(TJSNarrowToWideConversionError, TJS_WS("ANSI 文字列を UNICODE 文字列に変換できません。現在のコードページで解釈できない文字が含まれてます。正しいデータが指定されているかを確認してください。データが破損している可能性もあります"))
TJS_MSG_DECL(TJSVariantConvertError, TJS_WS("%1 から %2 へ型を変換できません"))
TJS_MSG_DECL(TJSVariantConvertErrorToObject, TJS_WS("%1 から Object へ型を変換できません。Object 型が要求される文脈で Object 型以外の値が渡されるとこのエラーが発生します"))
TJS_MSG_DECL(TJSIDExpected, TJS_WS("識別子を指定してください"))
TJS_MSG_DECL(TJSSubstitutionInBooleanContext, TJS_WS("論理値が求められている場所で = 演算子が使用されています(== 演算子の間違いですか？代入した上でゼロと値を比較したい場合は、(A=B) != 0 の形式を使うことをお勧めします)"));
TJS_MSG_DECL(TJSCannotModifyLHS, TJS_WS("不正な代入か不正な式の操作です"))
TJS_MSG_DECL(TJSInsufficientMem, TJS_WS("メモリが足りません"))
TJS_MSG_DECL(TJSCannotGetResult, TJS_WS("この式からは値を得ることができません"))
TJS_MSG_DECL(TJSNullAccess, TJS_WS("null オブジェクトにアクセスしようとしました"))
TJS_MSG_DECL(TJSMemberNotFound, TJS_WS("メンバ \"%1\" が見つかりません"))
TJS_MSG_DECL(TJSMemberNotFoundNoNameGiven, TJS_WS("メンバが見つかりません"))
TJS_MSG_DECL(TJSNotImplemented, TJS_WS("呼び出そうとした機能は未実装です"))
TJS_MSG_DECL(TJSInvalidParam, TJS_WS("不正な引数です"))
TJS_MSG_DECL(TJSBadParamCount, TJS_WS("引数の数が不正です"))
TJS_MSG_DECL(TJSInvalidType, TJS_WS("関数ではないかプロパティの種類が違います"))
TJS_MSG_DECL(TJSSpecifyDicOrArray, TJS_WS("Dictionary または Array クラスのオブジェクトを指定してください"))
TJS_MSG_DECL(TJSSpecifyArray, TJS_WS("Array クラスのオブジェクトを指定してください"))
TJS_MSG_DECL(TJSStringDeallocError, TJS_WS("文字列メモリブロックを解放できません"))
TJS_MSG_DECL(TJSStringAllocError, TJS_WS("文字列メモリブロックを確保できません"))
TJS_MSG_DECL(TJSMisplacedBreakContinue, TJS_WS("\"break\" または \"continue\" はここに書くことはできません"))
TJS_MSG_DECL(TJSMisplacedCase, TJS_WS("\"case\" はここに書くことはできません"))
TJS_MSG_DECL(TJSMisplacedReturn, TJS_WS("\"return\" はここに書くことはできません"))
TJS_MSG_DECL(TJSStringParseError, TJS_WS("文字列定数/正規表現/オクテット即値が終わらないままスクリプトの終端に達しました"))
TJS_MSG_DECL(TJSNumberError, TJS_WS("数値として解釈できません"))
TJS_MSG_DECL(TJSUnclosedComment, TJS_WS("コメントが終わらないままスクリプトの終端に達しました"))
TJS_MSG_DECL(TJSInvalidChar, TJS_WS("不正な文字です : \'%1\'"))
TJS_MSG_DECL(TJSExpected, TJS_WS("%1 がありません"))
TJS_MSG_DECL(TJSSyntaxError, TJS_WS("文法エラーです(%1)"))
TJS_MSG_DECL(TJSPPError, TJS_WS("条件コンパイル式にエラーがあります"))
TJS_MSG_DECL(TJSCannotGetSuper, TJS_WS("スーパークラスが存在しないかスーパークラスを特定できません"))
TJS_MSG_DECL(TJSInvalidOpecode, TJS_WS("不正な VM コードです"))
TJS_MSG_DECL(TJSRangeError, TJS_WS("値が範囲外です"))
TJS_MSG_DECL(TJSAccessDenyed, TJS_WS("読み込み専用あるいは書き込み専用プロパティに対して行えない操作をしようとしました"))
TJS_MSG_DECL(TJSNativeClassCrash, TJS_WS("実行コンテキストが違います"))
TJS_MSG_DECL(TJSInvalidObject, TJS_WS("オブジェクトはすでに無効化されています"))
TJS_MSG_DECL(TJSCannotOmit, TJS_WS("\"...\" は関数外では使えません"))
TJS_MSG_DECL(TJSCannotParseDate, TJS_WS("不正な日付文字列の形式です"))
TJS_MSG_DECL(TJSInvalidValueForTimestamp, TJS_WS("不正な日付・時刻です"))
TJS_MSG_DECL(TJSExceptionNotFound, TJS_WS("\"Exception\" が存在しないため例外オブジェクトを作成できません"))
TJS_MSG_DECL(TJSInvalidFormatString, TJS_WS("不正な書式文字列です"))
TJS_MSG_DECL(TJSDivideByZero, TJS_WS("0 で除算をしようとしました"))
TJS_MSG_DECL(TJSNotReconstructiveRandomizeData, TJS_WS("乱数系列を初期化できません(おそらく不正なデータが渡されました)"))
TJS_MSG_DECL(TJSSymbol, TJS_WS("識別子"))
TJS_MSG_DECL(TJSCallHistoryIsFromOutOfTJS3Script, TJS_WS("[TJSスクリプト管理外]"))
TJS_MSG_DECL(TJSNObjectsWasNotFreed, TJS_WS("合計 %1 個のオブジェクトが解放されていません"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_WS("\r\n                     "))
#else
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_WS("\n                     "))
#endif
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_WS("オブジェクト %1 [%2] が解放されていません。オブジェクト作成時の呼び出し履歴は以下の通りです:\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_WS("オブジェクト %1 [%2] が解放されていません。オブジェクト作成時の呼び出し履歴は以下の通りです:\n                     %3"))
#endif
TJS_MSG_DECL(TJSGroupByObjectTypeAndHistory, TJS_WS("オブジェクトのタイプとオブジェクト作成時の履歴による分類"))
TJS_MSG_DECL(TJSGroupByObjectType, TJS_WS("オブジェクトのタイプによる分類"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_WS("%1 個 : [%2]\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_WS("%1 個 : [%2]\n                     %3"))
#endif
TJS_MSG_DECL(TJSObjectCountingMessageTJSGroupByObjectType, TJS_WS("%1 個 : [%2]"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_WS("%4: 削除中のオブジェクト %1[%2] 上でコードが実行されています。このオブジェクトの作成時の呼び出し履歴は以下の通りです:\r\n                     %3"))
#else
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_WS("%4: 削除中のオブジェクト %1[%2] 上でコードが実行されています。このオブジェクトの作成時の呼び出し履歴は以下の通りです:\n                     %3"))
#endif
TJS_MSG_DECL(TJSWriteError, TJS_WS("書き込みエラーが発生しました"))
TJS_MSG_DECL(TJSReadError, TJS_WS("読み込みエラーが発生しました。ファイルが破損している可能性や、デバイスからの読み込みに失敗した可能性があります"))
TJS_MSG_DECL(TJSSeekError, TJS_WS("シークエラーが発生しました。ファイルが破損している可能性や、デバイスからの読み込みに失敗した可能性があります"))
//---------------------------------------------------------------------------
