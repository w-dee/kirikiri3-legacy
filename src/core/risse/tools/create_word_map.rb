#!/usr/bin/env ruby


# 単語ファイルからそれらの単語を切り出すためのC++関数を作成する
$cut_unmatched_word = nil

# ノードの末端の処理
def node_last(level, word, is_default)
	ret = ''

	# インデントレベル
	indent = ' ' * (level+1)

	# 結果文字列を生成
	ret << indent

	if(is_default)
		ret << "ptr += #{level-1};\n"
	else
		ret << "ptr += #{level};\n"
	end

	ret << indent

	if word[:match_word] == 'true' then
		ret << "if(!Risse_iswalnum_nc(p[#{level}])) "
	end

	ret << "{ "

	if word[:value] != '-' then
		ret << "value = #{word[:value]}; "
	end

	ret << "return #{word[:id]}; }\n"

	if word[:is_word] == 'true' && $cut_unmatched_word != nil then
		ret << "#{indent}goto cut_word;\n"
	end

	ret
end

# マップを書き出す
def gen(level, words, history, is_default)

	ret = ''

	# インデントレベル
	indent = ' ' * (level+1)

	# case_ignore な words について最初の一文字を大文字にして複製する
	temp = []
	words.each do |item|
		if item[:case_ignore] == 'true' && item[:token] != '' then
			n = item.clone
			new_token = n[:token].capitalize
			if n[:token] != new_token then
				n[:token] = new_token
				temp << n
			end
		end
	end
	words.concat(temp)


	# words のサイズが 1 でかつ words[0][:token] が空の場合
	if words.size == 1 && words[0][:token] == '' then
		return node_last(level, words[0], is_default)
	end

	# words をトークン順にソート
	words.sort! { |a,b| a[:token] <=> b[:token] }

	# token の最初の一文字でグルーピング
	prev_ch = 'DUMMY'
	group = {}
	subgroup = []
	words.each do |item|
		first_ch = ''
		first_ch = item[:token][0].chr if item[:token] != ''
		if first_ch != prev_ch && prev_ch != 'DUMMY' then
			group[prev_ch] = subgroup
			subgroup = []
		end
		prev_ch = first_ch
		# item の token の最初の文字を削る
		sub_item = item.clone
		if sub_item[:token] != '' then
			token = sub_item[:token]
			sub_item[:token] = token[1, token.length - 1]
		end
		subgroup << sub_item
	end
	group[prev_ch] = subgroup if !subgroup.empty?

	# switch 文を生成
	ret << "#{indent}switch(p[#{level}]) {\n"

	# subgroup ごとに case を作成
	content_map = {}
	group.each do |char, items|
		# 再帰
		content_map[char] = gen(level + 1, items, history + char, char == '' ? true : false)
	end

	# content_map を内容が同じものをまとめる
	content_map_rev = {}
	content_map.each do |key, item|
		if content_map_rev[item] == nil then
			content_map_rev[item] = [key]
		else
			content_map_rev[item] << key
		end
	end

	# 結果文字列の作成
	content_map_rev.keys.sort.each do |item|
		chars = content_map_rev[item]
		chars.each do |char|
			char = '\\\\' if char == '\\'
			char = '\\\'' if char == '\''
			if char != '' then
				ret << "#{indent}case '#{char}':\n"
			else
				ret << "#{indent}default:\n"
			end
		end
		ret << item
	end

	# $cut_unmatched_word のとき
	if $cut_unmatched_word != nil && level == 0 then
		ret << "#{indent}default:\n"
		ret << "#{indent} if(Risse_iswalpha_nc(p[0])) { p++; goto cut_word; }\n"
	end

	# switch 文の終わりを生成
	ret << "#{indent}}\n"

	if $cut_unmatched_word != nil && history =~ /^[A-Za-z_][A-Za-z0-9_]*$/ then
		ret << "#{indent}ptr += #{level};\n"
		ret << "#{indent}goto cut_word;\n"
	end

	ret
end



# ファイルを読み込む

words = []

File.open(ARGV[0]).readlines.each do |line|
	line.gsub!(/^.*?(\#.*)/, '')
	if line =~ /^\s*(.+?)\s+(.+?)\s+(.+?)\s+(.+?)\s+(.+?)\s*$/ then
		token = $1
		id = $2
		value = $3
		match_word = $4
		case_ignore = $5
		is_word = 'false'

		if token == '---*---' then
			$cut_unmatched_word = id
			next
		end

		if case_ignore == 'true' then
			token.downcase!
		end
		if token =~ /^[A-Za-z_][A-Za-z0-9_]*$/ then
			is_word = 'true'
		else
			match_word = 'false' 
		end
		hash = {:token => token, :id => id,
					:value => value,
					:match_word => match_word, :is_word => is_word,
					:case_ignore => case_ignore}
		words << hash
	end
end

# プロローグを書き出す

print <<EOS
static int #{ARGV[1]}(const risse_char * & ptr, tRisseVariant &value)
{
 const risse_char * p = ptr;

EOS

# ルート要素に従って再帰

print gen(0, words, '', false)

# エピローグを書き出す

print <<EOS
 return 0;
EOS

if $cut_unmatched_word != nil then
	print <<-EOS
cut_word:
 while(Risse_iswalnum_nc(*p)) p++;
 ptr = p;
 return #{$cut_unmatched_word};
	EOS
end

print <<EOS
}
EOS

