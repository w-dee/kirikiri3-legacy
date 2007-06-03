// 継承のテスト
// Array クラスは Object クラスを継承しているため、Objectクラスにある
// getInstanceMember と setInstanceMember を呼べるはずである。
// object-new2.rs も参照。

var obj = new Array();
obj.setInstanceMember("a", 3);
return obj.getInstanceMember("a"); //=> 3


