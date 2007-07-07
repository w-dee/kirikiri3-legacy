class C
{
	var name = "xx";
}

var i = new C();

return C.name + ':' + C::name + ':' + i.name; //=> "C:xx:xx"

