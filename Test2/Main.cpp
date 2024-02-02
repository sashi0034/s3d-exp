# include <Siv3D.hpp>

void Main()
{
	const Texture cat{U"🐈"_emoji};

	const Texture dog{U"👾"_emoji};

	while (System::Update())
	{
		// 2 倍に拡大して描画
		cat.scaled(2.0).drawAt(200, 300);

		// 1.5 倍に拡大して描画
		cat.scaled(1.5).drawAt(400, 300);

		// 半分のサイズに縮小して描画
		dog.scaled(0.5).drawAt(600, 300);
	}
}
