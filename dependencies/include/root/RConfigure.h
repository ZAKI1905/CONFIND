#ifndef ROOT_RConfigure
#define ROOT_RConfigure

/* Configurations file for macosx64 */
#ifdef R__HAVE_CONFIG
#define ROOTPREFIX    "/opt/local/libexec/root6"
#define ROOTBINDIR    "/opt/local/libexec/root6/bin"
#define ROOTLIBDIR    "/opt/local/libexec/root6/lib/root"
#define ROOTINCDIR    "/opt/local/libexec/root6/include/root"
#define ROOTETCDIR    "/opt/local/libexec/root6/etc/root"
#define ROOTDATADIR   "/opt/local/libexec/root6/share/root"
#define ROOTDOCDIR    "/opt/local/libexec/root6/share/doc/root"
#define ROOTMACRODIR  "/opt/local/libexec/root6/share/root/macros"
#define ROOTTUTDIR    "/opt/local/libexec/root6/share/doc/root/tutorials"
#define ROOTSRCDIR    "/opt/local/libexec/root6/share/root/src"
#define ROOTICONPATH  "/opt/local/libexec/root6/share/root/icons"
#define TTFFONTDIR    "/opt/local/libexec/root6/share/root/fonts"
#endif

#define EXTRAICONPATH ""

#undef R__HAS_SETRESUID   /**/
#define R__HAS_MATHMORE   /**/
#define R__HAS_PTHREAD    /**/
#undef R__HAS_XFT    /**/
#define R__HAS_COCOA    /**/
#undef R__HAS_VC    /**/
#define R__HAS_VDT    /**/
#undef R__HAS_VECCORE    /**/
#undef R__USE_CXXMODULES   /**/
#define R__USE_LIBCXX    /**/
#define R__HAS_STD_STRING_VIEW   /**/
#undef R__HAS_STD_EXPERIMENTAL_STRING_VIEW   /**/
#undef R__HAS_STOD_STRING_VIEW /**/
#define R__HAS_STD_APPLY /**/
#define R__HAS_STD_INVOKE /**/
#define R__HAS_STD_INDEX_SEQUENCE /**/
#define R__HAS_ATTRIBUTE_ALWAYS_INLINE /**/
#define R__HAS_ATTRIBUTE_NOINLINE /**/
#undef R__EXTERN_LLVMDIR /**/
#define R__USE_IMT   /**/
#undef R__COMPLETE_MEM_TERMINATION /**/
#undef R__HAS_CEFWEB  /**/
#undef R__HAS_QT5WEB  /**/
#define R__HAS_DAVIX  /**/

#if defined(R__HAS_VECCORE) && defined(R__HAS_VC)
#ifndef VECCORE_ENABLE_VC
#define VECCORE_ENABLE_VC
#endif
#endif

#undef R__HAS_DEFAULT_LZ4  /**/
#define R__HAS_DEFAULT_ZLIB  /**/
#undef R__HAS_DEFAULT_LZMA  /**/
#undef R__HAS_CLOUDFLARE_ZLIB /**/

#define R__HAS_TMVACPU /**/
#undef R__HAS_TMVAGPU /**/


#endif
