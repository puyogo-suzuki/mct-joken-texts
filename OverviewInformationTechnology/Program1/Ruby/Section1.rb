def parse(str)
    ret = /^.(\d+),(\d+)$/.match(str)
    if ret == nil then
	ret = /^.(\d+)$/.match(str)
	if ret == nil then
            return nil
        else
            return [ret.captures[0], (ret.captures[0].to_i + 1).to_s]
        end
    else
        return ret.captures
    end
end

def openFile(path)
    fp = nil
    begin
        fp = File.open(path, "r")
    rescue => e
        puts ("Error:Cannnot read " + path + ".")
        puts e
        return nil
    end
    ret = ""
    fp.read(nil, ret)
    fp.close
    return ret
end

def writeFile(str, path)
    fp = nil
    begin
        fp = File.open(path, "w")
    rescue => e
        puts ("Error:Cannnot open " + path + ".")
        puts e
        return nil
    end
    begin
        fp.write str
    rescue => e
        puts ("Error:Cannnot write " + path + ".")
        puts e
        return nil
    end
    fp.close
    return true
end

def printLines(str, start, e) # e is end
    lfCount = 0
    str.chars.each do |c|
        if lfCount >= start then
            putc c
        end
        if c == "\n" then
            lfCount = lfCount + 1
        end
        if lfCount >= e then
            return
        end
    end
end

def appendLines(str, start, e) # e is end
    lfCount = 0
    i = 0
    (0...str.length).each do |i|
        if str[i] == "\n" then
            lfCount = lfCount + 1
        end
        if lfCount == (start+1)
            break
        end
    end
    while true
        line = $stdin.gets
        if line == nil then
            return
        end
        if line[0] == "." then
            return
        end
        str.insert(i, line)
        i = i + line.length
    end
end

def removeLines(str, start, e) # e is end
    lfCount = 0
    st_pos = 0
    en_pos = 0
    i = 0
    (0...str.length).each do |i|
        if str[i] == "\n" then
            lfCount = lfCount + 1
            if start == lfCount then
                st_pos = i + 1
            end
            if e == lfCount then
                en_pos = i + 1
            end
        end
    end
    if en_pos == 0 then
        return str[0...st_pos]
    else
        return str[0...st_pos] + str[en_pos...str.length]
    end
end

# Main Function
if ARGV.length == 0 then
    puts "Usage: <file name>."
    exit 1
end
text = openFile(ARGV[0]) 
if text == nil then
    exit 1
end

while true
    print "> "
    tmp = $stdin.gets()
    if tmp[0] == "\n" then
        next
    end
    parsed = parse(tmp)
    if parsed == nil then
        case tmp[0]
        when "w" then
            writeFile(text, ARGV[0])
        when "q" then
            exit 0
        end
    else
        case tmp[0]
        when "p" then
            printLines(text, parsed[0].to_i, parsed[1].to_i)
        when "a" then
            appendLines(text, parsed[0].to_i, parsed[1].to_i)
        when "r" then
            text = removeLines(text, parsed[0].to_i, parsed[1].to_i)
        end
    end
end