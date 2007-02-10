function p1()
{
	var a = 0;
	return function (x) { a++; return a; };
};
q1 = p1();
q2 = p1();
q1();
return q2(); //=> 1

