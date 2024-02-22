# include <Siv3D.hpp>

enum SyntaxColors
{
	Cl_Plain,
	Cl_Comment,
	Cl_Number,
};

enum class ParseState
{
	FirstSlash,
	Number,
};

struct DrewState
{
	Array<Glyph> glyphs;
	size_t readIndex{};
	Vec2 penPos{};
	SyntaxColors colorIndex{};
	ParseState parsing{};
};

void drawGlyph(DrewState& state)
{
	static constexpr std::array colors = {
		Palette::White,
		Palette::Green,
		Palette::Darkorange
	};

	while (state.readIndex < state.glyphs.size())
	{
		auto&& glyph = state.glyphs[state.readIndex];
		state.readIndex++;
		const auto currPos = state.penPos;
		state.penPos.x += glyph.xAdvance;

		switch (glyph.codePoint)
		{
		case U'/':
			if (state.parsing == ParseState::FirstSlash)
			{
				state.colorIndex = Cl_Comment;
			}
			else if (state.colorIndex != Cl_Comment)
			{
				state.parsing = ParseState::FirstSlash;
				drawGlyph(state);
			}
			break;
		case U'0':
		case U'1':
		case U'2':
		case U'3':
		case U'4':
		case U'5':
		case U'6':
		case U'7':
		case U'8':
		case U'9': {
			state.colorIndex = Cl_Number;
			break;
		}
		case U'\t':
			state.penPos.x += 40;
			continue;
		default:
			if (state.colorIndex != Cl_Comment) state.colorIndex = Cl_Plain;
			break;
		}
		glyph.texture.draw(Math::Round(currPos + glyph.getOffset()), colors[state.colorIndex]);
	}
}

void drawCodeStyle(const Font& font, const String& text, const Vec2& basePos)
{
	DrewState s{
		.glyphs = font.getGlyphs(text), .penPos = basePos,
	};
	drawGlyph(s);
}

void Main()
{
	const Font font{30};

	String text;

	constexpr Rect area{50, 50, 700, 300};

	while (System::Update())
	{
		// キーボードからテキストを入力
		TextInput::UpdateText(text);

		// 未変換の文字入力を取得
		const String editingText = TextInput::GetEditingText();

		area.draw(ColorF{0.3});

		drawCodeStyle(font, text + U"|" + editingText, area.tl());
		// font(text + U'|' + editingText).draw(area.stretched(-20));
	}
}
