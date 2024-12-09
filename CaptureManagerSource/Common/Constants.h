#pragma once

enum {
	VENDOR_NVIDIA = 0x000010de,
	VENDOR_INTEL = 0x00008086,   // Haha!
	VENDOR_AMD = 0x00001002,
	VENDOR_ARM = 0x000013B5,  // Mali
	VENDOR_QUALCOMM = 0x00005143,
	VENDOR_IMGTEC = 0x00001010,  // PowerVR
	VENDOR_MICROSOFT_BASIC_RENDER_DRIVER = 0x00001414,
};

const D3DFORMAT D3DFMT_NV12 = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
const D3DFORMAT D3DFMT_AYUV = (D3DFORMAT)MAKEFOURCC('A', 'Y', 'U', 'V');


#define CleanPoint 1

#define HeaderFlag  1 << 31

#define CM_MediaEvent MEReservedMax + 800

#define CM_SnapMediaEvent CM_MediaEvent + 1