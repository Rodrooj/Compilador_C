program OperadorDesconhecido;
var
    y: integer;
begin
    y := 7;
    if y <> 10 then  { Erro: operador '<>' não reconhecido, o correto seria '=' ou '!=' }
        writeln('y não é igual a 10');
end.
