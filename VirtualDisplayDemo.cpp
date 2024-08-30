/*
 * Copyright (C) 2021 The Android Open Source Project
 */

#define LOG_TAG "VirtualDisplay"

#include <android/gui/ISurfaceComposerClient.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <gui/BLASTBufferQueue.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <hardware/gralloc.h>
#include <ui/DisplayState.h>
#include <ui/GraphicBuffer.h>
#include <ui/DisplayState.h>
#include <utils/Log.h>

using namespace android;

ui::DisplayState mMainDisplayState;
ui::DisplayMode mMainDisplayMode;
sp<IBinder> mMainDisplay;
sp<IBinder> mVirtualDisplay;
sp<SurfaceControl> mSurfaceControl;
const int mVirtualDisplayLayerStack = 99; 
bool mEnableMirrorDisplay = false;

static void usage(const char *me)
{
    fprintf(stderr, "\nusage: \t%s [options]\n"
                    "\t--------------------------------------- options ------------------------------------------------\n"
                    "\t[-h] help\n"
                    "\t[-m] enable mirror main display\n"
                    "\t------------------------------------------------------------------------------------------------\n",
                    me);
    exit(1);
}

void parseOptions(int argc, char **argv) {
    const char *me = argv[0];
    int res;
    while((res = getopt(argc, argv, "mh")) >= 0) {
        switch(res) {
            case 'm':
                mEnableMirrorDisplay = true;
                break;
            case 'h':
            default:
            {
                usage(me);
            }
        }
    }
}

int main(int argc, char **argv) {
    parseOptions(argc, argv);

    const auto ids = SurfaceComposerClient::getPhysicalDisplayIds();
    mMainDisplay = SurfaceComposerClient::getPhysicalDisplayToken(ids.front());
    SurfaceComposerClient::getDisplayState(mMainDisplay, &mMainDisplayState);
    SurfaceComposerClient::getActiveDisplayMode(mMainDisplay, &mMainDisplayMode);

    // 创建Surface用于显示虚拟屏幕的内容
    mSurfaceControl = SurfaceComposerClient::getDefault()->createSurface(String8("VirtualDisplay-Surface"), 
                                                                  mMainDisplayMode.resolution.getWidth()/2, 
                                                                  mMainDisplayMode.resolution.getHeight()/2,
                                                                  PIXEL_FORMAT_RGBA_8888,
                                                                  ISurfaceComposerClient::eFXSurfaceBufferState,
                                                                  /*parent*/ nullptr);

    SurfaceComposerClient::Transaction()
            .setLayer(mSurfaceControl, std::numeric_limits<int32_t>::max())
            .setLayerStack(mSurfaceControl, ui::DEFAULT_LAYER_STACK)
            .setPosition(mSurfaceControl, 100, 100)
            .show(mSurfaceControl)
            .apply();

    mVirtualDisplay =
            SurfaceComposerClient::createDisplay(String8("Super-VirtualDisplay"), false /*secure*/);
    SurfaceComposerClient::Transaction t;
    t.setDisplaySurface(mVirtualDisplay, mSurfaceControl->getIGraphicBufferProducer());
    t.setDisplayLayerStack(mVirtualDisplay, ui::LayerStack::fromValue(mVirtualDisplayLayerStack));
    t.setDisplayProjection(mVirtualDisplay, ui::ROTATION_0,
                           Rect(mMainDisplayState.layerStackSpaceRect), 
                           Rect(0, 0, mMainDisplayMode.resolution.getWidth()/2, mMainDisplayMode.resolution.getHeight()/2));
    t.apply(true);

    sp<SurfaceControl> mirrorRoot;
    if(mEnableMirrorDisplay) {
        // 创建主屏幕的镜像 mirrorRoot
        mirrorRoot = SurfaceComposerClient::getDefault()->mirrorDisplay(ids.front());
        if (mirrorRoot == nullptr) {
            fprintf(stderr, "Failed to create a mirror for VirtualDisplayDemo");
            return -1;
        }
        // 将mirrorRoot显示到虚拟屏幕上，也就时虚拟屏幕和主屏幕显示同样内容
        t.setLayerStack(mirrorRoot, ui::LayerStack::fromValue(mVirtualDisplayLayerStack));
        // 设置mirrorRoot在虚拟屏幕上显示的位置大小
        //t.setDestinationFrame(mirrorRoot, Rect(0, 0, mMainDisplayMode.resolution.getWidth()/2, mMainDisplayMode.resolution.getHeight()/2));
        t.apply();
    }

    fprintf(stderr, "Create Virtual Display(layer_stack=%d)\n", mVirtualDisplayLayerStack);
    fprintf(stderr, "Press any key to exit, waiting ...\n");
    getchar();
    fprintf(stderr, "Exit and destroy Virtual Display\n");

    SurfaceComposerClient::destroyDisplay(mVirtualDisplay);

    return 0;
}
