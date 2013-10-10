/*
 * CinergyT2 Device emulation
 *
 *
 * This code is licensed under the LGPL.
 */

#include "qemu-common.h"
#include "qemu/error-report.h"
#include "hw/usb.h"
#include "hw/usb/desc.h"
#include "sysemu/char.h"

#define DEBUG_Cinergy

#ifdef DEBUG_Cinergy
#define DPRINTF(fmt, ...) \
do { printf("usb-cinergyT2: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do {} while(0)
#endif

/* */

typedef struct {
    USBDevice dev;
    int init;
} USBCinergyState;

enum {
    STR_SERIALNUMBER = 1,
    STR_MANUFACTURER,
    STR_PRODUCT,
};

static const USBDescStrings desc_strings = {
    [STR_MANUFACTURER]    = "QEMU",
    [STR_PRODUCT]         = "QEMU USB CINERGYT2",
    [STR_SERIALNUMBER]    = "1",
};

static const USBDescIface desc_iface0 = {
    .bInterfaceNumber              = 0,
    .bNumEndpoints                 = 3,
    .bInterfaceClass               = USB_CLASS_VENDOR_SPEC,
    .bInterfaceSubClass            = 0xff,
    .bInterfaceProtocol            = 0x00,
    .eps = (USBDescEndpoint[]) {
        {
            .bEndpointAddress      = USB_DIR_OUT | 0x01,
            .bmAttributes          = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize        = 64,
        },{
            .bEndpointAddress      = USB_DIR_IN | 0x01,
            .bmAttributes          = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize        = 64,
        },{
            .bEndpointAddress      = USB_DIR_IN | 0x02,
            .bmAttributes          = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize        = 512,
        },
    }
};

static const USBDescDevice desc_device = {
    .bcdUSB                        = 0x0200,
    .bMaxPacketSize0               = 8,
    .bNumConfigurations            = 1,
    .confs = (USBDescConfig[]) {
        {
            .bNumInterfaces        = 1,
            .bConfigurationValue   = 1,
            .bmAttributes          = 0x80,
            .bMaxPower             = 238,
            .nif = 1,
            .ifs = &desc_iface0,
        },
    },
};

static const USBDesc desc_cinergy = {
    .id = {
        .idVendor          = 0x0ccd,
        .idProduct         = 0x0038,
        .bcdDevice         = 0x0106,
        .iManufacturer     = STR_MANUFACTURER,
        .iProduct          = STR_PRODUCT,
        .iSerialNumber     = STR_SERIALNUMBER,
    },
    .full = &desc_device,
    .str  = desc_strings,
};

/*
static USBDevice *usb_cinergy_init(USBBus *bus, const char *params)
{
    USBDevice *dev;
        
    dev = usb_create(bus, "usb-cinergyT2");
    if (!dev) {
        return NULL;
    }
    qdev_prop_set_chr(&dev->qdev, "chardev", cdrv);
    if (vendorid)
        qdev_prop_set_uint16(&dev->qdev, "vendorid", vendorid);
    if (productid)
        qdev_prop_set_uint16(&dev->qdev, "productid", productid);
    qdev_init_nofail(&dev->qdev);
    
    return dev;
}*/

static int usb_cinergy_initfn(USBDevice *dev) 
{
    USBCinergyState *s = DO_UPCAST(USBCinergyState, dev, dev);
    usb_desc_create_serial(dev);
    usb_desc_init(dev);
/*    s->intr = usb_ep_get(dev, USB_TOKEN_IN, 1);
    s->changed = 1;*/
    s->init = 1;
    return 0;
}

static void usb_cinergy_handle_reset(USBDevice *dev)
{
    return;
}

static const VMStateDescription vmstate_usb_cinergy = {
    .name = "usb-cinergyT2",
    .unmigratable = 1,
};

static void usb_cinergy_class_initfn(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    USBDeviceClass *uc = USB_DEVICE_CLASS(klass);
    
    uc->init           = usb_cinergy_initfn;
    uc->product_desc   = "QEMU USB CinergyT2";
    uc->usb_desc       = &desc_cinergy;
    uc->handle_reset   = usb_cinergy_handle_reset;
    //uc->handle_control = usb_serial_handle_control;
    //uc->handle_data    = usb_serial_handle_data;
    dc->desc           = "QEMU USB CinergyT2";
    dc->vmsd           = &vmstate_usb_cinergy;
    //dc->props = serial_properties;
    set_bit(DEVICE_CATEGORY_USB, dc->categories);
    
}
    
static const TypeInfo cinergy_info = {
    .name          = "usb-cinergyT2",
    .parent        = TYPE_USB_DEVICE,
    .instance_size = sizeof(USBCinergyState),
    .class_init    = usb_cinergy_class_initfn,
};

static void usb_cinergy_register(void) {
    type_register_static(&cinergy_info);
    usb_legacy_register("usb-cinergyT2", "cinergyT2", NULL);
}

type_init(usb_cinergy_register)
