#include "stdafx.h"

namespace
{
	struct IllustMotionVsCb
	{
		int divideCount;
		int padding_0x20[3];
		Float4 meshGridOffset[32 * 32];
	};

	size_t getMeshGridIndex(size_t divideCount, int x, int y)
	{
		return x + y * (divideCount + 1);
	}

	void drawMeshFrames(const ConstantBuffer<IllustMotionVsCb>& illustMotionCb)
	{
		const size_t divideCount = illustMotionCb->divideCount;
		for (int x = 0; x < divideCount; ++x)
		{
			for (int y = 0; y < divideCount; ++y)
			{
				auto&& offsets = illustMotionCb->meshGridOffset;
				(void)Triangle(offsets[getMeshGridIndex(divideCount, x, y)].xy(),
				               offsets[getMeshGridIndex(divideCount, x + 1, y)].xy(),
				               offsets[getMeshGridIndex(divideCount, x, y + 1)].xy())
					.drawFrame(0.5, ColorF(1.0, 0.5));
				(void)Triangle(offsets[getMeshGridIndex(divideCount, x + 1, y + 1)].xy(),
				               offsets[getMeshGridIndex(divideCount, x + 1, y)].xy(),
				               offsets[getMeshGridIndex(divideCount, x, y + 1)].xy())
					.drawFrame(0.5, ColorF(1.0, 0.5));
			}
		}
	}

	struct MeshRigidBody
	{
		double angle;
		double angVel;
	};

	struct MeshBodyPoint
	{
		Vec2 currentPoint;
		Vec2 targetPoint;
		double slipPos;
		double slipVel;
	};

	class LineY
	{
	public:
		static constexpr int Height = 40;
		double CurrentLY() const { return 20 + m_y * Height; }

		double NextLY()
		{
			m_y++;
			return CurrentLY();
		}

	private:
		int m_y{-1};
	};

	double g_attenuate = 0.99;

	void fixedUpdateMeshes(
		size_t divideCount,
		const Vec2& drawSize,
		MeshRigidBody& meshRigidBody,
		Array<MeshBodyPoint>& baseMeshGrid)
	{
		// 剛体更新
		constexpr double angleSpeed = 0.2;
		meshRigidBody.angVel +=
			angleSpeed * (Math::Sin(ToRadians(meshRigidBody.angle)) * -1);

		meshRigidBody.angle = (meshRigidBody.angle + meshRigidBody.angVel) * g_attenuate;

		const auto center = Vec2{divideCount / 2.0, 0};
		const Mat3x2 rotationMat =
			Mat3x2::Rotate(ToRadians(meshRigidBody.angle), center * drawSize / divideCount);
		for (int x = 0; x <= divideCount; ++x)
		{
			for (int y = 0; y <= divideCount; ++y)
			{
				// 剛体の点を更新
				auto&& p = baseMeshGrid[getMeshGridIndex(divideCount, x, y)];
				p.targetPoint = rotationMat.transformPoint(Vec2{x, y} * drawSize / divideCount);

				// TODO: プロパティ整理

				if ((Vec2{x, y} - center) == Vec2::Zero()) continue;
				// p.slipVel += angleSpeed * (Math::Sin(ToRadians(meshRigidBody.angle)));
				p.slipPos = -meshRigidBody.angle; //(p.slipPos + p.slipVel) * g_attenuate;

				const double distCoefficient = (Vec2{x, y} - center).lengthSq() / (divideCount * divideCount);

				p.currentPoint = p.targetPoint
					+ Mat3x2::Rotate(ToRadians(meshRigidBody.angle)).transformPoint(Vec2{p.slipPos, 0}) *
					distCoefficient;
			}
		}
	}
}

void Main()
{
	Scene::Resize(1280, 720);
	Window::Resize(1280, 720);

	const Texture mainTexture(Emoji(U"🧸"));
	constexpr size_t divideCount1 = 8;
	constexpr size_t divideCount = divideCount1 - 1;
	constexpr Vec2 drawSize{120, 120};

	const FilePath shaderPath{U"asset/illust_motion.hlsl"};
	const VertexShader vs = HLSL{shaderPath};
	const PixelShader ps = HLSL{shaderPath};
	ConstantBuffer<IllustMotionVsCb> illustMotionCb{};
	illustMotionCb->divideCount = divideCount;

	MeshRigidBody meshRigidBody{};
	Array<MeshBodyPoint> baseMeshGrid{};
	baseMeshGrid.resize(divideCount1 * divideCount1);

	for (int x = 0; x <= divideCount; ++x)
	{
		for (int y = 0; y <= divideCount; ++y)
		{
			const auto index = getMeshGridIndex(divideCount, x, y);
			auto&& p = baseMeshGrid[index];
			p.targetPoint = Vec2{x, y} * drawSize / divideCount;
			p.currentPoint = p.targetPoint;
			p.slipVel = -1.5;
			illustMotionCb->meshGridOffset[index] =
				Float4(p.currentPoint, Vec2{});
		}
	}

	bool isDrawWires{};

	meshRigidBody.angVel = 1.5;

	while (System::Update())
	{
		fixedUpdateMeshes(divideCount, drawSize, meshRigidBody, baseMeshGrid); // FIXME: Fixed

		// CBへ反映
		for (int x = 0; x <= divideCount; ++x)
		{
			for (int y = 0; y <= divideCount; ++y)
			{
				illustMotionCb->meshGridOffset[getMeshGridIndex(divideCount, x, y)] =
					Float4(baseMeshGrid[getMeshGridIndex(divideCount, x, y)].currentPoint, Vec2{});
			}
		}

		// UI描画
		LineY ly{};
		SimpleGUI::CheckBox(isDrawWires, U"Draw wires", {0, ly.NextLY()});

		if (SimpleGUI::Button(U"Ang Vel +1.0", {0, ly.NextLY()}, 200))
		{
			meshRigidBody.angVel += 1.0;
		}
		if (SimpleGUI::Button(U"Ang Vel -1.0", {240, ly.CurrentLY()}, 240))
		{
			meshRigidBody.angVel += -1.1;
		}
		SimpleGUI::Slider(g_attenuate, Vec2{0, ly.NextLY()}, 120);
		SimpleGUI::Headline(U"Attenuate {:.4f}"_fmt(g_attenuate), {160, ly.CurrentLY()});

		ly.NextLY();
		ly.NextLY();

		for (auto&& offsetPoint : step(Size(1, 1)))
		{
			const auto drawBasePoint = Vec2{Scene::Size().x / 2, ly.CurrentLY()};
			constexpr double scale = 3.0;
			Transformer2D transformer2D{
				Mat3x2::Translate((offsetPoint * drawSize).movedBy(drawBasePoint))
				.scaled(scale, Vec2{drawBasePoint})
				.translated(Vec2{-scale * drawSize.x / 2, 0})
			};

			// メッシュ描画
			{
				const ScopedCustomShader2D shader{vs, ps};

				Graphics2D::SetVSConstantBuffer(1, illustMotionCb);
				Graphics2D::SetPSTexture(0, mainTexture);

				Graphics2D::DrawTriangles(divideCount * divideCount * 2);
			}

			if (isDrawWires)
			{
				// ワイヤーフレーム描画
				drawMeshFrames(illustMotionCb);
			}
		}
	}
}
