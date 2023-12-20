#include "stdafx.h"
#include "PsdReader.h"

#include "Psd/Psd.h"

// for convenience reasons, we directly include the platform header from the PSD library.
// we could have just included <Windows.h> as well, but that is unnecessarily big, and triggers lots of warnings.
#include "Psd/PsdPlatform.h"

// in the sample, we use the provided malloc allocator for all memory allocations. likewise, we also use the provided
// native file interface.
// in your code, feel free to use whatever allocator you have lying around.
#include "Psd/PsdMallocAllocator.h"
#include "Psd/PsdNativeFile.h"

#include "Psd/PsdDocument.h"
#include "Psd/PsdColorMode.h"
#include "Psd/PsdLayer.h"
#include "Psd/PsdChannel.h"
#include "Psd/PsdChannelType.h"
#include "Psd/PsdLayerMask.h"
#include "Psd/PsdVectorMask.h"
#include "Psd/PsdLayerMaskSection.h"
#include "Psd/PsdImageDataSection.h"
#include "Psd/PsdImageResourcesSection.h"
#include "Psd/PsdParseDocument.h"
#include "Psd/PsdParseLayerMaskSection.h"
#include "Psd/PsdParseImageDataSection.h"
#include "Psd/PsdParseImageResourcesSection.h"
#include "Psd/PsdLayerCanvasCopy.h"
#include "Psd/PsdInterleave.h"
#include "Psd/PsdPlanarImage.h"
#include "Psd/PsdExport.h"
#include "Psd/PsdExportDocument.h"

#include "PsdTgaExporter.h"
#include "PsdDebug.h"
#include "Psd/PsdLayerType.h"

namespace
{
	using namespace ExPsd;
	using namespace psd;

	constexpr uint32 CHANNEL_NOT_FOUND = UINT_MAX;

	uint32 findChannel(const Layer* layer, int16 channelType)
	{
		for (uint32 i = 0; i < layer->channelCount; ++i)
		{
			const Channel* channel = &layer->channels[i];
			if (channel->data && channel->type == channelType)
				return i;
		}

		return CHANNEL_NOT_FOUND;
	}

	void expandChannelToCanvas(
		const Layer* layer, const Channel* channel, Array<uint8>& canvasData, Size canvasSize)
	{
		imageUtil::CopyLayerData(
			static_cast<uint8*>(channel->data),
			canvasData.data(),
			layer->left, layer->top, layer->right, layer->bottom,
			canvasSize.x, canvasSize.y);
	}

	void applyMask(Rect maskRect, Size imageSize, const uint8_t* mask, uint8_t* dest)
	{
		for (int32 y = 0u; y < imageSize.y; ++y)
		{
			for (int32 x = 0u; x < imageSize.x; ++x)
			{
				const Point maskPoint = Point(x, y) - maskRect.tl();
				const int destIndex = (y * imageSize.x + x) * 4u + 3u;

				if (InRange(maskPoint.x, 0, maskRect.w - 1)
					&& InRange(maskPoint.y, 0, maskRect.h - 1))
				{
					const double maskRate = mask[maskPoint.y * maskRect.w + maskPoint.x] / 255.0;
					dest[destIndex] = static_cast<uint8>(dest[destIndex] * maskRate);
				}
				else
				{
					dest[destIndex] = 0;
				}
			}
		}
	}
}

struct PsdReader::Impl
{
	Array<DynamicTexture> m_textures{};

