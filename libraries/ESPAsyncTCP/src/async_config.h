#ifndef LIBRARIES_ESPASYNCTCP_SRC_ASYNC_CONFIG_H_
#define LIBRARIES_ESPASYNCTCP_SRC_ASYNC_CONFIG_H_

#ifndef ASYNC_TCP_SSL_ENABLED
#define ASYNC_TCP_SSL_ENABLED 0
#endif

#ifndef TCP_MSS
#define TCP_MSS (1460)
#endif

#include <DebugPrintMacros.h>

#ifndef ASYNC_TCP_ASSERT
#define ASYNC_TCP_ASSERT(...) do { (void)0;} while(false)
#endif
#ifndef ASYNC_TCP_DEBUG
#define ASYNC_TCP_DEBUG(...) do { (void)0;} while(false)
#endif
#ifndef TCP_SSL_DEBUG
#define TCP_SSL_DEBUG(...) do { (void)0;} while(false)
#endif

#endif
