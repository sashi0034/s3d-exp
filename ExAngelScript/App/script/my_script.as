
void setup()
{
    breakpoint(0, @callback);

    breakpoint(2, function(v){
        fire(v + 100);
    });

    // 本開発では以下のようなイメージにしたい
    // watch_pc(0x8400, function(){
    //    if (get_a() == 0x40){
    //       breakpoint();
    //    }
    // }
    // )
    //
}

void callback(uint32 val)
{
    fire(21 + val);
}
