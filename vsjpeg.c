/*======================================================================
	vsjpeg.c
			Copyright 1998, 2001 Igarashi
======================================================================*/
/*======================================================================
　本ファイルの著作権は作者（いがらし）に属しますが、利用者の常識と責任の
範囲において、自由に使用 / 複製 / 配付 / 改造することができます。その際、
許諾を求める必要もなければ、ロイヤリティの類も一切必要ありません。

　本ファイルは無保証です。本ファイルを使用した結果について、作者は一切の
責任を負わないものとします。
======================================================================*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/dos.h>
#include	<cdjpeg.h>

#define	VRAM_WIDTH		512
#define	VRAM_HEIGHT		512

typedef struct {
	int	fileidx;					/* filename */
	int	x1, y1, lwidth, lheight;	/* for -L   */
	int	square;						/* for -A   */
	unsigned short	*vsmem;			/* for -VS  */
	int	scale;						/* for -I   */
} arginfo;

static void usage( void )
{
	fprintf( stderr,
		"JPEGローダ vsjpeg.x version 0.15 Copyright 1998, 2001 Igarashi\n"
		"usage:\tvsjpeg [option] filename\n"
		"\t-L[x1,y1,[x2,y2]]\t画面に展開・表示（デフォルト）\n"
		"\t-VS$address\t\t指定のメモリに展開\n"
		"\t-A\t\t\tスクエア表示モード\n"
		"\t-Inx/ny\t\t\t展開時の縮尺を指定（1/1,1/2,1/4 or 1/8）\n" );
}

static void init_arginfo( arginfo *arginfo )
{
	arginfo->fileidx = 0;
	arginfo->x1 = 0;
	arginfo->y1 = 0;
	arginfo->lwidth = VRAM_WIDTH;
	arginfo->lheight = VRAM_HEIGHT;
	arginfo->square = FALSE;
	arginfo->vsmem = NULL;
	arginfo->scale = 1;
}

#define	NO_FILEIDX( p )		(!(( p )->fileidx))
#define	NO_VSMEM( p )		(( p )->vsmem == NULL )

static int chkarg( arginfo *arginfo, int argc, char *argv[] )
{
	int	fnflag = FALSE, lflag = FALSE, vsflag = FALSE, scaleflag = FALSE, aflag = FALSE;
	int	ret = TRUE;
	int	i;
	char	*arg;

	init_arginfo( arginfo );

	for ( i = 1; i < argc; i++ ) {
		arg = argv[i];
		if ( *arg++ != '-' ) {
			if ( fnflag ) {
				ret = FALSE;
				break;
			}
			arginfo->fileidx = i;
			fnflag = TRUE;
		} else {
			if ( !strnicmp( arg, "L", 1 )) {
				int	x2 = VRAM_WIDTH, y2 = VRAM_HEIGHT;
				if ( lflag != FALSE ) {
					ret = FALSE;
					break;
				}
				if ( arg[1] == '\0' ) {
					x2 = arginfo->lwidth;
					y2 = arginfo->lheight;
				} else if ( sscanf( &arg[1], "%d,%d,%d,%d",
						&arginfo->x1, &arginfo->y1, &x2, &y2 ) == 4 ) {
					;
				} else if ( sscanf( &arg[1], "%d,%d",
						&arginfo->x1, &arginfo->y1 ) == 2 ) {
					;
				} else {
					ret = FALSE;
					break;
				}
				if ( arginfo->x1 > x2 ) {
					int	temp;
					temp = x2;		/* x1 <= x2を保証する */
					x2 = arginfo->x1;
					arginfo->x1 = temp;
				}
				if ( arginfo->y1 > y2 ) {
					int	temp;
					temp = y2;		/* y1 <= y2を保証する */
					y2 = arginfo->y1;
					arginfo->y1 = temp;
				}
				arginfo->lwidth = x2 - arginfo->x1;
				arginfo->lheight = y2 - arginfo->y1;
				lflag = TRUE;
			} else if ( !stricmp( arg, "A" )) {
				if ( aflag != FALSE ) {
					ret = FALSE;
					break;
				}
				arginfo->square = TRUE;
				aflag = TRUE;
			} else if ( !strnicmp( arg, "VS", 2 )) {
				if ( vsflag != FALSE
					|| sscanf( &arg[2], "$%x", &arginfo->vsmem ) != 1 ) {
					ret = FALSE;
					break;
				}
				vsflag = TRUE;
			} else if ( !strnicmp( arg, "I", 1 )) {
				int	tmp;
				if ( scaleflag != FALSE || sscanf( &arg[1], "%d/%d",
						&tmp, &arginfo->scale )!= 2 ) {
					ret = FALSE;
					break;
				}
				/* "-I1/1"、"-I1/2"、"-I1/4"、"-I1/8"のみ有効 */
				if (( tmp != 1 ) ||
					(( arginfo->scale != 1 ) && ( arginfo->scale != 2 ) && 
					 ( arginfo->scale != 4 ) && ( arginfo->scale != 8 ))) {
					ret = FALSE;
					break;
				}
				scaleflag = TRUE;
			} else {
				ret = FALSE;
				break;
			}
		}
	}
	if ( NO_FILEIDX( arginfo ))	/* ファイルの指定がない */
		ret = FALSE;
	if ( vsflag ) {
		if ( lflag || aflag ) {
			ret = FALSE;
		}
	}
	return ret;
}

