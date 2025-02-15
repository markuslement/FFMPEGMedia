// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "IMediaTextureSample.h"
#include "MediaObjectPool.h"
#include "MediaSampleQueue.h"
#include "Math/IntPoint.h"
#include "Misc/Timespan.h"


/**
 * Texture sample generated by FFMPEGMedia player.
 */
class FFFMPEGMediaTextureSample
	: public IMediaTextureSample
	  , public IMediaPoolable
{
public:
	/** Default constructor. */
	FFFMPEGMediaTextureSample()
		: Dim(FIntPoint::ZeroValue)
		  , Duration(FTimespan::Zero())
		  , OutputDim(FIntPoint::ZeroValue)
		  , SampleFormat(EMediaTextureSampleFormat::Undefined)
		  , Stride(0)
		  , Time(FTimespan::Zero())
	{
	}

	/** Virtual destructor. */
	virtual ~FFFMPEGMediaTextureSample()
	{
	}

public:
	/**
	 * Initialize the sample.
	 *
	 * @param InBuffer The sample's data buffer.
	 * @param InSize Size of the buffer.
	 * @param InDim The sample buffer's width and height (in pixels).
	 * @param InOutputDim The sample's output width and height (in pixels).
	 * @param InSampleFormat The sample format.
	 * @param InStride Number of bytes per pixel row.
	 * @param InTime The sample time (relative to presentation clock).
	 * @param InDuration The duration for which the sample is valid.
	 */
	bool Initialize(
		const void* InBuffer,
		uint32 InSize,
		const FIntPoint& InDim,
		uint32 InStride,
		FTimespan InTime,
		FTimespan InDuration)
	{
		if ((InBuffer == nullptr) || (InSize == 0) || (InStride == 0))
		{
			return false;
		}

		if ((InStride * InDim.Y) > InSize)
		{
			return false;
		}


		Buffer.Reset(InSize);
		Buffer.Append((uint8*)InBuffer, InSize);
		Duration = InDuration;
		Dim = InDim;
		SampleFormat = EMediaTextureSampleFormat::CharBGRA;
		Stride = InStride;
		Time.Time = InTime;

		return true;
	}

public:
	//~ IMediaTextureSample interface

	virtual const void* GetBuffer() override
	{
		return Buffer.GetData();
	}

	virtual FIntPoint GetDim() const override
	{
		return Dim;
	}

	virtual FTimespan GetDuration() const override
	{
		return Duration;
	}

	virtual EMediaTextureSampleFormat GetFormat() const override
	{
		return SampleFormat;
	}

	virtual FIntPoint GetOutputDim() const override
	{
		return Dim;
	}

	virtual uint32 GetStride() const override
	{
		return Stride;
	}

#if WITH_ENGINE
	virtual FRHITexture* GetTexture() const override
	{
		return nullptr;
	}
#endif //WITH_ENGINE

	virtual FMediaTimeStamp GetTime() const override
	{
		return Time;
	}

	virtual bool IsCacheable() const override
	{
		return true;
	}

	virtual bool IsOutputSrgb() const override
	{
		return true;
	}

private:
	/** The sample's data buffer. */
	TArray<uint8> Buffer;

	/** Width and height of the texture sample. */
	FIntPoint Dim;

	/** Duration for which the sample is valid. */
	FTimespan Duration;

	/** Width and height of the output. */
	FIntPoint OutputDim;

	/** The sample format. */
	EMediaTextureSampleFormat SampleFormat;

	/** Number of bytes per pixel row. */
	uint32 Stride;

	/** Presentation for which the sample was generated. */
	FMediaTimeStamp Time;
};


/** Implements a pool for WMF texture samples. */
class FFFMPEGMediaTextureSamplePool : public TMediaObjectPool<FFFMPEGMediaTextureSample>
{
};
