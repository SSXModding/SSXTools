#include <ssxtools/BxTypes.hpp>
#include <ssxtools/OffsetPtr.hpp>

namespace ssxtools::ssxog {

	/// hash function used for cm2 node names
	constexpr u32 CM2_HashString(const char* string, i32 stringLength) {
		i32 hashVal = 0;
		u32 hashShifted = 0;
		char currChar = *string;
		i32 length = -1;
		i64 hashTotal = 0;

		if(stringLength != 0)
			length = stringLength;

		if(currChar != 0) {
			while(length != 0) {
				string++;

				// perform the hash function
				hashTotal = (hashTotal << 23 ^ hashShifted) ^ currChar;
				hashVal = hashTotal;

				// grab next string char
				currChar = *string;
				length--;

				if(currChar == 0)
					return hashVal;

				hashShifted = hashVal >> 7;
			}
		}

		return hashVal;
	}

	constexpr u32 CM2_HashString(std::string_view sv) {
		return CM2_HashString(sv.data(), sv.length());
	}

	/// common header all cm2 objects start with
	template <class Inner>
	struct [[gnu::packed]] CM2ObjectHeader {
		core::OffsetPtr<Inner> backLink;
		core::OffsetPtr<Inner> frntLink;

		core::OffsetPtr<u32> dataOffset;
		u32 nameHash;
		char name[16];
	};

	struct CM2Camera : CM2ObjectHeader<CM2Camera> {
		u32 unk;
		u32 unk2; // pad?
		u32 unk3;
		u32 unk4;
		u32 unk5;
		float predampSeek;
		u32 unk7;
		u32 unk8;
		u32 unk9;
		float unk10;
		u32 unk11;
		u32 unk12;
		core::tVec3_Aligned offset;
		u32 unk13;
		float poleModeLength;
		u32 zNear;
		u32 unk16;
		u32 unk17;
		u32 unk18;
		u32 unk19;
		u32 unk20;
		u32 unk21;
		u32 unk22;
		u32 unk23;
		u32 unk24;
		u32 unk25;
		u32 unk26;
		float poleModePitchOffset;
		u32 unk27;
		u32 unk28;
		float poleModePitchMin;
		float poleModePitchMax;
		float poleModeDownPitchDiv;
		float poleModeUpPitchDiv;
		u32 unk29;
		u32 unk30;
		u32 unk31;
		u32 unk32;
		u32 unk33;
		u32 unk34;
		float offsetYaw;
		float offsetPitch;
		float offsetRoll;
		float offsetZoom;
		float dampen;
		float terrainRoll;
		u32 terrainRollOnOff;

		u32 boneBox; //?
		u32 boneRef;
		u32 zoomRelBox; //?
		float zoomRelDist;
		u32 negAnglesBox;
		u32 unk35; //?
		float speedLen;
		u32 unk36;
		u32 unk37;
	};

	struct CM2Anim : CM2ObjectHeader<CM2Anim> {
		u32 animIndex[18];
	};

	struct CM2Director : CM2ObjectHeader<CM2Director> {
		// TODO
	};

	struct CM2Path : CM2ObjectHeader<CM2Path> {
		// string pool
		char cameraStringPool[300];
		u32 pathCount;
		float speed; // could also be speed?
	};

	struct CM2Region : CM2ObjectHeader<CM2Region> {
		u32 unk1;
		u32 unk2;
		u32 unk3;
		u32 unk4;
		u32 unk5;
		u32 unk6;
		u32 unk7;
		u32 unk8;
		u32 unk9;
		u32 unk11;
		u32 unk12;
		u32 unk13;
		core::tVec3_Aligned offset;
		u32 unk14;
		u32 unk15;
		u32 unk16;
		u32 unk17;
		u32 unk18;
		u32 unk19;
		u32 unk20;
		u32 unk21;
		u32 unk22;
		u32 unk23;
		u32 unk24;
		u32 unk25;
		u32 unk26;
		u32 unk27;
		u32 unk28;
		u32 unk29;
		u32 unk30;
		u32 unk31;
		u32 unk32;
		float poleModeDownPitchDiv;
		float poleModeUpPitchDiv;
		u32 unk33;
		char enterScript[15];
		char insideScript[15];
		char exitScript[16];
		u16 unk34; // pad
		float radius;

		u32 unk35;
		u32 unk36;
		u32 unk37;
		u32 unk38;
		u32 unk39;
		u32 unk40;
		u32 unk41;
		u32 unk42;
		u32 unk43;
		float len;
		float yaw;
		float roll;

		u32 unk44;
		float airDist;
		u32 unk45;
	};

	/// an actor inside of a scene.
	struct CM2SceneActor {
		char animationName[16];
		u32 unk1;
		u32 unk2;
		u32 unk3;

		float xOff;
		float yOff;
		float zOff;
		float heading; // unsure what this is
	};

	struct CM2Scene : CM2ObjectHeader<CM2Scene> {
		CM2SceneActor actors[8];
	};

	struct CM2Folder : CM2ObjectHeader<CM2Folder> {
		// TODO: accessors
		template <class Object>
		core::OffsetPtr<Object> GetFirst() {
			return dataOffset.PtrCast<Object>();
		}
	};

	struct [[gnu::packed]] CM2FileHeader {
		core::OffsetPtr<CM2Camera> cameras;
		core::OffsetPtr<CM2Anim> anims;
		core::OffsetPtr<CM2Director> directors;
		core::OffsetPtr<CM2Path> paths;
		core::OffsetPtr<CM2Region> regions;
		core::OffsetPtr<CM2Scene> scenes;
		core::OffsetPtr<CM2Folder> folders;
	};

} // namespace ssxtools::ssxog
