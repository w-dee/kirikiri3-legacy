class C
{
}

class D : C
{
}


module M
{
}

module N
{
}

C.include(M);
D.include(N);

var c = new C();
var d = new D();

function tf(b) { return b?'o':'.'; }

var ret = 
	tf(c .isA(C)) +			// o
	tf(c .isA(D)) +			// .
	tf(c .isA(c)) +			// .
	tf(c .isA(d)) +			// .
	tf(c .isA(M)) +			// o
	tf(c .isA(N)) +			// .
	tf(c .isA(Object)) +		// o
	tf(c .isA(Array)) +		// .
	tf(c .isA(Array)) +		// .
	'|' +
	tf(d .isA(C)) +			// o
	tf(d .isA(D)) +			// o
	tf(d .isA(c)) +			// .
	tf(d .isA(d)) +			// .
	tf(d .isA(M)) +			// o
	tf(d .isA(N)) +			// o
	tf(d .isA(Object)) +		// o
	tf(d .isA(Array)) +		// .
	tf(d .isA(Array)) +		// .
	'|' +
	tf(1 .isA(Integer)) +		// o
	tf(1 .isA(Real)) +			// .
	tf(1 .isA(Number)) +		// o
	tf(1 .isA(String)) +		// .
	tf(1 .isA(Primitive)) +	// o
	tf(1 .isA(Object)) +		// o
	tf(1 .isA(1)) +			// .
	'|' +
	tf(1.4 .isA(Integer)) +	// .
	tf(1.4 .isA(Real)) +		// o
	tf(1.4 .isA(Number)) +		// o
	tf(1.4 .isA(String)) +		// .
	tf(1.4 .isA(Primitive)) +	// o
	tf(1.4 .isA(Object)) +		// o
	tf(1.4 .isA(1)) +			// .
	'|' +
	tf("1.4" .isA(Integer)) +	// .
	tf("1.4" .isA(Real)) +		// .
	tf("1.4" .isA(Number)) +	// .
	tf("1.4" .isA(String)) +	// o
	tf("1.4" .isA(Primitive)) +// o
	tf("1.4" .isA(Object)) +	// o
	tf("1.4" .isA(1)) +		// .
	'|' +
	tf(global .isA(Object)) +	// o
	tf(global .isA(1)) +		// .
	'|' +
	tf(Object .isA(Class)) +		// o
	tf(Object .isA(Module)) +		// o
	tf(Object .isA(Object)) +		// o
	tf(Object .isA(C)) +			// .
	tf(Object .isA(M)) +			// .
	'|' +
	tf(Array .isA(Class)) +		// o
	tf(Array .isA(Module)) +		// o
	tf(Array .isA(Object)) +		// o
	tf(Array .isA(C)) +			// .
	tf(Array .isA(M)) +			// .
	'|' +
	tf(Class .isA(Class)) +		// o
	tf(Class .isA(Module)) +		// o
	tf(Class .isA(Object)) +		// o
	tf(Class .isA(C)) +			// .
	tf(Class .isA(M)) +			// .
	'|' +
	tf(C .isA(Class)) +		// o
	tf(C .isA(Module)) +		// o
	tf(C .isA(Object)) +		// o
	tf(C .isA(C)) +			// .
	tf(C .isA(M)) +			// .
	'|' +
	tf(M .isA(Class)) +		// .
	tf(M .isA(Module)) +		// o
	tf(M .isA(Object)) +		// o
	tf(M .isA(C)) +			// .
	tf(M .isA(M)) +			// .
	'';

return ret; //=> "o...o.o..|oo..ooo..|o.o.oo.|.oo.oo.|...ooo.|o.|ooo..|ooo..|ooo..|ooo..|.oo.."
