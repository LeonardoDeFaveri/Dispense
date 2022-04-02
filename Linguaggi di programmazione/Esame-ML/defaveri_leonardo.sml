fun f file_name =
    let
        val file = TextIO.openIn(file_name);
        val text = explode(TextIO.inputAll(file));

        fun take [] = [""]
         | take [x] = [""]
         | take (x::y::zs) = implode([x,y]) :: skip(zs)
        and skip [] = [""]
         | skip [x] = [""]
         | skip (x::y::zs) = take(zs);
    in
        TextIO.closeIn(file);
        skip text
    end;

f "test.txt";