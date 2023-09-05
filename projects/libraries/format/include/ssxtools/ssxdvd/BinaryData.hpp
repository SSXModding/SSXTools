//! SSX Tricky Binary Data (BD) level structures.
#pragma once
#include <cstring>
#include <ssxtools/BxStringHash.hpp>
#include <ssxtools/BxTypes.hpp>
#include <ssxtools/OffsetPtr.hpp>

// N.B: These structures' helper functions for the most part (unless they do not
// operate on any data outside of the structure) *expect* you have
// created a large file-sized block, or memory mapped them in beforehand. 
//
// If you haven't, crashes are an expectation, not a bug in my code.

namespace ssxtools::core {

	enum eBdFormatType : u8 { Generic, Ps2, Xbox, Ngc };

	/// A patch object.
	struct tBdPatch {
		/// Patch type basically.
		enum Style : u32 {
			Reset,
			SnowStandard,
			OffTrackStandard,
			PowderedSnow,
			SlowPowderedShow,
			IceStandard,
			Bounce,
			IceNoTrail,
			SnowFresh, //?
			Rock,
			Wall,
			IceNoTrailCrunch,
			UnkBoardWakeOnly,
			MetalStandard,
			Speed, //?
			SnowStandardDup,
			SandStandard,
			Door,
			MetalShowoffRamp
		};

		tVec4 lightmapPoint;
		tVec4 uvPoints[4];

		tMat4 bezierControlPoints;

		tVec3 lowestPoint;
		tVec3 highestPoint;

		tVec4 points[4];

		Style style;

		u32 unk2;
		u32 visibility;

		u32 textureId;

		i32 unk4; // -1
		i32 unk5;
		i32 unk6;
	};

	/// The "hash table" object.
	struct tBdHashTable {
		struct Bucket {
			u32 hash;
			u32 objectUid;

			// TODO: It might be a good idea to move this, or try and
			// abstract it better.

			/// Find a object inside of the hash table.
			///
			/// # Safety
			/// It is extremely unsafe to call this function from a bucket
			/// returned by this function. Don't even try.
			constexpr Bucket* Find(std::size_t size, std::string_view name) {
				const auto nameHash = BxStringHash(name);

				// Simple lil b-search. Nothing goes over I'd say
				// 1000 elements, so this should be decent enough.
				for(std::size_t i = 0; i < size; ++i)
					if(this[i].hash == nameHash)
						return &this[i];

				return nullptr;
			}
		};

		u32 hashTableDataSize; // the entire size of the block

		u32 numPatches;
		OffsetPtr<Bucket> patchHash;
		u32 numInstances;
		OffsetPtr<Bucket> instanceHash;
		u32 numModels;
		OffsetPtr<Bucket> modelHash;
		u32 numLights;
		OffsetPtr<Bucket> lightHash;
		u32 numSplines;
		OffsetPtr<Bucket> splineHash;
		u32 numCameras;
		OffsetPtr<Bucket> cameraHash;

		constexpr Bucket* PatchHash() noexcept {
			if(numPatches == 0)
				return nullptr;
			return patchHash(this);
		}

		constexpr Bucket* InstanceHash() noexcept {
			if(numInstances == 0) [[unlikely]]
				return nullptr;
			return instanceHash(this);
		}

		constexpr Bucket* ModelHash() noexcept {
			if(numModels == 0)
				return nullptr;
			return modelHash(this);
		}

		constexpr Bucket* LightHash() noexcept {
			if(numLights == 0) [[unlikely]]
				return nullptr;
			return lightHash(this);
		}

		constexpr Bucket* SplineHash() noexcept {
			if(numSplines == 0)
				return nullptr;
			return splineHash(this);
		}

		constexpr Bucket* CameraHash() noexcept {
			if(numCameras == 0)
				return nullptr;
			return cameraHash(this);
		}
	};

	struct tBdHeader {
		constexpr static u8 VALID_VERSION[3] { 0, 21, 27 };

		u8 version[3];
		eBdFormatType format;

		u32 NumPlayerStarts;
		u32 NumPatches;
		u32 NumInstances;
		u32 NumParticleInstances;
		u32 NumMaterials;
		u32 NumMaterialBlocks;
		u32 NumLights;
		u32 NumSplines;
		u32 NumSplineSegments;
		u32 NumTextureFlips;
		u32 NumModels;
		u32 NumParticleModels;
		u32 NumTextures;
		u32 NumCameras;
		u32 LightMapSize;

		// N.B: These will all become OffsetPtr<> at some point
		// maybe when lily is not lazy

		u32 PlayerStartsOffset;
		OffsetPtr<tBdPatch> patches;
		u32 InstancesOffset;
		u32 ParticleInstancesOffset;
		u32 MaterialsOffset;
		u32 MaterialBlocksOffset;
		u32 LightsOffset;
		u32 SplinesOffset;
		u32 SplineSegmentsOffset;
		u32 TextureFlipsOffset;
		u32 ModelPointersOffset;
		u32 ModelsOffset;
		u32 ParticleModelPointersOffset;
		u32 ParticleModelsOffset;
		u32 CameraPointerOffset;
		u32 CameraOffset;
		OffsetPtr<tBdHashTable> hashTable;

		constexpr tBdPatch* Patches() noexcept {
			return patches(this);
		}

		constexpr tBdHashTable* HashTable() noexcept {
			// The hash table always exists, so we don't need to worry about checking
			// if it does or doesn't
			return hashTable(this);
		}

		template <eBdFormatType Format>
		[[nodiscard]] constexpr bool Valid() const noexcept {
			// This header isn't valid for this file
			if(format != Format)
				return false;

			return !std::memcmp(&version[0], &VALID_VERSION[0], sizeof(version));
		}
	};

} // namespace ssxtools::core
