#include "GameSplashScreen.h"
#include "utils/logger.h"
#include "common/common.h"
#include "fs/FSUtils.h"
#include "utils/AsyncExecutor.h"

GameSplashScreen::GameSplashScreen(int32_t w, int32_t h, gameInfo * info, bool onTV) : GuiFrame(w, h),
    bgImageColor(w, h, (GX2Color) {
    0, 0, 0, 0
}) {
    bgImageColor.setImageColor((GX2Color) {
        79, 153, 239, 255
    }, 0);
    bgImageColor.setImageColor((GX2Color) {
        79, 153, 239, 255
    }, 1);
    bgImageColor.setImageColor((GX2Color) {
        59, 159, 223, 255
    }, 2);
    bgImageColor.setImageColor((GX2Color) {
        59, 159, 223, 255
    }, 3);
    append(&bgImageColor);
    this->onTV = onTV;
    this->info = info;

    std::string filepath = "fs:" + info->gamePath + META_PATH + "/bootDRCTex.tga";
    if(onTV) {
        filepath = "fs:" + info->gamePath + META_PATH + "/bootTVTex.tga";
    }
    uint8_t *buffer = NULL;
    uint32_t bufferSize = 0;
    int iResult = FSUtils::LoadFileToMem(filepath.c_str(), &buffer, &bufferSize);
    if(iResult > 0) {
        splashScreenData = new GuiImageData(buffer, bufferSize, GX2_TEX_CLAMP_MODE_MIRROR);
        if(splashScreenData) {
            bgImageColor.setImageData(splashScreenData);
            bgImageColor.setScale(((float) h) / splashScreenData->getHeight());
        }

        //! free original image buffer which is converted to texture now and not needed anymore
        free(buffer);
    }
    this->effectFinished.connect(this, &GameSplashScreen::OnSplashScreenFadeInDone);
}

void GameSplashScreen::OnSplashScreenFadeInDone(GuiElement *element) {
    // we need to wait one more frame becaus the effects get calculated before drawing.
    launchGame = true;

}

void GameSplashScreen::draw(CVideo * v) {
    GuiFrame::draw(v);
    if(launchGame && frameCounter++ > 1) {
        DEBUG_FUNCTION_LINE("Launch game %d\n", onTV);
        gameGameSplashScreenFinished(this, info, onTV);
    }
}

GameSplashScreen::~GameSplashScreen() {
    DEBUG_FUNCTION_LINE("Destroy me\n");
    if(splashScreenData) {
        AsyncExecutor::pushForDelete(splashScreenData);
    }
}