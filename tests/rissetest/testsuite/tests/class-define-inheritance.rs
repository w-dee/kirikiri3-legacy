class C {
	function toString() { return "C"; }
	function id() { return "1"; }
}
class D extends C {
	function toString() { return "D"; }
}

/*
var C = new Class();
C.toString = function() { return "C"; };
C.id = function() { return "1"; };
var D = new Class(C);
D.toString = function () { return "D"; };
*/
var c = new C();
var d = new D();

return c.toString() + c.id() + d.toString() + d.id(); //=> "C1D1"


