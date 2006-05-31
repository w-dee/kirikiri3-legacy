class Branch
	attr_accessor :condition
	attr_accessor :target

	def initialize(condition, target)
		self.condition = condition
		self.target = target
	end
end

class Block
	attr_accessor :name
	attr_accessor :lines
	attr_accessor :branches

	def initialize(name)
		self.name = name
		self.lines = []
		self.branches = []
	end
end

blocks = []
block = nil
label_pat = /^\*([.@\w]+)/
branch_pat = /^if\s+([.@\w]+)\s+then\s+\*([.@\w]+)\s+else\s+\*([.@\w]+)/
goto_pat = /^goto\s+\*([.@\w]+)/

STDIN.readlines.each do |line|
	line = line.chomp
	if m = label_pat.match(line) then
		blocks << block if block != nil
		block = Block.new(m[1])
	elsif m = branch_pat.match(line) then
		line = line.gsub(/then/, "then\n")
		line = line.gsub(/else/, "else\n")
		block.branches << Branch.new("#{m[1]} is true", m[2])
		block.branches << Branch.new("#{m[1]} is false", m[3])
		block.lines << line
	elsif m = goto_pat.match(line) then
		block.branches << Branch.new(nil, m[1])
		block.lines << line
	else
		line = line.gsub(/\/\//, "\n")
		block.lines << line if block != nil && !line.empty?
	end
end
blocks << block if block != nil

# define graph type
print <<EOS
digraph g {
graph [
rankdir = "LR"
];
EOS

class String
	def escape
		d = self.dump
		d[1, d.length - 2]
	end
end

# define nodes
blocks.each do |b|
	print "\"#{b.name.escape}\" [ \n"
	print "shape = \"record\"\n"
	print "label = \""
	print "<n> *#{b.name.escape}"
	l = 0
	b.lines.each do |line|
		print "| <l#{l}> #{line.escape}"
		l += 1
	end
	print "\"\n"
	print "];\n"
end

# define edges
id = 0
blocks.each do |b|
	b.branches.each do |branch|
		print "\"#{b.name.escape}\":l#{b.lines.size - 1} -> \"#{branch.target.escape}\":n"
		print " ["
		if branch.condition != nil then
			print " label=\"#{branch.condition.escape}\"" 
		end
		print " id=#{id}"
		id += 1
		print "]"
		print ";\n";
	end
end

print "}\n";

