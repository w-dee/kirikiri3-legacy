

var o = new Object();

o.value = 5;


var m = function () { return this.value; };

return (m incontextof o)(); //=> 5



