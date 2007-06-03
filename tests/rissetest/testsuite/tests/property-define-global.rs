var global.v = 0;

property p
{
	getter ()  { return -global.v; }
	setter (x) { global.v = -x; }
}

p = 6;
var s1 = v;
var s2 = p;

return "result: " + s1 + "," + s2; //=> "result: -6,6"