static unsigned short	redtbl[256] = {
#include	"redtbl.inc"
};
static unsigned short	greentbl[256] = {
#include	"greentbl.inc"
};
static unsigned short	bluetbl[256] = {
#include	"bluetbl.inc"
};

static void trans_to_vram( j_decompress_ptr cinfo, arginfo *arginfo, JSAMPROW linbuf )
{
	int	width, height;

	width = ( cinfo->output_width < arginfo->lwidth )?
				cinfo->output_width: arginfo->lwidth;
	height = ( cinfo->output_height < arginfo->lheight )?
				cinfo->output_height: arginfo->lheight;

	/* 完全不可視のケースをはじく */
	if (   ( arginfo->x1 + width <= 0 ) || ( VRAM_WIDTH <= arginfo->x1 )
		|| ( arginfo->x1 + height <= 0 ) || ( VRAM_WIDTH <= arginfo->y1 )) {
		return;
	}

	/* クリップ範囲外を展開（転送はしない） */
	{
		int	scanning_y = arginfo->y1;
		while ( scanning_y++ < 0 ) {
			jpeg_read_scanlines( cinfo, &linbuf, 1 );		/* 横１ライン展開 */
		}
	}

	{
		JSAMPROW	linbuftop, curbuf;
		unsigned short	*dest, *curdest;
		int	start_x, limit_x, limit_y;
		int ssp;

		ssp = _dos_super( 0 );	/* スーパーバイザモードへ */

		_dos_c_width( 5 );	/* 512x512 65536色 */
		if ( arginfo->square != FALSE ) {	/* スクエアモード */
			*(unsigned char  *)0xe80029 = 0x16;
			*(unsigned char  *)0xe80003 = 0x0e;
			*(unsigned char  *)0xe80005 = 0x2c;
			*(unsigned char  *)0xe80007 = 0x6c;
			*(unsigned short *)0xe80000 = 0x0089;
		}

		linbuftop = ( arginfo->x1 < 0 )?
						linbuf - arginfo->x1 * RGB_PIXELSIZE: linbuf;
		start_x = ( arginfo->x1 < 0 )? 0: arginfo->x1;
		limit_x = ( width + arginfo->x1 < VRAM_WIDTH )?
					width + arginfo->x1: VRAM_WIDTH;
		limit_y = ( height + arginfo->y1 < VRAM_HEIGHT )?
					height + arginfo->y1: VRAM_HEIGHT;
		dest = (unsigned short *)0xc00000 + start_x;
		if ( arginfo->y1 > 0 )
			dest += arginfo->y1 * VRAM_WIDTH;

#undef	PREPARE
#define	PREPARE																\
	({																		\
		jpeg_read_scanlines( cinfo, &linbuf, 1 );	/* 横１ライン展開 */	\
		curdest = dest;														\
		curbuf = linbuftop;													\
		i = limit_x - start_x;												\
	})
		switch ( cinfo->out_color_space ) {
			case JCS_GRAYSCALE:	/* グレイスケール（モノクロ）	*/
				while ( cinfo->output_scanline < limit_y - arginfo->y1 ) {
					int	i;
					PREPARE;
					while ( i-- ) {
						unsigned short	dat;
						dat = redtbl[*curbuf];						/* RGB合成      */
						dat |= greentbl[*curbuf];					/* テーブルから */
						*curdest++ = bluetbl[*curbuf++] | dat;		/* 拾ってくる   */
					}
					dest += VRAM_WIDTH;
				}
				break;
			case JCS_RGB:	/* RGB	*/
				while ( cinfo->output_scanline < limit_y - arginfo->y1 ) {
					int	i;
					PREPARE;
					while ( i-- ) {
						unsigned short	dat;
						dat = redtbl[*curbuf++];					/* RGB合成      */
						dat |= greentbl[*curbuf++];					/* テーブルから */
						*curdest++ = bluetbl[*curbuf++] | dat;		/* 拾ってくる   */
					}
					dest += VRAM_WIDTH;
				}
				break;
			default:
				break;
		}

		_dos_super( ssp );		/* ユーザーモードへ */
	}
#if 0
	/* クリップ範囲外を展開（転送はしない） */
	/* 空読みしないとアボートしてしまう？ */
	{
		int	scanning_y = cinfo->output_scanline;
		while ( scanning_y++ < cinfo->output_height ) {
			jpeg_read_scanlines( cinfo, &linbuf, 1 );		/* 横１ライン展開 */
		}
	}
#endif
}

