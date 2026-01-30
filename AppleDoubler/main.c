//
//  main.c
//  AppleDoubler
//
//  Created by Toni Lääveri on 2.7.2022.
//

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/attr.h>
#include <sys/errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

uint16_t normalization_table[256][4] = {
    { 0x00A4, 0x20ac, 0 },          // €
    { 0x00c0, 0x0041, 0x0300, 0},   // À
    { 0x00c1, 0x0041, 0x0301, 0},   // Á
    { 0x00c2, 0x0041, 0x0302, 0},   // Â
    { 0x00c3, 0x0041, 0x0303, 0},   // Ã
    { 0x00c4, 0x0041, 0x0308, 0},   // Ä
    { 0x00c5, 0x0041, 0x030a, 0},   // Å
    { 0x00c7, 0x0043, 0x0327, 0},   // Ç
    { 0x00c8, 0x0045, 0x0300, 0},   // È
    { 0x00c9, 0x0045, 0x0301, 0},   // É
    { 0x00ca, 0x0045, 0x0302, 0},   // Ê
    { 0x00cb, 0x0045, 0x0308, 0},   // Ë
    { 0x00cc, 0x0049, 0x0300, 0},   // Ì
    { 0x00cd, 0x0049, 0x0301, 0},   // Í
    { 0x00ce, 0x0049, 0x0302, 0},   // Î
    { 0x00cf, 0x0049, 0x0308, 0},   // Ï
    { 0x00d1, 0x004e, 0x0303, 0},   // Ñ
    { 0x00d2, 0x004f, 0x0300, 0},   // Ò
    { 0x00d3, 0x004f, 0x0301, 0},   // Ó
    { 0x00d4, 0x004f, 0x0302, 0},   // Ô
    { 0x00d5, 0x004f, 0x0303, 0},   // Õ
    { 0x00d6, 0x004f, 0x0308, 0},   // Ö
    { 0x00d9, 0x0055, 0x0300, 0},   // Ù
    { 0x00da, 0x0055, 0x0301, 0},   // Ú
    { 0x00db, 0x0055, 0x0302, 0},   // Û
    { 0x00dc, 0x0055, 0x0308, 0},   // Ü
    { 0x00e0, 0x0061, 0x0300, 0},   // à
    { 0x00e1, 0x0061, 0x0301, 0},   // á
    { 0x00e2, 0x0061, 0x0302, 0},   // â
    { 0x00e3, 0x0061, 0x0303, 0},   // ã
    { 0x00e4, 0x0061, 0x0308, 0},   // ä
    { 0x00e5, 0x0061, 0x030a, 0},   // å
    { 0x00e7, 0x0063, 0x0327, 0},   // ç
    { 0x00e8, 0x0065, 0x0300, 0},   // è
    { 0x00e9, 0x0065, 0x0301, 0},   // é
    { 0x00ea, 0x0065, 0x0302, 0},   // ê
    { 0x00eb, 0x0065, 0x0308, 0},   // ë
    { 0x00ec, 0x0069, 0x0300, 0},   // ì
    { 0x00ed, 0x0069, 0x0301, 0},   // í
    { 0x00ee, 0x0069, 0x0302, 0},   // î
    { 0x00ef, 0x0069, 0x0308, 0},   // ï
    { 0x00f1, 0x006e, 0x0303, 0},   // ñ
    { 0x00f2, 0x006f, 0x0300, 0},   // ò
    { 0x00f3, 0x006f, 0x0301, 0},   // ó
    { 0x00f4, 0x006f, 0x0302, 0},   // ô
    { 0x00f5, 0x006f, 0x0303, 0},   // õ
    { 0x00f6, 0x006f, 0x0308, 0},   // ö
    { 0x00f9, 0x0075, 0x0300, 0},   // ù
    { 0x00fa, 0x0075, 0x0301, 0},   // ú
    { 0x00fb, 0x0075, 0x0302, 0},   // û
    { 0x00fc, 0x0075, 0x0308, 0},   // ü
    { 0x00ff, 0x0079, 0x0308, 0},   // ÿ
    { 0x0178, 0x0059, 0x0308, 0},   // Ÿ
    { 0 }
};

