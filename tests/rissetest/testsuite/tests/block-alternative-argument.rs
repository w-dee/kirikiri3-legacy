var ret = "";

function f() a b
{
	ret += a + b();
}

function g(;a,b) c
{
	ret += a + b + c();
}


f(;"2",{ "y" });
g(;"3","4",{ "x" });


ret; //=> "2y34x"


