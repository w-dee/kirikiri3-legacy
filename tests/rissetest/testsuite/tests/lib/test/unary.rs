// library for examining operator unary test
var T = true;
var F = false;
var I = IllegalArgumentClassException;
var N = NoSuchMemberException;


function test(data, expects) testmethod
{
	var checks = '';
	var pass_count = 0;
	var test_count = 0;

	assert(data.length == expects.length);
	for(var i = 0; i < data.length; i++)
	{
		var result;
		try
		{
			result = testmethod(data[i]);
		}
		catch(e)
		{
			result = e.class;
		}
		if(result === expects[i])
			checks += '-- ',
			pass_count ++;
		else
			checks += 'NG ';
		test_count ++;
	}

	if(pass_count == test_count) return "ok";

	Script::print("Error!! (see below)\n");
	Script::print("\{checks}\n");

	return "failed";
}
