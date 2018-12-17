// C:\Users\leonr\Desktop\XplaneTest\transponder_device\transponder_desc.h


char ReportDescriptor[70] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x0e,                    //     USAGE_MAXIMUM (Button 14)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x0e,                    //     REPORT_COUNT (14)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x02,                    //     REPORT_SIZE (2)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0,                          //     END_COLLECTION
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x06, 0x00, 0xff,              //     USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    //     USAGE (Vendor Usage 1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
    0x75, 0x03,                    //     REPORT_SIZE (3)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x91, 0x03,                    //     OUTPUT (Cnst,Var,Abs)
    0x06, 0x00, 0xff,              //     USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x02,                    //     USAGE (Vendor Usage 2)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
    0xc0,                          //             END_COLLECTION
    0xc0                           //         END_COLLECTION
};

