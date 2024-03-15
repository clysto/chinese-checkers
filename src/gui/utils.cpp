#include "utils.hpp"

#include <FL/Fl.H>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdint>

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 /* need at least WinXP for this API, I think */
#endif
#include <windows.h>
#define i_load_private_font(PATH) AddFontResourceEx((PATH), FR_PRIVATE, 0)
#define v_unload_private_font(PATH) RemoveFontResourceEx((PATH), FR_PRIVATE, 0)
DWORD _count;
#define i_load_private_memory_font(NAME, DATA, LENGTH) \
  reinterpret_cast<std::uintptr_t>(AddFontMemResourceEx((void *)(DATA), (LENGTH), 0, &_count))
#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
static int i_load_private_font(const char *pf) {
  int result = 0;
  CFErrorRef err;
  // Make a URL from the font name given
  CFURLRef fontURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8 *)pf, strlen(pf), false);
  // Try to load the font file
  if (CTFontManagerRegisterFontsForURL(fontURL, kCTFontManagerScopeProcess, &err)) {
    result = 1;  // OK, we loaded the font, set this non-zero
  } else {
    printf("Failed loading font: %s\n", pf);
  }
  // discard the fontURL
  if (fontURL) CFRelease(fontURL);
  return result;
}  // i_load_private_font

static void v_unload_private_font(const char *pf) {
  CFErrorRef err;
  // Make a URL from the font name given
  CFURLRef fontURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8 *)pf, strlen(pf), false);
  // Try to unregister the font
  CTFontManagerUnregisterFontsForURL(fontURL, kCTFontManagerScopeProcess, &err);
  if (fontURL) CFRelease(fontURL);
}  // v_unload_private_font

static int i_load_private_memory_font(const char *name, const char *data, int length) {
  int result = 0;
  CFErrorRef err;
  CFDataRef fontData = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)data, length);
  CGDataProviderRef provider = CGDataProviderCreateWithCFData(fontData);
  CGFontRef font = CGFontCreateWithDataProvider(provider);
  if (CTFontManagerRegisterGraphicsFont(font, &err)) {
    result = 1;  // OK, we loaded the font, set this non-zero
  } else {
    printf("Failed loading font from memory\n");
  }
  if (font) CFRelease(font);
  if (provider) CFRelease(provider);
  if (fontData) CFRelease(fontData);
  return result;
}  // Fl_load_memory_font

#elif __ANDROID__
// Nothing!
#else /* Assume X11 with XFT/fontconfig - this will break on systems using \
         legacy Xlib fonts */
#include <fontconfig/fontconfig.h>
#include <freetype/freetype.h>
#define USE_XFT 1
#include <fontconfig/fcfreetype.h>
#define i_load_private_font(PATH) (int)FcConfigAppFontAddFile(nullptr, (const FcChar8 *)(PATH))
#define v_unload_private_font(PATH) FcConfigAppFontClear(nullptr)
FT_Library _fl_library = nullptr;
static int i_load_private_memory_font(const char *name, const char *data, int length) {
  FT_Face face;
  FcPattern *pattern;
  FcFontSet *set = FcConfigGetFonts(nullptr, FcSetApplication);
  FT_Error error;
  if (!set) {
    FcConfigAppFontAddFile(nullptr, (const FcChar8 *)":/non-existent");
    set = FcConfigGetFonts(nullptr, FcSetApplication);
    if (!set) {
      return 0;
    }
  }
  if (!_fl_library) {
    error = FT_Init_FreeType(&_fl_library);
    if (error) {
      return 0;
    }
  }
  FT_New_Memory_Face(_fl_library, (const FT_Byte *)data, length, 0, &face);
  pattern = FcFreeTypeQueryFace(face, (const FcChar8 *)name, 0, nullptr);
  FcPatternAddFTFace(pattern, FC_FT_FACE, face);
  FcFontSetAdd(set, pattern);
  return 1;
}
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

int Fl_load_font(const char *path) {
#ifndef __ANDROID__
  return i_load_private_font(path);
#else
  return 0;
#endif
}

int Fl_load_memory_font(const char *name, const char *data, int length) {
#ifndef __ANDROID__
  return i_load_private_memory_font(name, data, length);
#else
  return 0;
#endif
}

void Fl_unload_font(const char *path) {
#ifndef __ANDROID__
  v_unload_private_font(path);
#endif
}