#!/usr/bin/env ruby

# //! @brief  のような形式のdoxygenアノテーションをJavaDoc形式に変換するスクリプト


prev_is_file = false

last_desc = []
block = []
lws = ''
param_col_max = 0
suppress_next_sep = false

class String
	def cpos
		pos = 0
		self.each_byte do |e|
			if e == 9 # tab code
				pos = pos + 4
				pos = pos & ~3
			else
				pos = pos + 1
			end
		end
		pos
	end

	def filltab(n)
		str = self.dup
		while str.cpos < n
			str << "\t"
		end
		str
	end
end

line_count = 0
File.open(ARGV[0]).readlines.each do |line|
	line_count += 1

	thru = true


	# //! で始まってる行を探す
	if line=~ /^(\s*)\/\/!\s*(.*)/
		ws = $1
		directive = $2

		thru = false

		if directive =~ /^@(\w+)\s+(.*)/
			type = $1
			desc = $2

			if type != 'brief' &&
				type != 'file' &&
				type != 'param' &&
				type != 'note' &&
				type != 'return'
				STDERR.puts "unknown directive #{type} at #{line_count} in #{ARGV[0]}"
			end

			if prev_is_file && type == 'brief'
				thru = true
			else
				last_desc = []
				last_desc << desc.chomp
				block << { :type => type, :desc => last_desc }
				lws = ws
				if type == 'param'
					if desc =~ /^(\w+)/
						param = $1
						cpos = "#{lws} * @param #{param}\t".cpos
						param_col_max = cpos if param_col_max < cpos
					end
				end
			end

			prev_is_file = false
		elsif directive =~ /^@(\w+)/
			type = $1

			thru = true if type == 'file'

			prev_is_file = false
			prev_is_file = true if type == 'file'
		else
			last_desc << directive.chomp
		end
	else
		if block.size != 0
			brief_found = false
			print "#{lws}/**\n"
			# block の中身を出力
			block.each do |item|
				type = item[:type]
				if type == 'brief'
				brief_found = true
					item[:desc].each do |nline|
						print "#{lws} * #{nline}\n"
					end
				elsif type == 'param'
					if item[:desc][0] =~ /^(\w+)\s+(.*)/
						print "#{lws} * @param #{$1}".filltab(param_col_max)
						print "#{$2}\n"
						first = true
						item[:desc].each do |nline|
							if !first
								print "#{lws} *".filltab(param_col_max) 
								print nline
								print "\n"
							end
							first = false
						end
					end
				else
					start = "#{lws} * @#{type}\t"
					col = start.cpos
					first = true
					print start
					item[:desc].each do |nline|
						print "#{lws} *".filltab(col) if !first
						print nline
						print "\n"
						first = false
					end
				end
			end
			print "#{lws} */\n"

			if !brief_found
				STDERR.puts "no brief at #{line_count} in #{ARGV[0]}"
			end

			# //-------- の行を消す
			if line =~ /\s*\/\/-{10,}$/
				thru = false
			end

		end
		block = []
		param_col_max = 0
	end


	if thru
		print line
		last_sep = ''
	end
end

