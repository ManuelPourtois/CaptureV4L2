#include <iostream>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>

using namespace std;


void show(__u32 mask, int bit, char *name) {

    if (mask & bit) {
        cout << name << " : yes" << endl;
    } else {
        cout << name << " : no" << endl;

    }

}

string showCtrlType(enum v4l2_ctrl_type type) {

    switch (type) {
        case V4L2_CTRL_TYPE_INTEGER:
            return "INTEGER";
        case V4L2_CTRL_TYPE_BOOLEAN    :
            return "BOOLEAN";
        case V4L2_CTRL_TYPE_MENU    :
            return "MENU";
        case V4L2_CTRL_TYPE_BUTTON    :
            return "BUTTON";
        case V4L2_CTRL_TYPE_INTEGER64 :
            return "INTEGER64";
        case V4L2_CTRL_TYPE_STRING    :
            return "STRING";
        case V4L2_CTRL_TYPE_CTRL_CLASS :
            return "CTRL_CLASS";
        default:
            return "Unknown";
    }
}

void showControl(struct v4l2_queryctrl ctrl) {
    cout << "control : Id : " << ctrl.id << " Name : '" << ctrl.name << "' Type :" <<
    showCtrlType((v4l2_ctrl_type) ctrl.type) << endl;
}

int main() {

    const char *const file = "/dev/video0";

    int fd = open(file, O_RDWR);

    if (fd == -1) {
        cerr << "Can't open file " << file << endl;
        exit(1);
    }
    struct v4l2_capability cap;

    ioctl(fd, VIDIOC_QUERYCAP, &cap);
    cout << "driver " << cap.driver << endl;
    cout << "card " << cap.card << endl;
    cout << "bus_info " << cap.bus_info << endl;
    int mask = cap.capabilities;
    show(mask, V4L2_CAP_VIDEO_CAPTURE, "V4L2_CAP_VIDEO_CAPTURE");

    show(mask, V4L2_CAP_VIDEO_OUTPUT, "V4L2_CAP_VIDEO_OUTPUT");
    show(mask, V4L2_CAP_VIDEO_OVERLAY, "V4L2_CAP_VIDEO_OVERLAY");
    show(mask, V4L2_CAP_VBI_CAPTURE, "V4L2_CAP_VBI_CAPTURE");
    show(mask, V4L2_CAP_VBI_OUTPUT, "V4L2_CAP_VBI_OUTPUT");
    show(mask, V4L2_CAP_SLICED_VBI_CAPTURE, "V4L2_CAP_SLICED_VBI_CAPTURE");
    show(mask, V4L2_CAP_SLICED_VBI_OUTPUT, "V4L2_CAP_SLICED_VBI_OUTPUT");
    show(mask, V4L2_CAP_RDS_CAPTURE, "V4L2_CAP_RDS_CAPTURE");
    show(mask, V4L2_CAP_VIDEO_OUTPUT_OVERLAY, "V4L2_CAP_VIDEO_OUTPUT_OVERLAY");
    show(mask, V4L2_CAP_HW_FREQ_SEEK, "V4L2_CAP_HW_FREQ_SEEK");
    show(mask, V4L2_CAP_TUNER, "V4L2_CAP_TUNER");
    show(mask, V4L2_CAP_AUDIO, "V4L2_CAP_AUDIO");
    show(mask, V4L2_CAP_RADIO, "V4L2_CAP_RADIO");
    show(mask, V4L2_CAP_MODULATOR, "V4L2_CAP_MODULATOR");
    show(mask, V4L2_CAP_READWRITE, "V4L2_CAP_READWRITE");
    show(mask, V4L2_CAP_ASYNCIO, "V4L2_CAP_ASYNCIO");
    show(mask, V4L2_CAP_STREAMING, "V4L2_CAP_STREAMING");

    cout << endl;
    struct v4l2_queryctrl ctrl;

    for (int id = V4L2_CID_BASE; id < V4L2_CID_LASTP1; id++) {
        ctrl.id = id;
        int ret = ioctl(fd, VIDIOC_QUERYCTRL, &ctrl);
        if (ret != -1) {

            showControl(ctrl);
        }

    }

    return 0;
}