var C = Class.new();
var C.toString = function () { return "This is an instance of class C"; };
var i = C.new();
return "Hey! " + i; //=> "Hey! This is an instance of class C"
