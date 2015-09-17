#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include "Common.h"

#include <vector>
#include <algorithm>

namespace Importer
{
	namespace FBX
	{

		struct PNTVertex
		{
			Math::Vector3 mPosition;
			Math::Vector3 mNormal;
			Math::Vector2 mUV;

			bool operator==(const PNTVertex& rhs) const
			{
				return (mPosition == rhs.mPosition) && (mNormal == rhs.mNormal) && (mUV == rhs.mUV);
			}
		};

		struct VertexBlendingInfo
		{
			unsigned int mBlendingIndex;
			double mBlendingWeight;

			VertexBlendingInfo():
				mBlendingIndex(0),
				mBlendingWeight(0.0)
			{}

			bool operator < (const VertexBlendingInfo& rhs)
			{
				return (mBlendingWeight > rhs.mBlendingWeight);
			}
		};

		struct PNTIWVertex
		{
			Math::Vector3 mPosition;
			Math::Vector3 mNormal;
			Math::Vector2 mUV;
			std::vector<VertexBlendingInfo> mVertexBlendingInfos;

			void SortBlendingInfoByWeight()
			{
				std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
			}

			bool operator==(const PNTIWVertex& rhs) const
			{
				bool sameBlendingInfo = true;

				// We only compare the blending info when there is blending info
				if(!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
				{
					// Each vertex should only have 4 index-weight blending info pairs
					for (unsigned int i = 0; i < 4; ++i)
					{
						if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
							abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
						{
							sameBlendingInfo = false;
							break;
						}
					}
				}

				bool result1 = mPosition == rhs.mPosition;
				bool result2 = mNormal == rhs.mNormal;
				bool result3 = mUV == rhs.mUV;

				return result1 && result2 && result3 && sameBlendingInfo;
			}
		};
	}
}