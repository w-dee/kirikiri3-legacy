var t = '!';
var a = new Array(t, t, t, t);

var test1 = a[-5] === void ? 't' : 'f';
var test2 = a[4]  === void ? 't' : 'f';

a.default = '-';

var test3 = a[-5] === '-' ? 't' : 'f';
var test4 = a[4]  === '-' ? 't' : 'f';

return test1 + test2 + test3 + test4; //=> "tttt"

