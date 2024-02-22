
void setup()
{
    breakpoint(10, @callback);
}

void callback(uint32 val)
{
    fire(21 + val);
}
