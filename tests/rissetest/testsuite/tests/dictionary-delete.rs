var dic = new Dictionary();

dic["a"] = "string a";
dic["b"] = "string b";
dic["c"] = "string c";

dic.default = "-";

var ret = '';
ret += delete dic["a"] + ":";
ret += delete dic["b"] + ":";
ret += dic.delete[]("c") + ":";
ret += dic.delete[]("hoge"); // this should be '-'


ret; //=> "string a:string b:string c:-"

