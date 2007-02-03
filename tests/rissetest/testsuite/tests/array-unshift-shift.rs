var a = new Array();
a.unshift(4, 2);
a.unshift(3);
var a1 = a.shift();
var a2 = a.shift();
var a3 = a.shift();
return a1*a2+a3; //=> 14
