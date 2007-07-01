
function a(v)
{
	var a1 = "" + v; // implicit toString
	var a2 = v.toString();
	var a3 = v.dump();

	@"${a1}:${a2}:${a3}"
}

a(true) + ":" + a(false) //=> "true:true:true:false:false:false"
