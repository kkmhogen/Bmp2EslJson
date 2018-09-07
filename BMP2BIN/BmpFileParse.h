#ifndef BIPTMAP_2SINGLE_H
#define BIPTMAP_2SINGLE_H

#define ERR_SUCCESS 0x0
#define ERR_FILE_OPEN_FAIL  0x01
#define ERR_FILE_NOT_BMP 0x2
#define ERR_FILE_NOT_SUPPORT 0x3
#define ERR_FILE_EMPTY 0x4
#define ERR_LCD_SIZE_ERR 0x5
#define ERR_FILE_PARSE_ERR 0x6


#pragma  pack (push,1)
/*bitmap file header,size:14 bytes*/
typedef struct tagBITMAPFILEHEADER
{   
    uint16_t bfType; 
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
}BITMAPFILEHEADER;

/*bitmap information header,size:40 bytes*/
typedef struct tagBITMAPINFOHEADER
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;     //ʵ��λͼ����ռ�õ��ֽ���
    uint32_t biXPelsPerMeter; //X����ֱ���
    uint32_t biYPelsPerMeter; //Y����ֱ���
    uint32_t biClrUsed;   //ʹ�õ���ɫ�������Ϊ0�����ʾĬ��ֵ(2^��ɫλ��)
    uint32_t biClrImportant;
}BITMAPINFOHEADER;

/*color palette,size:4 bytes*/
typedef struct tagRGBQUAD
{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
}RGBQUAD;

typedef struct tagBITMAP
{
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    RGBQUAD* bmpColors;

	int width;  //��ȣ����ص�λ
    int height;//�߶�,���ص�λ
    int channels;//ͨ����

    uint8_t* imageData;
}BITMAPFILE;
#pragma pack(pop) 

typedef enum eLcdType
{
	LCD_29_TWO_COLOR,
	LCD_29_THREE_COLOR,
	LCD_42_TWO_COLOR
}LcdType;

#define LCD_WHITE_COLOR 255
#define LCD_BLACK_COLOR 0
#define LCD_RED_COLOR 1

#define LCD_29_WIDTH 296
#define LCD_29_HEIGHT 128

#define LCD_42_WIDTH 400
#define LCD_42_HEIGHT 300

class BmpFileParse
{
private:
	BITMAPFILE mBitMapFile;

public:
	BmpFileParse(void);
	~BmpFileParse(void);

	int loadBitmap(char *path);

	int getBmpLcdBinFile(LcdType nLcdType, uint8_t* pOutputLcdBinFile, uint32_t* nOutputFileLength);

private:
	//������ɫͼƬ
	bool parse1BitBmpFile(FILE* bmfp);

	//����8λbmpͼƬ
	bool parse8BitBmpFile(FILE* bmfp);

	//����24λbmpͼƬ
	bool parse24BitBmpFile(FILE* bmfp);

	void clearResource();

	void toSingleColor(BITMAPFILE *bitmap,char color);//color='R'||'G'||'B'
};

#endif