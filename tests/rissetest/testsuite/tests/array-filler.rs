var a = new Array();
a[2] = '!'; // a = [void, void, '!']
a.filler = '-';
a[5] = '!'; // a = [void, void, '!', '-', '-', '!']

for(var i = 0; i < a.length; i++)
	{ if(a[i] === void) a[i] = 'v'; } // void を 'v' に変換する

return a[0]+a[1]+a[2]+a[3]+a[4]+a[5]; //=> "vv!--!"

