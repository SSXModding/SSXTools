// hexpat for CM2 files.
// not at all complete, but mostly there
// and could probably be used for epic trolling
//
// 2023 lily tsuru <lily@crustywindo.ws>

#pragma eval_depth 1024


// helper
struct cm2Offset<T> {
	u32 firstOffset;

	if(firstOffset != 0xffffffff) {
		T first @ firstOffset;
	}
};

// madness
struct cm2AliVec3 {
    padding[8];
    float x;
    padding[40];
    float y;
    padding[40];
    float z;
    padding[48];
};


// Every cm2 object starts with this
struct cm2ObjectHeader {
	u32 backLinkOffset;
	u32 frontLinkOffset;

	u32 dataOffset; // data offset? unsure what purpose this has.
	u32 nameHash; // common
	char name[0x10];
};

struct cm2Camera : cm2ObjectHeader {
    // TODO: a lot of these are booleans
    // "t" or "f" in the .cam files
    // indicates a boolean. It's awesome!
    // (not really.)

    u32 unk;
    u32 unk2; //pad?
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
    cm2AliVec3 offset;
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

	// unfortunately imhex templates
    // do not really work well for this
    // kind of thing, which is rather unfortunate.
	if(frontLinkOffset != 0xffffffff) {
		cm2Camera next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Camera next @ backLinkOffset;
	}
};

struct cm2Anim : cm2ObjectHeader {
    // These indexes match the values in the "aninames.cam"
    // file. Pretty cool.
    u32 animIndex[18];

	if(frontLinkOffset != 0xffffffff) {
		cm2Anim next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Anim next @ backLinkOffset;
	}
};

struct cm2Director : cm2ObjectHeader {
    // no idea how to parse this hellscape
    // this is where scripting hell is too

	if(frontLinkOffset != 0xffffffff) {
		cm2Director next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Director next @ backLinkOffset;
	}
};

struct cm2Path : cm2ObjectHeader {
    char cameraStringPool[300];
    u32 pathCount;
    float speed; // could also be speed?

	if(frontLinkOffset != 0xffffffff) {
		cm2Path next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Path next @ backLinkOffset;
	}
};

struct cm2Region : cm2ObjectHeader {
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
    cm2AliVec3 offset;
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
    u16 unk34; //pad
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

	if(frontLinkOffset != 0xffffffff) {
		cm2Region next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Region next @ backLinkOffset;
	}
};

// an actor inside of a scene.
struct cm2SceneActor {
    char animationName[16];
    u32 unk1;
    u32 unk2;
    u32 unk3;

    float xOff;
    float yOff;
    float zOff;
    float heading; // unsure what this is
} [[single_color]];

struct cm2Scene : cm2ObjectHeader {
    cm2SceneActor actor[8];

	if(frontLinkOffset != 0xffffffff) {
		cm2Scene next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Scene next @ backLinkOffset;
	}
};

struct cm2Folder : cm2ObjectHeader {
    // note: dataOff in folders is repurposed
    // to instead be the first object in the folder
    // (maybe the offsets depend on folder stuff, idk)

	if(frontLinkOffset != 0xffffffff) {
		cm2Folder next @ frontLinkOffset;
	} else if(backLinkOffset != 0xffffffff) {
    	cm2Folder next @ backLinkOffset;
	}
};


struct cm2Header {
    // These start the offset chain
    cm2Offset<cm2Camera> cameras @ 0x0;
    cm2Offset<cm2Anim> anims @ 0x4;
    cm2Offset<cm2Director> directors @ 0x8;
    cm2Offset<cm2Path> paths @ 0xc;
    cm2Offset<cm2Region> regions @ 0x10;
    cm2Offset<cm2Scene> scenes @ 0x14;
    cm2Offset<cm2Folder> folders @ 0x18;
};

cm2Header cm2 @ 0x0;
