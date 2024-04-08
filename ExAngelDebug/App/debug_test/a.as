void Test(){
    Array<int> gs12 = {1, 5, 8};

    A a;
    a.u = 2;
    int d = 7;
    Emoji e2222;
    int k1 = 9;
    int kd = 6;
    ClearPrint();

    Print << "Called 'Test' " << d;

    k();

    a.v = 9;

    int e = d + 9;

    Print << "e = " << e;

    Texture texture(Emoji("🧪"));


}

void k(){
    Print << "Called 'k'";

}

class AC{
    int c1;
    int c2;
}

class A{
    int v;
    int k;
    int u;
    AC ac;
}


void Main(){
    Print << "Called 'Main'";

    Texture texture(Emoji("💘"));

    while (System::Update()) {
        texture.drawAt(Scene::Center());
    }
}

//
// - OpenSiv3D 用の補完をするには、https://github.com/sashi0034/angel-lsp/blob/main/examples/OpenSiv3D/as.predefined をワークスペース直下に配置してください。
//
// - 右上の ▶️ ボタンからスクリプトを実行できます。
//
// - ScriptRunner はホットリロードに対応しており、起動後でもスクリプトを変更すると動的に反映されます。
//
// - エントリーポイントは 'Main' (F5) と 'Test' (Shift + F5) の 2 つです。必要に応じて使い分けてください。
//
