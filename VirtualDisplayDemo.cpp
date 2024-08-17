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

int main() {
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

	fprintf(stderr, "Create Virtual Display(layer_stack=%d)\n", mVirtualDisplayLayerStack);
	fprintf(stderr, "Press any key to exit, waiting ...\n");
	getchar();
	fprintf(stderr, "Exit and destroy Virtual Display\n");

	SurfaceComposerClient::destroyDisplay(mVirtualDisplay);

    return 0;
}