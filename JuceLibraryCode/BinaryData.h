/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   logo_svg;
    const int            logo_svgSize = 14004;

    extern const char*   logo_png;
    const int            logo_pngSize = 3879;

    extern const char*   OpenSansLight_ttf;
    const int            OpenSansLight_ttfSize = 129756;

    extern const char*   OpenSansMedium_ttf;
    const int            OpenSansMedium_ttfSize = 129948;

    extern const char*   OpenSansRegular_ttf;
    const int            OpenSansRegular_ttfSize = 129796;

    extern const char*   OpenSansSemiBold_ttf;
    const int            OpenSansSemiBold_ttfSize = 129716;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
