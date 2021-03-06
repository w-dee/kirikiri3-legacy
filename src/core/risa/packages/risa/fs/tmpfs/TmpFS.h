//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tmpfs の実装
//---------------------------------------------------------------------------
#ifndef TmpFSH
#define TmpFSH

#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/fs/tmpfs/MemoryStream.h"
#include "risa/packages/risa/fs/FileSystem.h"
#include "risa/common/RisaThread.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ノードを表すクラス
 */
class tTmpFSNode : public tCollectee
{
public:
	enum tType { ntDirectory, ntFile  };

private:
	tTmpFSNode * Parent; //!< 親へのリンク
	tType Type; //!< ノードのタイプ
	tString Name;
	union
	{
		tHashTable<tString, tTmpFSNode *> * Directory; //!< ディレクトリ
		tMemoryStreamBlock * File; //!< ファイル
	};
public:
	/**
	 * コンストラクタ
	 * @param parent	親ノード
	 * @param type		ノードタイプ
	 * @param name		ノードの名前
	 */
	tTmpFSNode(tTmpFSNode *parent, tType type, const tString & name);

	/**
	 * シリアライズされたデータを読み取るコンストラクタ
	 * @param parent	親ノード
	 * @param type		ノードタイプ
	 * @param src		入力もとストリーム
	 */
	tTmpFSNode(tTmpFSNode *parent, tType type, tStreamAdapter src);

public:
	/**
	 * 内容をシリアライズする
	 * @param dest	出力先ストリーム
	 */
	void Serialize(tStreamAdapter dest) const;

	/**
	 * 指定された名前を持つノードを返す
	 * @param name	名前
	 * @return	ノード(ノードが見つからない場合は NULL)
	 * @note	このノードが File を表す場合も NULL が返る
	 */
	tTmpFSNode * GetSubNode(const tString & name);

	/**
	 * 指定された名前を持つサブノードを削除する
	 * @param name	名前
	 * @return	削除に成功すれば真
	 */
	bool DeleteSubNodeByName(const tString & name);

	/**
	 * 指定された名前を持つディレクトリを作成する
	 * @param name	名前
	 * @return	新規に作成されたディレクトリノード
	 * @note	すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
	 */
	tTmpFSNode * CreateDirectory(const tString & name);

	/**
	 * 指定された名前を持つファイルを作成する
	 * @param name	名前
	 * @return	新規に作成されたファイルノード
	 * @note	すでにその名前を持つノードがあった場合は何もしないで NULL を返すので注意
	 */
	tTmpFSNode * CreateFile(const tString & name);

	tType GetType() const { return Type; }
	bool IsFile() const { return Type == ntFile; }
	bool IsDirectory() const { return Type == ntDirectory; }
	tMemoryStreamBlock * GetMemoryStreamBlock() { return File; }
	tTmpFSNode * GetParent() { return Parent; }
	const tString & GetName() const { return Name; }
	bool HasSubNode() const { return Type == ntDirectory &&
							Directory->GetCount() != 0; }

	/**
	 * ノードのサイズを取得する
	 * @return	ノードのサイズ (ファイルの場合はファイルサイズ、そうでない場合は 0)
	 */
	risse_size GetSize() const;

	/**
	 * すべての子要素に対して callback を呼び出す
	 * @return	callback を呼び出した回数
	 */
	size_t Iterate(tVariant & callback);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * tmp ファイルシステム
 */
class tTmpFSInstance : public tFileSystemInstance
{
	tTmpFSNode * Root; //!< ルートノード

	/**
	 * シリアライズ時のファイルの先頭に着くマジック
	 */
	static const unsigned char SerializeMagic[];

public:
	/**
	 * コンストラクタ
	 */
	tTmpFSInstance();

private:
	/**
	 * 指定されたストリームに内容をシリアライズする
	 * @param dest	出力先ストリーム
	 */
	void SerializeTo(tStreamAdapter dest);

	/**
	 * 指定されたストリームから内容を復元する
	 * @param src	入力元ストリーム
	 */
	void UnserializeFrom(tStreamAdapter src);

	/**
	 * 指定された位置のノードを得る
	 * @param name	ノード
	 * @return	その位置にあるノード。その位置が見つからない場合は NULL
	 */
	tTmpFSNode * GetNodeAt(const tString & name);

	/**
	 * ルートディレクトリを作成する
	 */
	void CreateRoot();

	/**
	 * 内容をすべてクリアする
	 */
	void Clear();

public: // Risse 用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);


	//-- FileSystem メンバ
	/**
	 * ファイル一覧をコールバックで受け取る
	 * @param dirname	ディレクトリ名
	 * @param args		追加パラメータ(ブロック引数としてコールバック関数オブジェクト)
	 * @return	取得できたファイル数
	 */
	size_t walkAt(const tString & dirname,
		const tMethodArgument &args);

	/**
	 * ファイルが存在するかどうかを得る
	 * @param filename	ファイル名
	 * @return	ファイルが存在する場合真
	 */
	bool isFile(const tString & filename);

	/**
	 * ディレクトリが存在するかどうかを得る
	 * @param dirname	ディレクトリ名
	 * @return	ディレクトリが存在する場合真
	 */
	bool isDirectory(const tString & dirname);

	/**
	 * ファイルを削除する
	 * @param filename	ファイル名
	 */
	void removeFile(const tString & filename);

	/**
	 * ディレクトリを削除する
	 * @param dirname	ディレクトリ名
	 * @param args		追加パラメータ(再帰的にディレクトリを削除するかどうか)
	 */
	void removeDirectory(const tString & dirname, const tMethodArgument &args);

	/**
	 * ディレクトリを作成する
	 * @param dirname	ディレクトリ名
	 */
	void createDirectory(const tString & dirname);

	/**
	 * 指定されたファイルの stat を得る
	 * @param filename	ファイル名
	 * @return	stat の結果を表す辞書配列
	 */
	tObjectInterface * stat(const tString & filename);

	/**
	 * 指定されたファイルのストリームを得る
	 * @param filename	ファイル名
	 * @param flags		フラグ
	 * @return	ストリームオブジェクト
	 */
	tStreamInstance * open(const tString & filename,
		risse_uint32 flags);

	/**
	 * ディスク(等に)内容をフラッシュする
	 * @note	このメソッドが呼ばれたら、ファイルシステムはそれまでの
	 *			変更点を確実にディスクなどに永続化することが望まれる。
	 *			逆にこのメソッドが呼ばれずにプログラムが終了したり
	 *			ファイルシステムを開放した場合は、永続化する保証はない。
	 *			永続化されなかった場合、それまでの変更内容が中途半端に
	 *			反映された状態になるのか、あるいはそれまでの変更内容が
	 *			全くなかったかにされるかは、ファイルシステムの実装による。
	 */
	void flush();

	//-- FileSystem メンバ ここまで

	/**
	 * 内容をファイルに保存する
	 * @param filename	保存先のストリームまたはファイル名
	 */
	void save(const tVariant & filename);

	/**
	 * 内容をファイルから読み出す
	 * @param filename	読み込むストリームまたはファイル名
	 */
	void load(const tVariant & filename);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * "TmpFS" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tTmpFSClass, tClassBase, tTmpFSInstance, itNormal)
private:
	tMemoryStreamClass * MemoryStreamClass; //!< MemoryStream クラスインスタンス
public:
	/**
	 * MemoryStream クラスインスタンスを得る
	 */
	tMemoryStreamClass * GetMemoryStreamClass() const { return MemoryStreamClass; }
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
} // namespace Risa


#endif
