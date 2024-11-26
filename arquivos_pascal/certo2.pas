program ParOuImpar;
var
    num: integer;
begin
    num := 15;
    if (num mod 2 = 0) then
        writeln(num ' é par')
    else
        writeln(num ' é ímpar');
end.
