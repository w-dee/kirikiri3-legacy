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
	tf(c instanceof C) +			// o
	tf(c instanceof D) +			// .
	tf(c instanceof c) +			// .
	tf(c instanceof d) +			// .
	tf(c instanceof M) +			// o
	tf(c instanceof N) +			// .
	tf(c instanceof Object) +		// o
	tf(c instanceof Array) +		// .
	tf(c instanceof Array) +		// .
	'|' +
	tf(d instanceof C) +			// o
	tf(d instanceof D) +			// o
	tf(d instanceof c) +			// .
	tf(d instanceof d) +			// .
	tf(d instanceof M) +			// o
	tf(d instanceof N) +			// o
	tf(d instanceof Object) +		// o
	tf(d instanceof Array) +		// .
	tf(d instanceof Array) +		// .
	'|' +
	tf(1 instanceof Integer) +		// o
	tf(1 instanceof Real) +			// .
	tf(1 instanceof Number) +		// o
	tf(1 instanceof String) +		// .
	tf(1 instanceof Primitive) +	// o
	tf(1 instanceof Object) +		// o
	tf(1 instanceof 1) +			// .
	'|' +
	tf(1.4 instanceof Integer) +	// .
	tf(1.4 instanceof Real) +		// o
	tf(1.4 instanceof Number) +		// o
	tf(1.4 instanceof String) +		// .
	tf(1.4 instanceof Primitive) +	// o
	tf(1.4 instanceof Object) +		// o
	tf(1.4 instanceof 1) +			// .
	'|' +
	tf("1.4" instanceof Integer) +	// .
	tf("1.4" instanceof Real) +		// .
	tf("1.4" instanceof Number) +	// .
	tf("1.4" instanceof String) +	// o
	tf("1.4" instanceof Primitive) +// o
	tf("1.4" instanceof Object) +	// o
	tf("1.4" instanceof 1) +		// .
	'|' +
	tf(global instanceof Object) +	// o
	tf(global instanceof 1) +		// .
	'|' +
	tf(Object instanceof Class) +		// o
	tf(Object instanceof Module) +		// o
	tf(Object instanceof Object) +		// o
	tf(Object instanceof C) +			// .
	tf(Object instanceof M) +			// .
	'|' +
	tf(Array instanceof Class) +		// o
	tf(Array instanceof Module) +		// o
	tf(Array instanceof Object) +		// o
	tf(Array instanceof C) +			// .
	tf(Array instanceof M) +			// .
	'|' +
	tf(Class instanceof Class) +		// o
	tf(Class instanceof Module) +		// o
	tf(Class instanceof Object) +		// o
	tf(Class instanceof C) +			// .
	tf(Class instanceof M) +			// .
	'|' +
	tf(C instanceof Class) +		// o
	tf(C instanceof Module) +		// o
	tf(C instanceof Object) +		// o
	tf(C instanceof C) +			// .
	tf(C instanceof M) +			// .
	'|' +
	tf(M instanceof Class) +		// .
	tf(M instanceof Module) +		// o
	tf(M instanceof Object) +		// o
	tf(M instanceof C) +			// .
	tf(M instanceof M) +			// .
	'';

return ret; //=> "o...o.o..|oo..ooo..|o.o.oo.|.oo.oo.|...ooo.|o.|ooo..|ooo..|ooo..|ooo..|.oo.."
