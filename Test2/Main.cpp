# include <Siv3D.hpp> // OpenSiv3D v0.3.0

void Main()
{
	ManagedScript main(U"script.as");

	while (System::Update())
	{
		main.run();
	}
}
