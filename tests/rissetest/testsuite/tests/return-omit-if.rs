function a(x)
{
	if(x)
		"true"
	else
		"false"
}

function b(x)
{
	if(x) "true";
}

a(false) + ":" + a(true) + ":" +
b(false) + ":" + b(true)

//=> "false:true::true"

