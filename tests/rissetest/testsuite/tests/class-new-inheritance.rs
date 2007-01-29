var C = new Class();
C.m = function() { return "This is C.m"; };
var D = new Class(C);

var i = new C();
return i.m(); //=> "This is C.m"

