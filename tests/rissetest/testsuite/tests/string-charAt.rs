var s1 = "abcdef"; // 6 length

var p0 = s1.charAt(0); // "a"
var p3 = s1.charAt(3); // "d"
var p5 = s1.charAt(5); // "f"
var p_1 = s1.charAt(-1); // "f" 負のインデックスの場合は文字列の最後から数える
var p_6 = s1.charAt(-6); // "a"

var p6 = s1.charAt(6); // void
var p_7 = s1.charAt(7); // void


return p0 + p3 + p5 + p_1 + p_6 + (p6===void?'-':'!') + (p_7===void?'-':'!');
	//=> "adffa--"
