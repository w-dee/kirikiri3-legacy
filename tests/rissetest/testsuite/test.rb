#!/usr/bin/ruby

BASE_DIR = File.dirname(__FILE__)
EXECUTABLE = BASE_DIR + '/../../../build_output/bin/rissetest'
TESTS_DIR = BASE_DIR + '/tests/'
TEMP_DIR = BASE_DIR

errored = []
test_count = 0

files = Dir.glob(TESTS_DIR + '*.rs').sort.delete_if { |x| x =~ /^\./ }

# if an argument is specified, pick up files witch include specified string.
if ARGV[0]
	files.delete_if { |x| !x[ARGV[0]] }
end

files.each do |file|
	begin
		# search test result string from file in question
		match = IO.read(file).match(/\/\/=>\s+([^\r\n]+)/)
		if match == nil
			raise "file does not contain result pattern"
		else
			pattern = match[1]
			print "testing #{File.basename file} ... "
			STDOUT.flush
			system("\"#{File.expand_path(EXECUTABLE)}\" " +
						" #{file} 1>#{TEMP_DIR}/stdout.log 2>#{TEMP_DIR}/stderr.log");
			result = IO.read("#{TEMP_DIR}/stdout.log")
			matched = false
			if p = pattern.match(/\/(.*)\//)
				matched = result =~ Regexp.compile(p[1])
			else
				matched = result == pattern
			end

			if !matched
				print "\rfailed!\n"
				raise "test failed.\n" +
						"  resulted: #{result}\n" +
						"  expected: #{match[1]}\n" +
						"  To examine the test:\n" +
						"    #{EXECUTABLE} #{file}"
			else
				print "\rok     \n"
			end
			STDOUT.flush
		end
	rescue => e
		errored << "#{file}: #{e.message}"
	end
	test_count += 1
end

print "========================================\n"

case errored.size
when 0
	print "All tests passed.\n"
when 1
	print "Test failed.\nFailed test is:\n"
else
	print "Test failed.\nFailed tests are:\n"
end

if errored.size > 0
	errored.each do |i|
		print "---------------------------------------\n"
		print "#{i}\n"
	end
	print "---------------------------------------\n"
end

print "\n"
print "Passed #{test_count - errored.size} / Failed #{errored.size} / Total #{test_count}\n"

begin
	File.unlink("#{TEMP_DIR}/stdout.log")
	File.unlink("#{TEMP_DIR}/stderr.log")
rescue
	nil
end
