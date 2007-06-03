var global.s = '';

module M1
{
	function push1() { global.s += '1'; }
}

module M2
{
	function push2() { global.s += '2'; }
}


class C1
{
	include(global.M1);

	function m() { return "M"; }
}


class C2 extends C1
{
	include(global.M2);
}

var i = C2.new();

i.push1();
i.push2();

return global.s + i.m(); //=> "12M"
