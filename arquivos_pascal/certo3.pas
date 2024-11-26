program Fatorial;
var
    n: integer;
    i: integer;
    fatorial: integer;
begin
    n := 5;
    fatorial := 1;
    for i := 1 to n do
        fatorial := fatorial * i;
    writeln('O fatorial de ' n ' é: ' fatorial);
end.
