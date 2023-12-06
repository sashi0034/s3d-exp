# include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{ 0.98, 0.96, 0.94 });
	const Texture texture{ U"🦀"_emoji };

	Ellipse target{ 400, 300, 180, 120 };
	Rect outer = Scene::Rect();
	double minThickness = 3.0, maxThickness = 10.0;
	double lineCount = 150;
	double offsetRange = 60.0;

	SaturatedLinework<Ellipse> linework{ target, outer };
	linework
		.setThickness(minThickness, maxThickness)
		.setLineCount(static_cast<size_t>(lineCount))
		.setOffsetRange(offsetRange);

	while (System::Update())
	{
		if (MouseR.down())
		{
			target.setCenter(Cursor::Pos());
			linework.setTargetShape(target);
		}

		texture.scaled(1.6).drawAt(target.center);

		linework.draw(ColorF{ 0.1 });

		if (SimpleGUI::Slider(U"lineCount", lineCount, 0.0, 400.0, Vec2{ 20, 20 }, 150))
		{
			linework.setLineCount(static_cast<size_t>(lineCount));
		}

		if (SimpleGUI::Slider(U"offsetRange", offsetRange, 0.0, 100.0, Vec2{ 20, 60 }, 150))
		{
			linework.setOffsetRange(offsetRange);
		}
	}
}
