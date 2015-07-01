#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include "setjmp.h"

#if 0
        Allocate and initialize a JPEG decompression object
        Specify the source of the compressed data (eg, a file)
        Call jpeg_read_header() to obtain image info
        Set parameters for decompression
        jpeg_start_decompress(...);
        while (scan lines remain to be read)
                jpeg_read_scanlines(...);
        jpeg_finish_decompress(...);
        Release the JPEG decompression object
#endif

/* Uage: jpg2rgb <jpg_file>
 */
int main(int argc, char *argv[])
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * infile;

	if(argc != 2)
	{
		printf("Usage: %s <jpeg_file>\n", argv[0]);
		return -1;
	}

	/* ����ͳ�ʼ��һ��decompression�ṹ�� */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* ָ��Դ�ļ� */
	if ((infile = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

	/* ��ȡͼ����Ϣ */
#if 0
	JDIMENSION image_width                  Width and height of image
	JDIMENSION image_height
	int num_components                      Number of color components
	J_COLOR_SPACE jpeg_color_space          Colorspace of image
	boolean saw_JFIF_marker                 TRUE if a JFIF APP0 marker was seen
	UINT8 JFIF_major_version              Version information from JFIF marker
	UINT8 JFIF_minor_version
	UINT8 density_unit                    Resolution data from JFIF marker
	UINT16 X_density
	UINT16 Y_density
	boolean saw_Adobe_marker                TRUE if an Adobe APP14 marker was seen
	UINT8 Adobe_transform                 Color transform code from Adobe marker
#endif
	jpeg_read_header(&cinfo, TRUE);
	/* Դ��Ϣ */
	printf("image_width = %d\n", cinfo.image_width);
	printf("image_height = %d\n", cinfo.image_height);
	printf("num_components = %d\n", cinfo.num_components);
	
	/* ���ý�ѹ����������Ŵ���С */
	/* ����ϵ����scale_num / scale_denom */
	printf("Enter M/N\n");
	scanf("%d %d", &cinfo.scale_num, &cinfo.scale_denom);
	printf("Scaled to %d/%d\n", cinfo.scale_num, cinfo.scale_denom);

	/* ������ѹ */
	jpeg_start_decompress(&cinfo);
#if 0
	JDIMENSION output_width         Actual dimensions of output image.
	JDIMENSION output_height
	int out_color_components        Number of color components in out_color_space.
	int output_components           Number of color components returned.
	int rec_outbuf_height           Recommended height of scanline buffer.
#endif
	printf("output_width = %d\n", cinfo.output_width);
	printf("output_height = %d\n", cinfo.output_height);
	printf("num_components = %d\n", cinfo.output_components);

	/* ���л�ý�ѹ���� */
//	while (scan lines remain to be read)
//        jpeg_read_scanlines(...);

	/* ֹͣ��ѹ */
	jpeg_finish_decompress(&cinfo);

	/* �ͷ�decompression�ṹ�� */
	jpeg_destroy_decompress(&cinfo);

	return 0;
}