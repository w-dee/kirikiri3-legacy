class C {
	function m() { return "C"; }
	function id() { return "1"; }
}

class D extends C {
	function m() { return "D"; }
}

//-------------
var c = new C();
var d = new D();


return c.m() + c.id() + d.m() + d.id(); //=> "C1D1"


