﻿#include "stdafx.h"
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

namespace
{
	using namespace ExPsd;
	using namespace psd;
	using Allocator = psd::Allocator;

	constexpr unsigned int CHANNEL_NOT_FOUND = UINT_MAX;

	unsigned int FindChannel(Layer* layer, int16_t channelType)
	{
		for (unsigned int i = 0; i < layer->channelCount; ++i)
		{
			Channel* channel = &layer->channels[i];
			if (channel->data && channel->type == channelType)
				return i;
		}

		return CHANNEL_NOT_FOUND;
	}

	template <typename T, typename DataHolder>
	static void* ExpandChannelToCanvas(Allocator* allocator, const DataHolder* layer, const void* data,
	                                   unsigned int canvasWidth, unsigned int canvasHeight)
	{
		T* canvasData = static_cast<T*>(allocator->Allocate(sizeof(T) * canvasWidth * canvasHeight, 16u));
		memset(canvasData, 0u, sizeof(T) * canvasWidth * canvasHeight);

		imageUtil::CopyLayerData(static_cast<const T*>(data), canvasData, layer->left, layer->top, layer->right,
		                         layer->bottom, canvasWidth, canvasHeight);

		return canvasData;
	}

	static void* ExpandChannelToCanvas(const Document* document, Allocator* allocator, Layer* layer, Channel* channel)
	{
		if (document->bitsPerChannel == 8)
			return ExpandChannelToCanvas<uint8_t>(allocator, layer, channel->data, document->width, document->height);
		else if (document->bitsPerChannel == 16)
			return ExpandChannelToCanvas<uint16_t>(allocator, layer, channel->data, document->width, document->height);
		else if (document->bitsPerChannel == 32)
			return ExpandChannelToCanvas<float32_t>(allocator, layer, channel->data, document->width, document->height);

		return nullptr;
	}

	template <typename T>
	T* CreateInterleavedImage(Allocator* allocator, const void* srcR, const void* srcG, const void* srcB,
	                          unsigned int width, unsigned int height)
	{
		T* image = static_cast<T*>(allocator->Allocate(width * height * 4u * sizeof(T), 16u));

		const T* r = static_cast<const T*>(srcR);
		const T* g = static_cast<const T*>(srcG);
		const T* b = static_cast<const T*>(srcB);
		imageUtil::InterleaveRGB(r, g, b, T(0), image, width, height);

		return image;
	}

	template <typename T>
	T* CreateInterleavedImage(Allocator* allocator, const void* srcR, const void* srcG, const void* srcB,
	                          const void* srcA, unsigned int width, unsigned int height)
	{
		T* image = static_cast<T*>(allocator->Allocate(width * height * 4u * sizeof(T), 16u));

		const T* r = static_cast<const T*>(srcR);
		const T* g = static_cast<const T*>(srcG);
		const T* b = static_cast<const T*>(srcB);
		const T* a = static_cast<const T*>(srcA);
		imageUtil::InterleaveRGBA(r, g, b, a, image, width, height);

		return image;
	}

	void sveRGBA(uint32 width, uint32 height, const uint8_t* src, uint8_t* dest)
	{
		for (unsigned int i = 0u; i < height; ++i)
		{
			for (unsigned int j = 0u; j < width; ++j)
			{
				const uint8_t r = src[(i * width + j) * 4u + 0u];
				const uint8_t g = src[(i * width + j) * 4u + 1u];
				const uint8_t b = src[(i * width + j) * 4u + 2u];
				const uint8_t a = src[(i * width + j) * 4u + 3u];

				dest[(i * width + j) * 4u + 2u] = r;
				dest[(i * width + j) * 4u + 1u] = g;
				dest[(i * width + j) * 4u + 0u] = b;
				dest[(i * width + j) * 4u + 3u] = a;
			}
		}
	}
}

struct PsdReader::Impl
{
	Array<DynamicTexture> m_textures{};

