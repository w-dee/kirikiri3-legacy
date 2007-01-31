class C {
	function m() { return "1"; }
}

class D extends C {
}


D.m = function () { return "2"; };

var i = new C();
return i.m(); //=> "1"


