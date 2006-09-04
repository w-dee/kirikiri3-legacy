var repeat_4 = function () block
{
 // ブロックを４回実行する
 for(var i = 1; i <= 4; i++) block(i);
};
var n = 0;
repeat_4() { |v| n += v; };
return n; //=> 10
