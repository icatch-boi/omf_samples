#ifndef __LIBNLMCI_H__
#define __LIBNLMCI_H__

#include <stdbool.h>
#include <errno.h>
#include "icatchtek_mt7682_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * create an nlmci instance for control device.
 * on success, a new nlmci descriptor is returned.
 * on error, -1 is returned, and errno is set appropriately
 */
int nlmci_create(void);

/*
 * return the number of bytes received,
 * on error, -1 is returned, and errno is set appropriately
 */
ssize_t nlmci_recv(int nlmcifd, void *buf, size_t len, int *type);

/*
 * send data function, the maximum data length is 2000 bytes.
 * on success, return the number of characters sent.
 * on error, -1 is returned, and errno is set appropriately.
 */
ssize_t nlmci_send(int nlmcifd, const void *buf, size_t len, int type);

/*
 * release nlmci instance and related resources
 * on error, -1 is returned, and errno is set appropriately
 */
int nlmci_close(int nlmcifd);

/*
 * allow device system events to be received
 * on error, -1 is returned, and errno is set appropriately
 */
int nlmci_sysevent_enable(int nlmcifd);

/*
 * set the send/recv timeout in seconds.
 * the default value is 1 second
 * on error, -1 is returned, and errno is set appropriately
 */
int nlmci_send_timeout_set(int nlmcifd, unsigned int secs);
int nlmci_recv_timeout_set(int nlmcifd, unsigned int secs);

/*
 * by default, the nlmci_send() function automatically adds a packet header.
 * you can call the nlmci_raw_set() function to disable this action.
 * if the enable parameter is true, the packet header is not added.
 */
int nlmci_raw_set(int nlmcifd, bool enable);

#ifdef __cplusplus
}
#endif

#endif
