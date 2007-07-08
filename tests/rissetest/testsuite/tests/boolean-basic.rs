
function a(v)
{
	var a1 = v.toString();
	var a2 = v.dump();

	"\{a1}:\{a2}"
}

a(true) + ":" + a(false) //=> "true:true:false:false"
