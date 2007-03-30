function cv(x) { if(x === void) "void"; else x; }

function for_()
{
	for(var i = 0; i < 5; i++) ;
}

function for_break()
{
	for(var i = 0; i < 5; i++) 
	{
		break "B";
	}
}

function while_()
{
	var i = 0;
	while(++i < 4) ;
}

function while_break()
{
	var i = 0;
	while(++i < 4)
	{
		break "B";
	}
}


function do_while()
{
	var i = 0;
	do; while(++i < 4) ;
}

function do_while_break()
{
	var i = 0;
	do
	{
		break "B";
	}while(++i < 4) ;
}

cv(for_()) + ":" +
cv(for_break()) + ":" +
cv(while_()) + ":" +
cv(while_break()) + ":" +
cv(do_while()) + ":" +
cv(do_while_break()) 
 //=> "void:B:void:B:void:B"

