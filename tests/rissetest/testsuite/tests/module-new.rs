global.s = '';

var M1 = new Module();
M1.push1 = function () { global.s += '1'; };

var M2 = Module.new();
M2.push2 = function () { global.s += '2'; };

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
