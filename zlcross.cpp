// zlCross.cpp : 定义控制台应用程序的入口点。
//


#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include "zlCross.h"


enum ColorMap
{
	ANC = 1, 
	DMC, 
	MC, 
	ROSE
};


using namespace std;
void ColorAddBright(BYTE &R, BYTE &G, BYTE &B);
//90
const std::string sy_map[] = {".","1","2","3","4","5","6","7","8","9","0","A","B","C","D","E","F","G","H","I","J","K","L","M",
"N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a","b","c","d","e","f","g","h","i","j","k","m","n","p","q","r","s",
"t","u","v","w","x","y","z","(",")",":","?","[","]","%","#","@","$","!","^","~","+","=","-","{","}","\\","&gt","&lt","&amp","&quot",
"§","№","☆","★","○","●","◎","◇","◆","□","℃","‰","�","■","△","▲","※","→","←","↑","↓","〓","¤","♀","♂",
"Σ","Φ","Ψ","Ω","Γ","Э","Ю","Я","∈","∮","∞","Ξ","β","Ж","↖","↗","↘","↙"};

std::vector<DWORD> colorset;
BOOL BMP2Txt(LPCSTR BitmapFile, char *outputText );
void Bmp32ToBmp24(char Filename[]);
char cmdlineformat[1024] = "CrossSet ";
char cmdline[1024] = {0};
char filename[512] = {0};
char output[256] = {0};
int gird_cr = 0;
int icolorMap = 0;
char colorMap[128] = {0};
bool trymode = false;
int main(int argc, TCHAR* argv[], TCHAR* envp[])
{
	HANDLE hOut;
	hOut=GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hOut,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY |  BACKGROUND_RED );
	

	SetConsoleTextAttribute(hOut,FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("ZLCross-stitching Art\n");

	SetConsoleTextAttribute(hOut,FOREGROUND_BLUE | FOREGROUND_GREEN);

	SetConsoleTitle("ZLCross-stitching Art");


	SetConsoleTextAttribute(hOut,0x0007);	

	printf("图片路径（允许拖动，不得包含空格及标点）：\n>");	
	scanf("%s",filename);

	printf("输出文件名:\n>");
	scanf("%s",output);

	int newL = 0;
	printf("最大长度:\n>");
	scanf("%d", &newL);

	int newC = 0;
	printf("最多几种颜色:\n>");
	scanf("%d", &newC);

	printf("选择绣线品牌（会导致最终的图纸配色不同） (1.ANC, 2.DMC, 3.MC, 4.ROSE)\n>");
	scanf("%d", &icolorMap);

	switch(icolorMap)
	{
	case ANC: 
		strcpy(colorMap, "ANC.png");
		break;
	case DMC: 
		strcpy(colorMap, "DMC.png");
		break;
	case MC: 
		strcpy(colorMap, "MC.png");
		break;
	case ROSE:
		strcpy(colorMap, "ROSE.png");
		break;
	default:;
	}

	printf("输出彩色图纸?（Yes：1，No：0）\n>");
	scanf("%d", &gird_cr);

	printf("正在预处理图片，请稍后...\n");

	//Resize和改颜色
	strcat(cmdlineformat, filename);
	strcat(cmdlineformat, " -resize %dx%d -colors %d +dither -map %s ");
	strcat(cmdlineformat, "temp.bmp");
	sprintf(cmdline, cmdlineformat, newL, newL, newC, colorMap);
	system(cmdline);

	//转成gif
	memset(cmdline, 0, 1024);
	strcpy(cmdline, "CrossSet ");
	strcat(cmdline, "temp.bmp");
	strcat(cmdline, " temp.gif");
	system(cmdline);

	//转回bmp
	memset(cmdline, 0, 1024);
	strcpy(cmdline, "CrossSet temp.gif ");
	strcat(cmdline, "temp.bmp");
	system(cmdline);

	printf("正在生成图纸，请稍后...\n");

	//生成html
	if (!BMP2Txt("temp.bmp", output))
	{
		system("pause");
		return -1;
	}

	printf("\n正在生成效果图和配线编号表...\n");

	//Html2Image
	memset(cmdline, 0, 1024);
	strcpy(cmdline, "CrossInter \"");
	char CurD[256];
	GetCurrentDirectory(256, CurD);
	strcat(cmdline, CurD);
	strcat(cmdline, "\\");
	strcat(cmdline, "temp.htm\" ");
	strcat(cmdline, " .\\");
	strcat(output, "_sim.png");
	strcat(cmdline, output);
	system(cmdline);

	printf("全部完成！\n");

	//删除临时文件
	//system("@del temp.gif");
	//system("@del xcprof.txt");
	//system("@del temp.htm");
	//system("@del temp.bmp");

	//观看效果
	system("pause");

	return 0;
}