uint16_t macroman_ucs2_tbl[256] =
{
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
    0x00c4, 0x00c5, 0x00c7, 0x00c9, 0x00d1, 0x00d6, 0x00dc, 0x00e1, 0x00e0, 0x00e2, 0x00e4, 0x00e3, 0x00e5, 0x00e7, 0x00e9, 0x00e8,
    0x00ea, 0x00eb, 0x00ed, 0x00ec, 0x00ee, 0x00ef, 0x00f1, 0x00f3, 0x00f2, 0x00f4, 0x00f6, 0x00f5, 0x00fa, 0x00f9, 0x00fb, 0x00fc,
    0x2020, 0x00b0, 0x00a2, 0x00a3, 0x00a7, 0x2022, 0x00b6, 0x00df, 0x00ae, 0x00a9, 0x2122, 0x00b4, 0x00a8, 0x2260, 0x00c6, 0x00d8,
    0x221e, 0x00b1, 0x2264, 0x2265, 0x00a5, 0x00b5, 0x2202, 0x2211, 0x220f, 0x03c0, 0x222b, 0x00aa, 0x00ba, 0x2126, 0x00e6, 0x00f8,
    0x00bf, 0x00a1, 0x00ac, 0x221a, 0x0192, 0x2248, 0x2206, 0x00ab, 0x00bb, 0x2026, 0x00a0, 0x00c0, 0x00c3, 0x00d5, 0x0152, 0x0153,
    0x2013, 0x2014, 0x201c, 0x201d, 0x2018, 0x2019, 0x00f7, 0x25ca, 0x00ff, 0x0178, 0x2044, 0x00a4, 0x2039, 0x203a, 0xfb01, 0xfb02,
    0x2021, 0x00b7, 0x201a, 0x201e, 0x2030, 0x00c2, 0x00ca, 0x00c1, 0x00cb, 0x00c8, 0x00cd, 0x00ce, 0x00cf, 0x00cc, 0x00d3, 0x00d4,
    0xf8ff, 0x00d2, 0x00da, 0x00db, 0x00d9, 0x0131, 0x02c6, 0x02dc, 0x00af, 0x02d8, 0x02d9, 0x02da, 0x00b8, 0x02dd, 0x02db, 0x02c7
};

int utf8_to_ucs2(const char* src, uint16_t* ucs2)
{
    const unsigned char* utf8src = (const unsigned char*)src;

    *ucs2 = (uint16_t)'?';

    if (utf8src[0] < 0x80)
    {
        *ucs2 = (uint16_t)utf8src[0];
        return 1;
    }
    else if ((utf8src[0] & 0xE0) == 0xc0)
    {
        if ((utf8src[1] & 0xc0) != 0x80)
            return 0;

        *ucs2 = (uint16_t)(((utf8src[0] & 0x1f) << 6) | (utf8src[1] & 0x3f));
        return 2;
    }
    else if ((utf8src[0] & 0xf0) == 0xe0)
    {
        if (((utf8src[1] & 0xC0) != 0x80) || ((utf8src[2] & 0xC0) != 0x80))
            return 0;

        *ucs2 = (uint16_t)(((utf8src[0] & 0x0f) << 12) | ((utf8src[1] & 0x3f) << 6) | (utf8src[2] & 0x3f));
        return 3;
    }

    return 0;
}

void normalize_ucs2(uint16_t* in, uint16_t* out)
{
    int o = 0;
    for (int i = 0; i < 2048;)
    {
        if (!in[i])
            break;
        
        int match = 0;
        for (int c = 0; normalization_table[c][0] && !match; c++)
        {
            int match = 1, m;
            for (m = 0; match && normalization_table[c][m + 1] ; m++)
            {
                if (!in[i + m] || (in[i + m] != normalization_table[c][m + 1]))
                    match = 0;
            }
            
            if (match)
            {
                out[o++] = normalization_table[c][0];
                i += m;
            }
        }
        if (!match)
            out[o++] = in[i++];
    }
    out[o] = 0;
}

char ucs2_to_macroman(uint16_t ucs2)
{
    for (int i = 0; i < 256; i++)
    {
        if (macroman_ucs2_tbl[i] == ucs2)
            return i;
    }
    return '?';
}

void convert_utf8_to_ucs2(const char* src, uint16_t* dst)
{
    int out = 0;
    for (int i = 0; i < strlen(src); )
    {
        uint16_t ucs2;
        i += utf8_to_ucs2(src + i, &ucs2);
        dst[out++] = ucs2;
    }
    dst[out++] = 0;
}

