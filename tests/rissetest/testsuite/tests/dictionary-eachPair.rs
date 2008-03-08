var dic = new Dictionary();

dic["a"] = "string a";
dic["b"] = "string b";
dic["c"] = "string c";

var a, b, c;

dic.eachPair() { |key, value|
	global.(key) = value;
}


"\{a}:\{b}:\{c}" //=> "string a:string b:string c"