	int ReadPsd()
	{
		const std::wstring srcPath =
			// L"psd/Sample.psd";
			L"psd/miko15.psd";

		MallocAllocator allocator;
		NativeFile file(&allocator);

		// try opening the file. if it fails, bail out.
		if (!file.OpenRead(srcPath.c_str()))
		{
			PSD_SAMPLE_LOG("Cannot open file.\n");
			return 1;
		}

		// create a new document that can be used for extracting different sections from the PSD.
		// additionally, the document stores information like width, height, bits per pixel, etc.
		Document* document = CreateDocument(&file, &allocator);
		if (!document)
		{
			PSD_SAMPLE_LOG("Cannot create document.\n");
			file.Close();
			return 1;
		}

		// the sample only supports RGB colormode
		if (document->colorMode != colorMode::RGB)
		{
			PSD_SAMPLE_LOG("Document is not in RGB color mode.\n");
			DestroyDocument(document, &allocator);
			file.Close();
			return 1;
		}

		// extract image resources section.
		// this gives access to the ICC profile, EXIF data and XMP metadata.
		{
			ImageResourcesSection* imageResourcesSection = ParseImageResourcesSection(document, &file, &allocator);
			PSD_SAMPLE_LOG("XMP metadata:\n");
			PSD_SAMPLE_LOG(imageResourcesSection->xmpMetadata);
			PSD_SAMPLE_LOG("\n");
			DestroyImageResourcesSection(imageResourcesSection, &allocator);
		}

		Size canvasSize{document->width, document->height};
		std::array<Array<uint8>, 4> canvasData{};
		canvasData.fill(Array<uint8>(canvasSize.x * canvasSize.y));
		Array<Color> colorArray{document->width * document->height};

		// extract all layers and masks.
		bool hasTransparencyMask = false;
		LayerMaskSection* layerMaskSection = ParseLayerMaskSection(document, &file, &allocator);
		if (layerMaskSection)
		{
			hasTransparencyMask = layerMaskSection->hasTransparencyMask;

			// extract all layers one by one. this should be done in parallel for maximum efficiency.
			for (uint32 i = 0; i < layerMaskSection->layerCount; ++i)
			{
				Layer* layer = &layerMaskSection->layers[i];
				ExtractLayer(document, &file, &allocator, layer);

				// 非表示
				if (layer->isVisible == false) continue;

				if (layer->type != layerType::ANY)
				{
					Console.writeln(U"Type: {}"_fmt(layer->type));
				}

				// check availability of R, G, B, and A channels.
				// we need to determine the indices of channels individually, because there is no guarantee that R is the first channel,
				// G is the second, B is the third, and so on.
				const uint32 indexR = findChannel(layer, channelType::R);
				const uint32 indexG = findChannel(layer, channelType::G);
				const uint32 indexB = findChannel(layer, channelType::B);
				const uint32 indexA = findChannel(layer, channelType::TRANSPARENCY_MASK);
				if (indexA == CHANNEL_NOT_FOUND)
				{
					Console.writeln(U"Missing alpha");
					continue;
				}

				if ((indexR != CHANNEL_NOT_FOUND) && (indexG != CHANNEL_NOT_FOUND) && (indexB != CHANNEL_NOT_FOUND))
				{
					// RGB channels were found.
					expandChannelToCanvas(layer, &layer->channels[indexR], canvasData[0], canvasSize);
					expandChannelToCanvas(layer, &layer->channels[indexG], canvasData[1], canvasSize);
					expandChannelToCanvas(layer, &layer->channels[indexB], canvasData[2], canvasSize);

					canvasData[3].fill(0);
					expandChannelToCanvas(layer, &layer->channels[indexA], canvasData[3], canvasSize);
				}

				if (document->bitsPerChannel != 8)
				{
					Console.writeln(U"{}-BPC is not supported."_fmt(document->bitsPerChannel));
					continue;
				}

				imageUtil::InterleaveRGBA(
					canvasData[0].data(), canvasData[1].data(), canvasData[2].data(), canvasData[3].data(),
					reinterpret_cast<uint8_t*>(colorArray.data()),
					canvasSize.x, canvasSize.y);

				// get the layer name.
				// Unicode data is preferred because it is not truncated by Photoshop, but unfortunately it is optional.
				// fall back to the ASCII name in case no Unicode name was found.
				std::wstringstream layerName;
				if (layer->utf16Name)
				{
					//In Windows wchar_t is utf16
					PSD_STATIC_ASSERT(sizeof(wchar_t) == sizeof(uint16));
					layerName << reinterpret_cast<wchar_t*>(layer->utf16Name);
				}
				else
				{
					layerName << layer->name.c_str();
				}
				// at this point, image8, image16 or image32 store either a 8-bit, 16-bit, or 32-bit image, respectively.
				// the image data is stored in interleaved RGB or RGBA, and has the size "document->width*document->height".
				// it is up to you to do whatever you want with the image data. in the sample, we simply write the image to a .TGA file.
				const String layerNameU32 = (Unicode::FromWstring(layerName.str()));
				Print(layerNameU32);

				// RGBA
				Grid<Color> colorData{document->width, document->height, colorArray};
				auto image = Image(colorData);

				// in addition to the layer data, we also want to extract the user and/or vector mask.
				// luckily, this has been handled already by the ExtractLayer() function. we just need to check whether a mask exists.
				if (layer->layerMask)
				{
					const int32 maskW = layer->layerMask->right - layer->layerMask->left;
					const int32 maskH = layer->layerMask->bottom - layer->layerMask->top;
					const Rect maskRect{layer->layerMask->left, layer->layerMask->top, maskW, maskH};

					Print(U"Mask: " + Unicode::FromWstring(layerName.str()));
					const void* maskData = layer->layerMask->data;
					applyMask(
						maskRect,
						{document->width, document->height},
						static_cast<const uint8_t*>(maskData),
						image.dataAsUint8());
				}

				if (layer->vectorMask)
				{
					Print(U"Vector Mask: " + Unicode::FromWstring(layerName.str()));
					Console.writeln(U"Vector mask is not supported.");
				}

				m_textures.push_back(DynamicTexture(image));
			}

			DestroyLayerMaskSection(layerMaskSection, &allocator);
		}

		// don't forget to destroy the document, and close the file.
		DestroyDocument(document, &allocator);
		file.Close();

		return 0;
	}
};

namespace ExPsd
{
	PsdReader::PsdReader() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void PsdReader::ReadPsd()
	{
		p_impl->ReadPsd();
	}

	const Array<DynamicTexture>& PsdReader::Textures() const
	{
		return p_impl->m_textures;
	}
}
