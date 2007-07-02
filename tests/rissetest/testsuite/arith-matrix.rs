// library for examining operator matrix test


var E = "errors";


function test(labels, values1, values2, expects) testmethod
{
	var checks = '';
	var test_count = 0;
	var pass_count = 0;

	checks += "   ";
	for(var i = 0; i < labels.length ; i++)
		checks += labels[i]+ "2 ";
	checks += "\n";

	for(var r1 = 0; r1 < expects.length; r1++)
	{
		checks += labels[r1] + "1 ";
		var expects_row = expects[r1];
		for(var r2 = 0; r2 < expects_row.length; r2++)
		{
			var result;
			try
			{
				result = testmethod(values1[r1], values2[r2]);
			}
			catch(e)
			{
				result = E;
			}
			if(result === expects_row[r2])
				checks += '-- ', pass_count ++;
			else
				checks += 'NG ';

			test_count ++;
		}
		checks += "\n";
	}

	if(pass_count == test_count) return "ok";

	Script.print("Error!! (see below)\n");
	Script.print(checks);

	return "failed";
}
