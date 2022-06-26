#ifndef MUR_TARGETVER_H
#define MUR_TARGETVER_H 

/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

/// 如果要为以前的 Windows 平台生成应用程序，请包括 WinSDKVer.h，并将
/// WIN32_WINNT 宏设置为要支持的平台，然后再包括 SDKDDKVer.h。

#if 0
/// 定义可用的最高版本的 Windows 平台
#include <SDKDDKVer.h>
#endif

#ifndef WINVER                  // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410   // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif

#ifndef _WIN32_IE               // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#endif /*MUR_TARGETVER_H*/