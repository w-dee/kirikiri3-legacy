
var a = 0, A = 1;

{
	var l = 1, L = 2;

	[global.A, L] = [4, 5];
	[a, l] <-> [global.A, L];

	return "\{a}:\{l}"; //=> "4:5"
}

