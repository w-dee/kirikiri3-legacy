var C = Class.new();
C.toString = function () { return "This is a instance of class C"; };
var i = C.new();
return "Hey! " + i; //=> "Hey! This is a instance of class C"
