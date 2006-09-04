// スクリプト言語Risse用テストスクリプト

var repeat_4 = function () block block2
{
	for(var i = 1; i <= 4; i++) block(i);
	for(var i = 5; i <= 8; i++) block2(i);
};

var n = 0;

//repeat_4() function(v) { n += v; } function(v) { n += v; };
repeat_4() { |v| n += v; }{ |v| n += v; };

return n; //=> 36
