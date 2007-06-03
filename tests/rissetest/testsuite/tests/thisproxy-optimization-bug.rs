var result = '';

class C {
	function initialize() { var this.i = 0; }
	function f(x) { global.result += x; this.i ++; }
	property v { getter { return this.i < 5; }}
}


var c1 = C.new();


while(true)
{
	if(c1.v) c1.f("a"); else { break; }
}

"ok" //=> "ok"

// earlier version of Risse has a optimization bug
// around this-proxy.
// buggy Risse will show "exception: member "c1" not found at u.rs:15".

