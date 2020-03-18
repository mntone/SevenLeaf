#ifndef _SNLF_GRAPHICS_DEFINES_H
#define _SNLF_GRAPHICS_DEFINES_H

#include "SnlfCore.h"

// ---
// Apsect ratio
// ---
typedef enum {
  SNLF_AR_UNSPECIFIED =  0,
  SNLF_AR_SQUARE      =  1,
  SNLF_AR_12_TO_11    =  2,
  SNLF_AR_10_TO_11    =  3,
  SNLF_AR_16_TO_11    =  4,
  SNLF_AR_40_TO_33    =  5,
  SNLF_AR_24_TO_11    =  6,
  SNLF_AR_20_TO_11    =  7,
  SNLF_AR_32_TO_11    =  8,
  SNLF_AR_80_TO_33    =  9,
  SNLF_AR_18_TO_11    = 10,
  SNLF_AR_15_TO_11    = 11,
  SNLF_AR_64_TO_33    = 12,
  SNLF_AR_160_TO_99   = 13,
  SNLF_AR_4_TO_3      = 14,
  SNLF_AR_3_TO_2      = 15,
  SNLF_AR_2_TO_1      = 16,
  SNLF_AR_EXTENDED    = 255,
} SnlfSampleAspectRatioType;

typedef struct {
  uint16_t width, height;
} SnlfSampleAspectRatio;

SNLF_EXPORT void SnlfGetSampleAspectRatio(SnlfSampleAspectRatioType type, SnlfSampleAspectRatio *sampleAspectRatio);

// ---
// Color primaries
// ---
// Output support: BT.709 (HD), BT.601 (SD), BT.2020 (UHD), DCI-P3, DCI-P3 D65
typedef enum {
  SNLF_CP_AUTO,
  SNLF_CP_BT709          =  1, // BT.709-6 / IEC 61966-2-1 sRGB or sYCC / IEC 61966-2-4
  SNLF_CP_UNSPECIFIED    =  2,
  //SNLF_CP_RESERVED     =  3,
  SNLF_CP_NTSC           =  4, // BT.470-6 M
  SNLF_CP_BT470BG        =  5, // BT.470-6 B, G
  SNLF_CP_BT601          =  6, // BT.601-7 525 / BT.1700-0 NTSC / SMPTE 170M
  SNLF_CP_SMPTE240M      =  7, // SMPTE 240M
  SNLF_CP_FILM           =  8,
  SNLF_CP_BT2020         =  9, // BT.2020-2 / BT.2100-2
  SNLF_CP_CIE1931XYZ     = 10, // SMPTE ST 428-1 / CIE 1931 XYZ
  SNLF_CP_DCIP3          = 11, // SMPTE ST 431-2 / SMPTE ST 2113
  SNLF_CP_DCIP3_D65      = 12, // SMPTE ST 422-1 / DCI-P3 D65 / Display P3
  SNLF_CP_JEDEC_P22      = 22, // EBU Tech. 3213-E
  
  SNLF_CP_CUSTOM         = 255,
  SNLF_CP_NTSC_FCC       = (SNLF_CP_CUSTOM + 1),     // NTSC D65 -> C
  SNLF_CP_NTSC_J         = (SNLF_CP_NTSC_FCC + 1),   // NTSC D65 -> D93
  SNLF_CP_DCIP3_D60      = (SNLF_CP_NTSC_J + 1),     // DCI-P3 D60
  SNLF_CP_DCIP3_PLUS     = (SNLF_CP_DCIP3_D60 + 1),  // DCI-P3+
  SNLF_CP_ACES2065       = (SNLF_CP_DCIP3_PLUS + 1), // ACES 2065-1
  SNLF_CP_ADOBE_RGB      = (SNLF_CP_ACES2065 + 1),
  SNLF_CP_PRO_PHOTO      = (SNLF_CP_ADOBE_RGB + 1),
  SNLF_CP_WIDE_GAMUT_RGB = (SNLF_CP_PRO_PHOTO + 1),
} SnlfColorPrimariesType;

typedef struct {
  float x, y;
} SnlfColorPrimary;

typedef struct {
  SnlfColorPrimary redPrimary;
  SnlfColorPrimary greenPrimary;
  SnlfColorPrimary bluePrimary;
  SnlfColorPrimary whitePoint;
} SnlfColorPrimaries;

SNLF_EXPORT void SnlfGetColorPrimaries(SnlfColorPrimariesType type, SnlfColorPrimaries *colorPrimaries);

