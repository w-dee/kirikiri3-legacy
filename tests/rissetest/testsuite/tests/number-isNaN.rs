var n1 = 3.2;
var n2 = NaN;
var n3 = -NaN;
var n4 = 4;
var n5 = -Infinity;
var n6 = Infinity;

function yn (b) { return b?'y':'n'; }

return
	yn(n1.isNaN)+
	yn(n2.isNaN)+
	yn(n3.isNaN)+
	yn(n4.isNaN)+
	yn(n5.isNaN)+
	yn(n6.isNaN);
	//=> "nyynnn"

