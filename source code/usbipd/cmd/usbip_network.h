/*
 * Copyright (C) 2005-2007 Takahiro Hirofuchi
 */

#ifndef _USBIP_NETWORK_H
#define _USBIP_NETWORK_H

#include "usbip.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>


/* -------------------------------------------------- */
/* Define Protocol Format                             */
/* -------------------------------------------------- */


/* ---------------------------------------------------------------------- */
/* Common header for all the kinds of PDUs. */
struct op_common {
	uint16_t version;

#define OP_REQUEST	(0x80 << 8)
#define OP_REPLY	(0x00 << 8)
	uint16_t code;

	/* add more error code */
#define ST_OK	0x00
#define ST_NA	0x01
	uint32_t status; /* op_code status (for reply) */

} __attribute__((packed));

#define PACK_OP_COMMON(pack, op_common)  do {\
	pack_uint16_t(pack, &(op_common)->version);\
	pack_uint16_t(pack, &(op_common)->code   );\
	pack_uint32_t(pack, &(op_common)->status );\
} while (0)


/* ---------------------------------------------------------------------- */
/* Dummy Code */
#define OP_UNSPEC	0x00
#define OP_REQ_UNSPEC	OP_UNSPEC
#define OP_REP_UNSPEC	OP_UNSPEC

/* ---------------------------------------------------------------------- */
/* Retrieve USB device information. (still not used) */
#define OP_DEVINFO	0x02
#define OP_REQ_DEVINFO	(OP_REQUEST | OP_DEVINFO)
#define OP_REP_DEVINFO	(OP_REPLY   | OP_DEVINFO)

struct op_devinfo_request {
	char busid[SYSFS_BUS_ID_SIZE];
} __attribute__((packed));

struct op_devinfo_reply {
	struct usb_device udev;
	struct usb_interface uinf[];
} __attribute__((packed));


/* ---------------------------------------------------------------------- */
/* Import a remote USB device. */
#define OP_IMPORT	0x03
#define OP_REQ_IMPORT	(OP_REQUEST | OP_IMPORT)
#define OP_REP_IMPORT   (OP_REPLY   | OP_IMPORT)

struct op_import_request {
	char busid[SYSFS_BUS_ID_SIZE];
} __attribute__((packed));

struct op_import_reply {
	struct usb_device udev;
//	struct usb_interface uinf[];
} __attribute__((packed));

#define PACK_OP_IMPORT_REQUEST(pack, request)  do {\
} while (0)

#define PACK_OP_IMPORT_REPLY(pack, reply)  do {\
	pack_usb_device(pack, &(reply)->udev);\
} while (0)



/* ---------------------------------------------------------------------- */
/* Export a USB device to a remote host. */
#define OP_EXPORT	0x06
#define OP_REQ_EXPORT	(OP_REQUEST | OP_EXPORT)
#define OP_REP_EXPORT	(OP_REPLY   | OP_EXPORT)

struct op_export_request {
	struct usb_device udev;
} __attribute__((packed));

struct op_export_reply {
	int returncode; 
} __attribute__((packed));


#define PACK_OP_EXPORT_REQUEST(pack, request)  do {\
	pack_usb_device(pack, &(request)->udev);\
} while (0)

#define PACK_OP_EXPORT_REPLY(pack, reply)  do {\
} while (0)

/* ---------------------------------------------------------------------- */
/* un-Export a USB device from a remote host. */
#define OP_UNEXPORT	0x07
#define OP_REQ_UNEXPORT	(OP_REQUEST | OP_UNEXPORT)
#define OP_REP_UNEXPORT	(OP_REPLY   | OP_UNEXPORT)

struct op_unexport_request {
	struct usb_device udev;
} __attribute__((packed));

struct op_unexport_reply {
	int returncode; 
} __attribute__((packed));

#define PACK_OP_UNEXPORT_REQUEST(pack, request)  do {\
	pack_usb_device(pack, &(request)->udev);\
} while (0)

#define PACK_OP_UNEXPORT_REPLY(pack, reply)  do {\
} while (0)



/* ---------------------------------------------------------------------- */
/* Negotiate IPSec encryption key. (still not used) */
#define OP_CRYPKEY	0x04
#define OP_REQ_CRYPKEY	(OP_REQUEST | OP_CRYPKEY)
#define OP_REP_CRYPKEY	(OP_REPLY   | OP_CRYPKEY)

struct op_crypkey_request {
	/* 128bit key */
	uint32_t key[4];
} __attribute__((packed));

struct op_crypkey_reply {
	uint32_t __reserved;
} __attribute__((packed));


/* ---------------------------------------------------------------------- */
/* Retrieve the list of exported USB devices. */
#define OP_DEVLIST	0x05
#define OP_REQ_DEVLIST	(OP_REQUEST | OP_DEVLIST)
#define OP_REP_DEVLIST	(OP_REPLY   | OP_DEVLIST)

struct op_devlist_request {
} __attribute__((packed));

struct op_devlist_reply {
	uint32_t ndev;
	/* followed by reply_extra[] */
} __attribute__((packed));

struct op_devlist_reply_extra {
	struct usb_device    udev;
	struct usb_interface uinf[];
} __attribute__((packed));

#define PACK_OP_DEVLIST_REQUEST(pack, request)  do {\
} while (0)

#define PACK_OP_DEVLIST_REPLY(pack, reply)  do {\
	pack_uint32_t(pack, &(reply)->ndev);\
} while (0)


/* -------------------------------------------------- */
/* Declare Prototype Function                         */
/* -------------------------------------------------- */

void pack_uint32_t(int pack, uint32_t *num);
void pack_uint16_t(int pack, uint16_t *num);
void pack_usb_device(int pack, struct usb_device *udev);
void pack_usb_interface(int pack, struct usb_interface *uinf);

ssize_t usbip_recv(int sockfd, void *buff, size_t bufflen);
ssize_t usbip_send(int sockfd, void *buff, size_t bufflen);
#define usbip_sendv(fd, iov, io_count) usbip_xmitv(fd, iov, io_count, 1)
#define usbip_recvv(fd, iov, io_count) usbip_xmitv(fd, iov, io_count, 0)
ssize_t usbip_xmitv(int sockfd, struct iovec *iov, int io_count, int sending);
int usbip_send_op_common(int sockfd, uint32_t code, uint32_t status);
int usbip_recv_op_common(int sockfd, uint16_t *code);
int usbip_set_reuseaddr(int sockfd);
int usbip_set_nodelay(int sockfd);
int usbip_set_keepalive(int sockfd);

int tcp_connect(char *hostname, char *service);

#define USBIP_PORT 3240
#define USBIP_PORT_STRING "3240"

#endif