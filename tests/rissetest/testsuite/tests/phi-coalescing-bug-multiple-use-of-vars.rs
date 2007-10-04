var cnt;
function c() { return ++cnt <= 1; }
{

	var a = 1;
	var b = 0;

	while(global.c())
	{
		b = a;
		a = 2;
	}
	b; //=> 1
}