BOOL BMP2Txt(LPCSTR BitmapFile, char *outputText )
{
	HDC				hdcImage;
	HBITMAP         hbm;
	BITMAP			bm;
	hdcImage = CreateCompatibleDC(NULL);
	hbm = (HBITMAP)::LoadImage( 0/*AfxGetInstanceHandle()*/,BitmapFile
		,IMAGE_BITMAP,0,0, 
		LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
	{
		printf("图片文件打开错误！\n");
		return false;
	}
	SelectObject(hdcImage, hbm);
	GetObject(hbm, sizeof(bm), &bm);

	const int MaxLengthPerPage = 90;
	int LengthLimits = 0;
	while (LengthLimits < bm.bmWidth)
	{
		int oldlengthl = LengthLimits;
		static char iPage[2] = "0";
		char htmfile[256] = {0};
		strcpy(htmfile, outputText);
		strcat(htmfile, "_table");
		strcat(htmfile, iPage);
		strcat(htmfile, ".htm");
		++(iPage[0]);
		if (iPage[0] == ':')
		{
			iPage[0] = 'A';
		}
		else if (iPage[0] == '[')
		{
			iPage[0] = 'a';
		}
		

		std::ofstream os(htmfile);

		static int curPix = 0;
		int totalPix = bm.bmWidth * bm.bmHeight;
		os	<< "<html>\n<head>\n<meta http-equiv=content-type content=\"text/html; charset=GB2312\" />\n<title>table</title>\n"
			<< "<style>\n"
			<< "td{border: 1px solid #888888; height: 6px; width: 6px; text-align: center; vertical-align: baseline; line-height: 6px; font-size: 7px; font-family: 宋体;}\n"
			<< "</style>\n</head>\n<body leftmargin=\"0\" topmargin=\"0\" marginwidth=\"0\" marginheight=\"0\" bgcolor = \"#FFFFFF\">\n"
			<< "<table cellspacing=\"0\" cellpadding=\"0\" style = \"width: "<< ((bm.bmWidth - oldlengthl)>MaxLengthPerPage?MaxLengthPerPage:(bm.bmWidth - oldlengthl)) * 7 + 1  <<"px; border-collapse:collapse; \" border=\"0\" bordercolor=\"#000000\" bgcolor=\"#ffffff\">\n";
		
		os << "<tr>";

		for (int j = 0;  (j <= bm.bmWidth - oldlengthl && j <= MaxLengthPerPage); ++j)
		{
			static int count = 0;
			os << "<td";

			if ( j % 10 == 0)
			{
				os << " style = \"border-right: 1px solid black;\"";
				os << ">" << count++;
				if (count == 10)
				{
					count = 0;
				}
			}
			else
			{
				os << ">&nbsp;";
			}

			os << "</td>";
		}
		os << "</tr>";
		
		for (int i = 0; i < bm.bmHeight; ++i)
		{
			static int count = 0;
			os << "<tr>";
			os << "<td";

			os << " style = \"border-right: 1px solid black;\"";

			if ( (i + 1) % 10 == 0)
			{
				os << " style = \"border-bottom: 1px solid black;\"";

				os << ">" << count++;
				if (count == 10)
				{
					count = 0;
				}
			}
			else
			{
				os << ">&nbsp;";
			}

			os << "</td>";
			for (int j = oldlengthl;  j < bm.bmWidth && j < oldlengthl + MaxLengthPerPage; ++j)
			{
				COLORREF color = GetPixel(hdcImage, j, i);
				if (std::find(colorset.begin(),colorset.end(), color) == colorset.end())
				{
					colorset.push_back(color);
				}

				for (unsigned int k = 0; k < colorset.size(); ++k)
				{
					if (colorset[k] == color)
					{
						os << "<td";

						if ((j+1) % 10 == 0)
						{
							os << " style = \"border-right: 1px solid black;\"";
						}

						if ( (i + 1) % 10 == 0)
						{
							os << " style = \"border-bottom: 1px solid black;\"";
						}

						if (gird_cr)
						{
							BYTE  r = GetRValue(color);
							BYTE  g = GetGValue(color);
							BYTE  b = GetBValue(color);
							//ColorAddBright(r,g,b);
							char colorStr[8] = {0};
							sprintf(colorStr, "#%x%x%x%x%x%x", r>>4,r&0x0f,g>>4,g&0x0f,b>>4,b&0x0f);
							os << " style = \"color: " << ( ( abs(long(color - RGB(0,0,0))) > abs(long(color - RGB(255,255,255))))? "BLACK;":"WHITE;" ) << " background-color:" << colorStr << "\"";
						}

						os << ">";
						if (trymode)
						{
							os << "&nbsp;";
						}
						else
						{
							os << sy_map[k].c_str();
						}
						printf("处理中: %d/%d\r", ++curPix ,totalPix);
						os << "</td>";
					}
				}
				LengthLimits = j + 1;
			}
			os << "</tr>\n";
		}
		os << "</table></body></html>";

		os.close();
	}

	//开始生成效果图
	std::map<COLORREF,int>	mColorUsage;	//每种颜色的用量
	std::ofstream os("temp.htm");
	os	<< "<html>\n<head>\n<meta http-equiv=content-type content=\"text/html; charset=GB2312\" />\n<title>table</title>\n"
		<< "<style>\n"
		<< "td{border: 1pt solid #dddddd; height: 6pt; width: 6pt; text-align: center; vertical-align: baseline; line-height: 6pt; font-size: 7pt; font-family: 宋体;}\n"
		<< "</style>\n</head>\n<boby leftmargin=\"0\" topmargin=\"0\" marginwidth=\"0\" marginheight=\"0\">\n"
		<< "<p style = \"font-size: 10pt\">[ 效果图 ]</p>"
		<< "<table cellspacing=\"0\" cellpadding=\"0\" style = \"width: "<< bm.bmWidth * 7 + 1 <<"pt; border-collapse:collapse; \" border=\"0\" bordercolor=\"#000000\" bgcolor=\"#ffffff\">\n";
		
		for (int i = 0; i < bm.bmHeight; ++i)
		{
			os << "<tr>";
			for (int j = 0; j < bm.bmWidth; ++j)
			{
				COLORREF color = GetPixel(hdcImage, j, i);

				std::map<COLORREF,int>::iterator it = mColorUsage.find(color);
				if (it != mColorUsage.end())
				{
					++(it->second);
				}
				else
				{
					mColorUsage.insert(std::make_pair(color,1));
				}

				BYTE  r = GetRValue(color);
				BYTE  g = GetGValue(color);
				BYTE  b = GetBValue(color);
				char colorStr[8] = {0};
				sprintf(colorStr, "#%x%x%x%x%x%x", r>>4,r&0x0f,g>>4,g&0x0f,b>>4,b&0x0f);
				os << "<td style = \"background-color: " << colorStr << "\"><img src = \"simulation.gif\" style = \"width: 6pt; height: 6pt;\" /></td>";
			}
			os << "</tr>";
		}

		os << "</table>"

		<< "<img height = \"" << bm.bmHeight << "\" width = \"" << bm.bmWidth  << "\" src = \"temp.bmp\">"
		<< "<p style = \"font-size: 10pt\">[ 图样资料 ]<br />宽：" << bm.bmWidth << "格<br />高：" << bm.bmHeight << "格<br />颜色数：" << colorset.size() << "<br />";
		switch(icolorMap)
		{
		case ANC: 
			ANCMapInit();
			os << "绣线：ANC<br />";
			break;
		case DMC: 
			os << "绣线：DMC<br />";
			DMCMapInit();
			break;
		case MC: 
			os << "绣线：MC<br />";
			MCMapInit();
			break;
		case ROSE:
			os << "绣线：ROSE<br />";
			ROSEMapInit();
			break;
		default:;
		}		
		os << "</p><p style = \"font-size: 10pt\">[ 配色表 ]</p>";
		os << "<table>";
		
	for (unsigned int k = 0; k < colorset.size(); ++k)
	{
		DWORD color = colorset[k];
		BYTE  r = GetRValue(color);
		BYTE  g = GetGValue(color);
		BYTE  b = GetBValue(color);

		DWORD colorIdx = 0;

		switch(icolorMap)
		{
		case ANC: 
			colorIdx = ANCMap[color];
			break;
		case DMC: 
			colorIdx = DMCMap[color];
			break;
		case MC: 
			colorIdx = MCMap[color];
			break;
		case ROSE:
			colorIdx = ROSEMap[color];
			break;
		default:;
		}

		char colorStr[8] = {0};
		sprintf(colorStr, "#%x%x%x%x%x%x", r>>4,r&0x0f,g>>4,g&0x0f,b>>4,b&0x0f);

		os << "<tr>"
		 <<  "<td style = \"vertical-align: middle; line-height: 10pt; font-size: 10pt;\">" << sy_map[k].c_str() << "</td><td style=\"vertical-align: middle; \"><font color = \"" << 
			colorStr
			<< "\" size = 6>■</font></td>";

		map<COLORREF,int>::iterator it = mColorUsage.find(color);
		if (it != mColorUsage.end())
		{
			os << "<td style = \"vertical-align: middle; line-height: 10pt; font-size: 10pt; width: 80\">占" << it->second << "格</td>";
		}

		char sColorIdx[10] = {0};
		sprintf(sColorIdx, "%d", colorIdx);
		if (icolorMap == DMC || icolorMap == MC )
		{
			if (colorIdx == 9999)
			{
				strcpy(sColorIdx,"White");
			}
			else if (colorIdx == 8888)
			{
				strcpy(sColorIdx,"Ecru");
			}
		}
		os << "<td style = \"vertical-align: middle; line-height: 10pt; font-size: 10pt; width: 80\">编号：" << sColorIdx << "</td>";
		
		os << "</tr>";
	}

	os << "</table></body></html>";
	os.close();

	DeleteDC(hdcImage);
	return TRUE;
}

// void ColorAddBright(BYTE &R, BYTE &G, BYTE &B)
// {
// 	if ((int)R + 100 < 0xFF)
// 	{
// 		R += 100;
// 	}
// 	else
// 	{
// 		R = 0xFF;
// 	}
// 
// 	if ((int)G + 100 < 0xFF)
// 	{
// 		G += 100;
// 	}
// 	else
// 	{
// 		G = 0xFF;
// 	}
// 
// 	if ((int)B + 100 < 0xFF)
// 	{
// 		B += 100;
// 	}
// 	else
// 	{
// 		B = 0xFF;
// 	}
// }