void convert_ucs2_to_macroman(uint16_t* src, char* dst)
{
    int out = 0;
    for (int i = 0; i < 2048; i++)
    {
        uint16_t ucs2 = src[i];
        if (!ucs2)
            break;
        dst[out++] = ucs2_to_macroman(ucs2);
    }
    dst[out++] = 0;
}

int adf_character_tbl[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char hex_tbl[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void convert_macroman_to_adf(const char* src, char* dst)
{
    for (int i = 0; i < strlen(src); i++)
    {
        uint8_t c = src[i];
        
        if (c == ':')
            c = '/';
        else if (c == '/')
            c = ':';
        
        if (adf_character_tbl[c])
            *(dst++) = c;
        else
        {
            *(dst++) = '%';
            *(dst++) = hex_tbl[(c >> 4) & 0xf];
            *(dst++) = hex_tbl[(c) & 0xf];
        }
    }
    *(dst++) = 0;
}

typedef struct attrlist attrlist_t;

#pragma pack(push,1)

enum
{
    adfDataFork = 1,
    adfResourceFork = 2,
    adfRealName = 3,
    adfComment = 4,
    adfIconBW = 5,
    adfIconColor = 6,
    adfFileDates = 8,
    adfFinderInfo = 9,
    adfMacFileInfo = 10,
    adfProDOSFileInfo = 11,
    adfMSDOSFileInfo = 12,
    adfAFPShortName = 13,
    adfAFPFileInfo = 14,
    adfAFPDirectoryID = 15
};

enum
{
    adfFlagLocked = 0x00000001,
    adfFlagProtected = 0x00000002
};

#define kADFHeaderMagic 0x00051607
#define kADFVersion 0x00020000

struct ADFEntry
{
    int32_t entryID;
    int32_t offset;
    int32_t length;
};
typedef struct ADFEntry ADFEntry;

struct ADFHeader
{
    int32_t magic;
    int32_t version;
    uint8_t filler[16];
    int16_t numEntries;
    ADFEntry entry[0];
};
typedef struct ADFHeader ADFHeader;

typedef struct FInfo
{
    uint32_t fdType;
    uint32_t fdCreator;
    uint16_t fdFlags;
    uint32_t fdLocation;
    int16_t fdFldr;
} FInfo;

typedef struct FXInfo
{
    int16_t fdIconID;
    int16_t fdUnused[3];
    int8_t fdScript;
    int8_t fdXFlags;
    int16_t fdComment;
    int32_t fdPutAway;
} FXInfo;

typedef struct FInfoAttrBuf
{
    uint32_t length;
    FInfo fInfo;
    FXInfo fxInfo;
} FInfoAttrBuf;

typedef struct TimestampAttrBuf
{
    uint32_t length;
    struct timespec timestamp;
} TimestampAttrBuf;

#pragma pack(pop)

void inset(int amount)
{
    for (int i = 0; i < amount; i++)
        printf("  ");
}

char tmpStr[1024];
char* fixNameForPrint(const char* src)
{
    char* dst = &tmpStr[0];
    while (*src)
    {
        if (*src < 32 || *src > 127)
        {
            *(dst++) = '\\';
            *(dst++) = hex_tbl[(*src >> 4) & 0xF];
            *(dst++) = hex_tbl[(*src) & 0xF];
        }
        else
            *(dst++) = *src;
        src++;
    }
    *dst = 0;
    return tmpStr;
}

void copyfork(int depth, FILE* in, FILE* out, char* forkName)
{
    char buffer[1024];
    size_t size;
    int32_t totalSize = 0;
    
    while ((size = fread(buffer, 1, 1024, in)) != 0)
    {
        fwrite(buffer, 1, size, out);
        totalSize += size;
    }
    
    inset(depth); printf("%d bytes in %s fork\n", totalSize, forkName);
}

#define kSecondsFrom1904To1970  2082801600L
#define kSecondsFrom1970To2000  946684800L

int ConvertFolder(int depth, const char* srcpath, const char* dstpath)
{
    DIR* dirp;
    struct dirent *dp;
    
    if ((dirp = opendir(srcpath)) == NULL)
        return 1;
    
    int anyChildren = 0;

    while ((dp = readdir(dirp)))
    {
        int isDir = 0;
        const char* fName = NULL;

        struct stat stbuf;
        char tmpStatPath[1024];
        char tmpStatPathRsrc[1024];
        sprintf(tmpStatPath, "%s/%s", srcpath, dp->d_name);
        sprintf(tmpStatPathRsrc, "%s/%s/..namedfork/rsrc", srcpath, dp->d_name);
        stat(tmpStatPath, &stbuf);
        isDir = S_ISDIR(stbuf.st_mode);
        fName = (char*)dp->d_name;
        
        if (!strcmp(fName, ".") || !strcmp(fName, ".."))
            continue;
        if (!strcmp(fName, ".DS_Store"))
            continue;
        
        char macroman_name[1024], adf_name[2048];
        uint16_t ucs2_name[2049], ucs2_normalized[2048];
        convert_utf8_to_ucs2(fName, ucs2_name);
        normalize_ucs2(ucs2_name, ucs2_normalized);
        convert_ucs2_to_macroman(ucs2_normalized, macroman_name);
        convert_macroman_to_adf(macroman_name, adf_name);
        
        char outADFHeaderPath[2048];
        sprintf(outADFHeaderPath, "%s/%c%s", dstpath, '%', adf_name);
        char outADFFilePath[2048];
        sprintf(outADFFilePath, "%s/%s", dstpath, adf_name);

        int err;
        attrlist_t attrList;
        FInfoAttrBuf attrBuf;
        TimestampAttrBuf timestamp;
        
        inset(depth); printf("%s:%s -> adfname: %s\n", isDir ? "FOLD" : "FILE", fixNameForPrint(fName), adf_name);

        memset(&attrList, 0, sizeof(attrList));
        attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
        attrList.commonattr  =  ATTR_CMN_FNDRINFO;
        err = getattrlist(tmpStatPath, &attrList, &attrBuf, sizeof(attrBuf), 0);
        if (err != 0)
            err = errno;
        else
        {
            inset(depth); printf("-> file type    = '%.4s'\n", (char*)&attrBuf.fInfo.fdType);
            inset(depth); printf("-> file creator = '%.4s'\n", (char*)&attrBuf.fInfo.fdCreator);
        }
        
        uint32_t fCrDat = 0, fMdDat = 0, fBkDat = 0, fAcDat = 0;
        char buff[100];
        
        memset(&attrList, 0, sizeof(attrList));
        memset(&timestamp, 0, sizeof(timestamp));
        attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
        attrList.commonattr = ATTR_CMN_CRTIME;
        err = getattrlist(tmpStatPath, &attrList, &timestamp, sizeof(timestamp), 0);
        if (err != 0)
            err = errno;
        else
        {
            fCrDat = (int32_t)(timestamp.timestamp.tv_sec - kSecondsFrom1970To2000);
            strftime(buff, sizeof buff, "%D %T", gmtime(&timestamp.timestamp.tv_sec));
            inset(depth); printf("-> CrDat = %s => %d\n", buff, fCrDat);
        }
        
        attrList.commonattr = ATTR_CMN_MODTIME;
        err = getattrlist(tmpStatPath, &attrList, &timestamp, sizeof(timestamp), 0);
        if (err != 0)
            err = errno;
        else
        {
            fMdDat = (int32_t)(timestamp.timestamp.tv_sec - kSecondsFrom1970To2000);
            strftime(buff, sizeof buff, "%D %T", gmtime(&timestamp.timestamp.tv_sec));
            inset(depth); printf("-> MdDat = %s => %d\n", buff, fMdDat);
        }
        
        attrList.commonattr = ATTR_CMN_BKUPTIME;
        err = getattrlist(tmpStatPath, &attrList, &timestamp, sizeof(timestamp), 0);
        if (err != 0)
            err = errno;
        else
        {
            fBkDat = (int32_t)(timestamp.timestamp.tv_sec - kSecondsFrom1970To2000);
            strftime(buff, sizeof buff, "%D %T", gmtime(&timestamp.timestamp.tv_sec));
            inset(depth); printf("-> BkDat = %s => %d\n", buff, fBkDat);
        }
        
        attrList.commonattr = ATTR_CMN_ACCTIME;
        err = getattrlist(tmpStatPath, &attrList, &timestamp, sizeof(timestamp), 0);
        if (err != 0)
            err = errno;
        else
        {
            fAcDat = (int32_t)(timestamp.timestamp.tv_sec - kSecondsFrom1970To2000);
            strftime(buff, sizeof buff, "%D %T", gmtime(&timestamp.timestamp.tv_sec));
            inset(depth); printf("-> AcDat = %s => %d\n", buff, fAcDat);
        }
        
        FILE* fpDataIn = NULL;
        FILE* fpRsrcIn = NULL;
        uint32_t rsrcLength = 0;
        if (!isDir)
        {
            fpDataIn = fopen(tmpStatPath, "rb");
            fpRsrcIn = fopen(tmpStatPathRsrc, "rb");
            
            if (fpRsrcIn)
            {
                fseek(fpRsrcIn, 0, SEEK_END);
                rsrcLength = ftell(fpRsrcIn);
                fseek((fpRsrcIn), 0, SEEK_SET);
            }
        }
        
        FILE* fpHeaderOut = fopen(outADFHeaderPath, "wb");
        
        ADFHeader header;
        memset(&header, 0, sizeof(ADFHeader));
        header.magic = htonl(kADFHeaderMagic);
        header.version = htonl(kADFVersion);
        
        int numFields = isDir ? 3 : 4;
        // adfMacFileInfo + adfFileDates + adfFinderInfo + (adfResourceFork)
        header.numEntries = htons(numFields);
        
        fseek(fpHeaderOut, 0, SEEK_SET);
        if (fwrite(&header, 1, sizeof(ADFHeader), fpHeaderOut) != sizeof(ADFHeader))
        {
            exit(1);
        }

#pragma pack(push, 1)
        ADFEntry entries[10];
#pragma pack(pop)
        memset(entries, 0, sizeof(entries));
        entries[0].entryID = htonl(adfMacFileInfo); // 0x000A
        entries[0].offset = htonl(0x92);
        entries[0].length = htonl(0x4);

        entries[1].entryID = htonl(adfFileDates); // 0x0008
        entries[1].offset = htonl(0x96);
        entries[1].length = htonl(0x10);

        entries[2].entryID = htonl(adfFinderInfo); // 0x0009
        entries[2].offset = htonl(0xA6);
        entries[2].length = htonl(0x20);

        if (!isDir)
        {
            entries[3].entryID = htonl(adfResourceFork); // 0x0002
            entries[3].offset = htonl(0x200);
            entries[3].length = htonl(rsrcLength);
        }
        
        if (fwrite(&entries, 1, sizeof(entries), fpHeaderOut) != sizeof(entries))
            exit(2);
        
        // Write MacFileInfo
        int32_t flags = 0;
        if (fwrite(&flags, sizeof(int32_t), 1, fpHeaderOut) != 1)
            exit(3);
        
        // Write FileDates
        int32_t dates[4] = {0, 0, 0, 0};
        {
            dates[0] = htonl(fCrDat);
            dates[1] = htonl(fMdDat);
            dates[2] = htonl(fBkDat);
        }
        if (fwrite(&dates, sizeof(int32_t), 4, fpHeaderOut) != 4)
            exit(4);
        
        // Write FinderInfo
        if (fwrite(&attrBuf.fInfo, sizeof(FInfo), 1, fpHeaderOut) != 1)
            exit(5);
        if (fwrite(&attrBuf.fxInfo, sizeof(FXInfo), 1, fpHeaderOut) != 1)
            exit(6);
        
        fflush(fpHeaderOut);
        
        anyChildren = 1;
        
        if (isDir)
        {
            printf("\n");
            char srcsubpath[1024];
            sprintf(srcsubpath, "%s/%s", srcpath, fName);
            char dstsubpath[1024];
            sprintf(dstsubpath, "%s/%s", dstpath, adf_name);
            mkdir(dstsubpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            ConvertFolder(depth + 1, srcsubpath, dstsubpath);
        }
        else
        {
            if (ftruncate(fileno(fpHeaderOut), 0x200) == -1)
                exit(7);
           
            fseek(fpHeaderOut, 0x200, SEEK_SET);
            if (rsrcLength)
                copyfork(depth, fpRsrcIn, fpHeaderOut, "resource");
            
            FILE* fpDataOut = fopen(outADFFilePath, "wb");
            copyfork(depth, fpDataIn, fpDataOut, "data");
            fclose(fpDataOut);
            
            printf("\n");
        }
        
        if (fpDataIn)
            fclose(fpDataIn);
        if (fpRsrcIn)
            fclose(fpRsrcIn);
        if (fpHeaderOut)
            fclose(fpHeaderOut);
    }
    closedir(dirp);
    if (!anyChildren)
        printf("\n");

    return 0;
}

int main(int argc, const char * argv[]) {
    //system("pwd");
    if (argc < 3)
    {
        printf("Usage: appledouble <src-folder> <dst-folder>\n");
        exit(1);
    }
    
    return ConvertFolder(0, argv[1], argv[2]);
}
