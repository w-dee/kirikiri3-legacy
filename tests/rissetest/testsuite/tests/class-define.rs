class C {
	function m() { return "C.m"; }
}

var c = new C();

Object.m =  function (){ return "Object.m"; };

return c.m(); //=> "C.m"


