// CvWork.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "wy_visualCrypto.h"
using namespace WY_VC1;
int main()
{
	//running environment: open cv
	// put the presentation folder CV_task on the root of D disk can run the example test
	// and you can check the result from the CV_task\result folder
	genGrayKeyPic("D:\\CV_task\\NoInnocent_Gray_test\\Img.png");
	recoverGrayPic("D:\\CV_task\\NoInnocent_Gray_test\\key1.png", "D:\\CV_task\\NoInnocent_Gray_test\\key2.png");

	genColorKeyPic("D:\\CV_task\\NoInnocent_Color_test\\Img.png");
	recoverColorKeyPic("D:\\CV_task\\NoInnocent_Color_test\\Key1_OBGImg.png", "D:\\CV_task\\NoInnocent_Color_test\\Key2_OBGImg.png", "D:\\CV_task\\NoInnocent_Color_test\\Key1_OGGImg.png", "D:\\CV_task\\NoInnocent_Color_test\\Key2_OGGImg.png", "D:\\CV_task\\NoInnocent_Color_test\\Key1_ORGImg.png", "D:\\CV_task\\NoInnocent_Color_test\\Key2_ORGImg.png");

	genBWStackingKeyPic("D:\\CV_task\\Innocent_BW_test\\Img.png", "D:\\CV_task\\Innocent_BW_test\\key1.png", "D:\\CV_task\\Innocent_BW_test\\key2.png");
	recoverBWStackingPic("D:\\CV_task\\Innocent_BW_test\\BWkey1.png", "D:\\CV_task\\Innocent_BW_test\\BWkey2.png");
	
	genGrayStackingKeyPic("D:\\CV_task\\Innocent_Gray_test\\Img.png", "D:\\CV_task\\Innocent_Gray_test\\key1.png", "D:\\CV_task\\Innocent_Gray_test\\key2.png");
	recoverGrayStackingPic("D:\\CV_task\\Innocent_Gray_test\\Graykey1.png", "D:\\CV_task\\Innocent_Gray_test\\Graykey2.png");

	genColorStackingKeyPic("D:\\CV_task\\Innocent_Color_test\\Img.png", "D:\\CV_task\\Innocent_Color_test\\key1.png", "D:\\CV_task\\Innocent_Color_test\\key2.png");
	recoverColorStackingPic("D:\\CV_task\\Innocent_Color_test\\Colorkey1.png", "D:\\CV_task\\Innocent_Color_test\\Colorkey2.png");
	cv::waitKey(0);
    return 0;
}
