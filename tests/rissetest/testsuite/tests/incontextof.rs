

var o = new Object();

var o.value = 5;


var m = function () { return this.value; };

return (m incontextof o)(); //=> 5



