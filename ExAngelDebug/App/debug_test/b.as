void Test(){
    Print << "Called 'Test'";

    Texture texture(Emoji("🧪"));

    while (System::Update()) {
        texture.drawAt(Scene::Center());
    }
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
