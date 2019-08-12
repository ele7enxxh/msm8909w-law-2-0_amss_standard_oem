#ifndef QHSUSB_FD_COMMON_H
#define QHSUSB_FD_COMMON_H


PACKED struct hdlc_config_struct_obex {
  usb_desc_configuration_t conf1;
  usb_desc_interface_t ifc_data;
  usb_desc_endpoint_t ep_in;
  usb_desc_endpoint_t ep_out;
#ifdef FEATURE_QHSUSB_MS
  usb_desc_interface_t ifc_ms;
  usb_desc_endpoint_t ep_in_ms;
  usb_desc_endpoint_t ep_out_ms;
#endif /* FEATURE_QHSUSB_MS */
};

PACKED struct hdlc_config_struct_obex_only {
  usb_desc_configuration_t conf1;
  usb_desc_interface_t ifc_data;
  usb_desc_endpoint_t ep_in;
  usb_desc_endpoint_t ep_out;
};

#endif /*QHSUSB_FD_COMMON_H */