	int ReadPsd()
	{
		const std::wstring srcPath = L"psd/Sample.psd";

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

		// extract all layers and masks.
		bool hasTransparencyMask = false;
		LayerMaskSection* layerMaskSection = ParseLayerMaskSection(document, &file, &allocator);
		if (layerMaskSection)
		{
			hasTransparencyMask = layerMaskSection->hasTransparencyMask;

			// extract all layers one by one. this should be done in parallel for maximum efficiency.
			for (unsigned int i = 0; i < layerMaskSection->layerCount; ++i)
			{
				Layer* layer = &layerMaskSection->layers[i];
				ExtractLayer(document, &file, &allocator, layer);

				// check availability of R, G, B, and A channels.
				// we need to determine the indices of channels individually, because there is no guarantee that R is the first channel,
				// G is the second, B is the third, and so on.
				const unsigned int indexR = FindChannel(layer, channelType::R);
				const unsigned int indexG = FindChannel(layer, channelType::G);
				const unsigned int indexB = FindChannel(layer, channelType::B);
				const unsigned int indexA = FindChannel(layer, channelType::TRANSPARENCY_MASK);

				// note that channel data is only as big as the layer it belongs to, e.g. it can be smaller or bigger than the canvas,
				// depending on where it is positioned. therefore, we use the provided utility functions to expand/shrink the channel data
				// to the canvas size. of course, you can work with the channel data directly if you need to.
				void* canvasData[4] = {};
				unsigned int channelCount = 0u;
				if ((indexR != CHANNEL_NOT_FOUND) && (indexG != CHANNEL_NOT_FOUND) && (indexB != CHANNEL_NOT_FOUND))
				{
					// RGB channels were found.
					canvasData[0] = ExpandChannelToCanvas(document, &allocator, layer, &layer->channels[indexR]);
					canvasData[1] = ExpandChannelToCanvas(document, &allocator, layer, &layer->channels[indexG]);
					canvasData[2] = ExpandChannelToCanvas(document, &allocator, layer, &layer->channels[indexB]);
					channelCount = 3u;

					if (indexA != CHANNEL_NOT_FOUND)
					{
						// A channel was also found.
						canvasData[3] = ExpandChannelToCanvas(document, &allocator, layer, &layer->channels[indexA]);
						channelCount = 4u;
					}
				}

				// interleave the different pieces of planar canvas data into one RGB or RGBA image, depending on what channels
				// we found, and what color mode the document is stored in.
				uint8_t* image8 = nullptr;
				uint16_t* image16 = nullptr;
				float32_t* image32 = nullptr;
				if (channelCount == 3u)
				{
					if (document->bitsPerChannel == 8)
					{
						image8 = CreateInterleavedImage<uint8_t>(
							&allocator, canvasData[0], canvasData[1],
							canvasData[2], document->width, document->height);
					}
					else if (document->bitsPerChannel == 16)
					{
						image16 = CreateInterleavedImage<uint16_t>
						(&allocator, canvasData[0], canvasData[1],
						 canvasData[2], document->width, document->height);
					}
					else if (document->bitsPerChannel == 32)
					{
						image32 = CreateInterleavedImage<float32_t>(
							&allocator, canvasData[0], canvasData[1],
							canvasData[2], document->width, document->height);
					}
				}
				else if (channelCount == 4u)
				{
					if (document->bitsPerChannel == 8)
					{
						image8 = CreateInterleavedImage<uint8_t>(&allocator, canvasData[0], canvasData[1],
						                                         canvasData[2], canvasData[3], document->width,
						                                         document->height);
					}
					else if (document->bitsPerChannel == 16)
					{
						image16 = CreateInterleavedImage<uint16_t>(&allocator, canvasData[0], canvasData[1],
						                                           canvasData[2], canvasData[3], document->width,
						                                           document->height);
					}
					else if (document->bitsPerChannel == 32)
					{
						image32 = CreateInterleavedImage<float32_t>(&allocator, canvasData[0], canvasData[1],
						                                            canvasData[2], canvasData[3], document->width,
						                                            document->height);
					}
				}

				allocator.Free(canvasData[0]);
				allocator.Free(canvasData[1]);
				allocator.Free(canvasData[2]);
				allocator.Free(canvasData[3]);

				// get the layer name.
				// Unicode data is preferred because it is not truncated by Photoshop, but unfortunately it is optional.
				// fall back to the ASCII name in case no Unicode name was found.
				std::wstringstream layerName;
				if (layer->utf16Name)
				{
					//In Windows wchar_t is utf16
					PSD_STATIC_ASSERT(sizeof(wchar_t) == sizeof(uint16_t));
					layerName << reinterpret_cast<wchar_t*>(layer->utf16Name);
				}
				else
				{
					layerName << layer->name.c_str();
				}

				// at this point, image8, image16 or image32 store either a 8-bit, 16-bit, or 32-bit image, respectively.
				// the image data is stored in interleaved RGB or RGBA, and has the size "document->width*document->height".
				// it is up to you to do whatever you want with the image data. in the sample, we simply write the image to a .TGA file.
				if (channelCount == 3u)
				{
					if (document->bitsPerChannel == 8u)
					{
						// RGB
						Print(Unicode::FromWstring(layerName.str()));
					}
				}
				else if (channelCount == 4u)
				{
					if (document->bitsPerChannel == 8u)
					{
						// RGBA
						auto a = image8[0];
						Print(Unicode::FromWstring(layerName.str()));

						s3d::Image image{document->width, document->height};
						sveRGBA(document->width, document->height, image8, image.dataAsUint8());
						m_textures.push_back(DynamicTexture(image));
					}
				}

				allocator.Free(image8);
				allocator.Free(image16);
				allocator.Free(image32);

				// in addition to the layer data, we also want to extract the user and/or vector mask.
				// luckily, this has been handled already by the ExtractLayer() function. we just need to check whether a mask exists.
				if (layer->layerMask)
				{
					Print(U"Mask: " + Unicode::FromWstring(layerName.str()));
				}

				if (layer->vectorMask)
				{
					Print(U"Vector Mask: " + Unicode::FromWstring(layerName.str()));
				}
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