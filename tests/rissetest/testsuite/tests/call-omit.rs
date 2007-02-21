global.a = '';
global.b = '';
global.c = '';

function f(a, b, c)
{
	global.a += a;
	global.b += b;
	global.c += c;
}

function g()
{
	f(...);
}

class C
{
	function initialize(a, b, c)
	{
		global.a += a;
		global.b += b;
		global.c += c;
	}
}


function h()
{
	new C(...);
}

g('i', 'j', 'k') {};
g('i', 'j', 'k');
h('i', 'j', 'k');



return global.a + global.b + global.c; //=> "iiijjjkkk"

