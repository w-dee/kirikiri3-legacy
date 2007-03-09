var C = new Class();
var C.m = function() { return "1"; };

var D = new Class(C);
var D.m = function () { return "2"; };

var i = new C();
return i.m(); //=> "1"