struct _SnlfColorPrimariesList {
  SnlfColorPrimariesType type;
  SnlfColorPrimaries descriptor;
} snlfColorPrimariesList = {
  { SNLF_CP_BT709,           {{ 0.640   , 0.330    }, { 0.300   , 0.600    }, { 0.150   ,  0.060    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_NTSC,            {{ 0.67    , 0.33     }, { 0.21    , 0.71     }, { 0.14    ,  0.08     }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_BT470BG,         {{ 0.64    , 0.33     }, { 0.29    , 0.60     }, { 0.15    ,  0.06     }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_BT601,           {{ 0.630   , 0.340    }, { 0.310   , 0.595    }, { 0.155   ,  0.070    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_SMPTE240M,       {{ 0.630   , 0.340    }, { 0.310   , 0.595    }, { 0.155   ,  0.070    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_FILM,            {{ 0.681   , 0.319    }, { 0.243   , 0.692    }, { 0.145   ,  0.049    }, { 0.310   , 0.316    }}},
  { SNLF_CP_BT2020,          {{ 0.708   , 0.292    }, { 0.170   , 0.797    }, { 0.131   ,  0.046    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_CIE1931XYZ,      {{ 1.0     , 0.0      }, { 0.0     , 1.0      }, { 0.0     ,  0.0      }, { 1.0 / 3 , 1.0 / 3  }}},
  { SNLF_CP_DCIP3,           {{ 0.680   , 0.320    }, { 0.265   , 0.690    }, { 0.150   ,  0.060    }, { 0.314   , 0.351    }}},
  { SNLF_CP_DCIP3_D65,       {{ 0.680   , 0.320    }, { 0.265   , 0.690    }, { 0.150   ,  0.060    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_JEDEC_P22,       {{ 0.630   , 0.340    }, { 0.295   , 0.605    }, { 0.155   ,  0.077    }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_NTSC_FCC,        {{ 0.67    , 0.33     }, { 0.21    , 0.71     }, { 0.14    ,  0.08     }, { 0.310   , 0.316    }}},
  { SNLF_CP_NTSC_J,          {{ 0.67    , 0.33     }, { 0.21    , 0.71     }, { 0.14    ,  0.08     }, { 0.2831  , 0.2970   }}},
  { SNLF_CP_DCIP3_D60,       {{ 0.680   , 0.320    }, { 0.265   , 0.690    }, { 0.150   ,  0.060    }, { 0.3217  , 0.3378   }}},
  { SNLF_CP_DCIP3_PLUS,      {{ 0.740   , 0.270    }, { 0.220   , 0.780    }, { 0.090   , -0.090    }, { 0.314   , 0.351    }}},
  { SNLF_CP_ACES2065,        {{ 0.680   , 0.320    }, { 0.265   , 0.690    }, { 0.150   ,  0.060    }, { 0.32168 , 0.33767  }}},
  { SNLF_CP_ADOBE_RGB,       {{ 0.6400  , 0.3300   }, { 0.2100  , 0.7100   }, { 0.1500  ,  0.0600   }, { 0.3127  , 0.3290   }}},
  { SNLF_CP_PRO_PHOTO,       {{ 0.734699, 0.265301 }, { 0.159597, 0.840403 }, { 0.036598,  0.000105 }, { 0.345704, 0.358540 }}},
  { SNLF_CP_WIDE_GAMUT_RGB,  {{ 0.7347  , 0.2653   }, { 0.1152  , 0.8264   }, { 0.1566  ,  0.0177   }, { 0.3457  , 0.3585   }}},
};

// ---
// Transfer characteristics
// ---
// Output support: BT709, BT1361, xvYCC, sRGB, BT2020-10, BT2020-12, PQ, HLG
typedef enum {
  SNLF_TC_AUTO,
  SNLF_TC_BT709        =  1, // BT.709-6
  SNLF_TC_UNSPECIFIED  =  2,
  //SNLF_TC_RESERVED   =  3,
  SNLF_TC_GAMMA22      =  4, // BT.470-6 M
  SNLF_TC_GAMMA28      =  5, // BT.470-6 B, G
  SNLF_TC_BT601        =  6, // BT.601-7 / BT.1700-0 NTSC / SMPTE 170M
  SNLF_TC_SMPTE240M    =  7, // SMPTE 240M
  SNLF_TC_LINEAR       =  8,
  SNLF_TC_LOG          =  9, // Logarithmic transfer characteristic (100:1 range)
  SNLF_TC_LOG_SQRT     = 10, // Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)
  SNLF_TC_XVYCC        = 11, // IEC 61966-2-4
  SNLF_TC_BT1361       = 12, // BT.1361-0
  SNLF_TC_SRGB         = 13, // IEC 61966-2-1 / sRGB / sYCC
  SNLF_TC_BT2020_10    = 14, // BT.2020 10-bit
  SNLF_TC_BT2020_12    = 15, // BT.2020 12-bit
  SNLF_TC_PQ           = 16, // SMPTE ST 2084
  SNLF_TC_SMPTE428     = 17, // SMPTE ST 428-1
  SNLF_TC_HLG          = 18, // ARIB STD-B67
  
  SNLF_TC_CUSTOM       = 255,
  SNLF_TC_GAMMA18      = (SNLF_TC_CUSTOM + 1),
  SNLF_TC_GAMMA20      = (SNLF_TC_GAMMA18 + 1),
  SNLF_TC_GAMMA23H     = (SNLF_TC_GAMMA20 + 1),
  SNLF_TC_GAMMA24      = (SNLF_TC_GAMMA23H + 1),
  SNLF_TC_GAMMA26      = (SNLF_TC_GAMMA24 + 1),
  SNLF_TC_BT1886       = (SNLF_TC_GAMMA26 + 1), // EOTF Gamma=2.4
  SNLF_TC_BT1886_G22   = (SNLF_TC_BT1886 + 1),  // EOTF Gamma=2.2
  SNLF_TC_PRO_PHOTO    = (SNLF_TC_BT1886_G22 + 1),
} SnlfTransferCharacteristicsType;

typedef struct {
  float min;
  float max;
  float gamma;
} SnlfTransferCharacteristicsSimple;

typedef struct {
  
} SnlfTransferCharacteristics;

// ---
// Matrix coefficients
// ---
enum _SnlfMatrixCoefficientsType {
  SNLF_CM_GBR         =  0, // IEC 61966-2-1 / sRGB
  SNLF_CM_BT709       =  1, // BT.709-6
  SNLF_CM_UNSPECIFIED =  2,
  //SNLF_CM_RESERVED  =  3,
  SNLF_CM_FCC         =  4, // FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
  SNLF_CM_BT470BG     =  5, // BT.601-7 625 / BT.1700-0 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
  SNLF_CM_SMPTE170M   =  6, // BT.601-7 525 / BT.1700-0 NTSC
  SNLF_CM_SMPTE240M   =  7,
  SNLF_CM_YCGCO       =  8, // Dirac / ITU-T SG16
  SNLF_CM_BT2020NCL   =  9, // BT.2020 non-constant luminance
  SNLF_CM_BT2020CL    = 10, // BT.2020 constant luminance
  SNLF_CM_YDZDX       = 11, // SMPTE ST 2085
  SNLF_CM_ICTCP       = 14, // BT.2100-2 ICTCP
};
typedef enum _SnlfMatrixCoefficientsType SnlfMatrixCoefficientsType;

// ---
// Chroma location
// ---
enum _SnlfChromaLocationType {
  SNLF_CL_UNSPECIFIED = 0,
  SNLF_CL_LEFT        = 1, // MPEG-2/4, H.264/AVC
  SNLF_CL_CENTER      = 2, // MPEG-1, JPEG, H.263
  SNLF_CL_TOPLEFT     = 3, // DV
  SNLF_CL_TOP         = 4,
  SNLF_CL_BOTTOMLEFT  = 5,
  SNLF_CL_BOTTOM      = 6,
};
typedef enum _SnlfChromaLocationType SnlfChromaLocationType;

// ---
// HDR10 (SMPTE ST 2086)
// ---
struct _SnlfHDR10Descriptor {
  union {
    struct {
      SnlfColorPrimary redPrimary;
      SnlfColorPrimary greenPrimary;
      SnlfColorPrimary bluePrimary;
      SnlfColorPrimary whitePoint;
    };
    SnlfColorPrimaries colorPrimaries;
  };
  float maxMasteringLuminance;
  float minMasteringLuminance;
  uint32_t maxContentLightLevel;
  uint32_t maxFrameAverageLightLevel;
};
typedef struct _SnlfHDR10Descriptor SnlfHDR10Descriptor;

// ---
// DMCVT (SMPTE ST 2094)
// ---


// ---
// Frame descriptor
// ---
typedef struct {
  float black, peak;
} SnlfColorRange;

typedef struct {
  float maxLuminance, minLuminance;
} SnlfLuminance;
// sRGB { 80.0, 0.02 }
// BT.709 { 100.0, 0.05 }
// PQ { 10000.0, 0.005 }

typedef struct {
  SnlfColorPrimaries primaries;
  //SnlfLuminance luminance;
  SnlfSampleAspectRatio sampleAspectRatio;
  SnlfColorRange yRange;
  SnlfColorRange uvRange;
  SnlfMatrixCoefficientsType coefficients;
} SnlfYUVFrameDescriptor;

typedef struct {
  SnlfColorPrimaries primaries;
  //SnlfLuminance luminance;
  SnlfSampleAspectRatio sampleAspectRatio;
  SnlfColorRange range;
} SnlfRGBFrameDescriptor;

typedef struct {
  SnlfColorPrimaries primaries;
  //SnlfLuminance luminance;
} SnlfNormalizedFrameDescriptor;

#endif _SNLF_GRAPHICS_DEFINES_H
