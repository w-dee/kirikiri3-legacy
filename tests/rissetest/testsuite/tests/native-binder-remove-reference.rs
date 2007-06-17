// const tRisseString & のような、参照付きの型がネイティブ関数の引数に
// 指定されていた際に、かつて Native Binder が変換に失敗していた場合があった。

// Object.eval の第１引数は const tRisseString & として宣言されている。

global.eval(1); //=> 1

