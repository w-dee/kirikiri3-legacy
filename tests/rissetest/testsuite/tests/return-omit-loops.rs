function cv(x) { if(x === void) "void"; else x.toString(); }

function for_()
{
	for(var i = 0; i < 5; i++) ;
}

function while_()
{
	var i = 0;
	while(++i < 4) ;
}


function do_while()
{
	var i = 0;
	do; while(++i < 4) ;
}

cv(for_()) + ":" +
cv(while_()) + ":" +
cv(do_while())  //=> "void:void:void"