static void trans_to_vsmem( j_decompress_ptr cinfo, arginfo *arginfo, JSAMPROW linbuf )
{
	JSAMPROW	curbuf;
	JDIMENSION	width, height;
	unsigned short	*dest, *rp, *gp, *bp;

	dest = arginfo->vsmem;
	rp = redtbl;
	gp = greentbl;
	bp = bluetbl;

	/* 縮小する場合に端数が切り上げられるようなので（libjpegの仕様？）、 */
	/* vsjpeg側で必要な量だけ転送する                                    */
	width = cinfo->image_width / arginfo->scale;
	height = cinfo->image_height / arginfo->scale;

#undef	PREPARE
#define	PREPARE																\
	({																		\
		jpeg_read_scanlines( cinfo, &linbuf, 1 );	/* 横１ライン展開 */	\
		curbuf = linbuf;													\
		i = width;															\
	})
	switch ( cinfo->out_color_space ) {
		case JCS_GRAYSCALE:	/* グレイスケール（モノクロ）	*/
			while ( cinfo->output_scanline < height ) {
				int	i;
				PREPARE;
				while ( i-- ) {
					unsigned short	dat;
					dat = rp[*curbuf];					/* RGB合成      */
					dat |= gp[*curbuf];					/* テーブルから */
					*dest++ = bp[*curbuf++] | dat;		/* 拾ってくる   */
				}
			}
			break;
		case JCS_RGB:	/* RGB	*/
			while ( cinfo->output_scanline < height ) {
				int	i;
				PREPARE;
				while ( i-- ) {
					unsigned short	dat;
					dat = rp[*curbuf++];				/* RGB合成      */
					dat |= gp[*curbuf++];				/* テーブルから */
					*dest++ = bp[*curbuf++] | dat;		/* 拾ってくる   */
				}
			}
			break;
		default:
			break;
	}
	/* 端数を展開 */
	while ( cinfo->output_scanline < cinfo->output_height ) {
		jpeg_read_scanlines( cinfo, &linbuf, 1 );		/* 横１ライン展開 */
	}
}

int main( int argc, char *argv[] )
{
	int		ret = EXIT_FAILURE;
	FILE	*infile;
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr	jerr;
	JSAMPROW	linbuf;				/* 横１ライン分のバッファへのポインタ */
	arginfo	arginfo;

	if ( chkarg( &arginfo, argc, argv ) != FALSE ) {
		if (( infile = fopen( argv[arginfo.fileidx], READ_BINARY )) != NULL ) {
			cinfo.err = jpeg_std_error( &jerr );
			cinfo.err->trace_level = 0;
			jpeg_create_decompress( &cinfo );

			jpeg_stdio_src( &cinfo, infile );	/*  */
			jpeg_read_header( &cinfo, TRUE );	/* ヘッダを読む */

			cinfo.dct_method = JDCT_FASTEST;	/* DCT最高速 */
			cinfo.do_fancy_upsampling = FALSE;	/* fancy upsampling抑止 */

			cinfo.scale_denom = arginfo.scale;	/* スケーリング設定 */

			jpeg_start_decompress( &cinfo );	/* 展開スタート */
												/* まずこれが遅い */
			/* 横１ライン分のバッファを確保 */
			if (( linbuf = malloc( cinfo.output_width * RGB_PIXELSIZE )) != NULL ) {
				if ( NO_VSMEM( &arginfo ))
					trans_to_vram( &cinfo, &arginfo, linbuf );
				else
					trans_to_vsmem( &cinfo, &arginfo, linbuf );

				free( linbuf );
				ret = jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS;
			}
#if 0
			jpeg_finish_decompress( &cinfo );	/* 後始末 */
#endif
			jpeg_destroy_decompress( &cinfo );	/*        */
			fclose( infile );
		}
	}
	if ( ret != EXIT_SUCCESS )
	usage();
	return ret;
}

