var ret;

function f() a b
{
	ret = a + b();
}


f(;"2",{ "y" });


ret; //=> "2y"


