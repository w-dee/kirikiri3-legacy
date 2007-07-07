
var td = [
 [ 1 , 2 , 3 ],
 ["a","b","c"],
 [1.5,2.5,3.5],
];


var res = '';
for(var y = 0; y < td.length; y++)
{
	var row = td[y];
	for(var x = 0; x < row.length; x++)
	{
		res += ':' + row[x].toString();
	}
	res += "|";
}


res;
//=> ":1:2:3|:a:b:c|:1.5:2.5:3.5|"
