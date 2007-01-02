#!/usr/bin/ruby

BASE_DIR = File.dirname(__FILE__)
EXECUTABLE = BASE_DIR + '/../../../build_output/bin/rissetest.exe'
TESTS_DIR = BASE_DIR + '/tests/'
TEMP_DIR = BASE_DIR

errored = []
test_count = 0

Dir.glob(TESTS_DIR + '*').sort.delete_if { |x| x =~ /^\./ }.each do |file|
	begin
		# search test result string from file in question
		match = IO.read(file).match(/\/\/=>\s+([^\r\n]+)/)
		if match == nil
			raise "file does not contain result pattern"
		else
			pattern = match[1]
			print "testing #{file}\n"
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
				raise "test failed.\n" +
						"  resulted: #{result}\n" +
						"  expected: #{match[1]}\n" +
						"  To examine the test:\n" +
						"    #{EXECUTABLE} #{file}"
			end
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

errored.each do |i|
	print "#{i}\n"
end

print "\n"
print "Passed #{test_count - errored.size} / Failed #{errored.size} / Total #{test_count}\n"

begin
	File.unlink("#{TEMP_DIR}/stdout.log")
	File.unlink("#{TEMP_DIR}/stderr.log")
rescue
	nil
end
