CC=gcc
CFLAGS = -g

jpegll_enc: jpegll_enc.o readimage.o image_size.o
	$(CC) -o jpegll_enc jpegll_enc.o readimage.o image_size.o

jpegll_dec: jpegll_dec.o
	$(CC) -o jpegll_dec jpegll_dec.o


progt_enc: progt_enc.o readimage.o image_size.o
	$(CC) -o progt_enc progt_enc.o readimage.o image_size.o

progt_dec: progt_dec.o
	$(CC) -o progt_dec progt_dec.o


uqimg_enc: uqimg_enc.o readimage.o image_size.o uquanint.o stuffit.o
	$(CC) -o uqimg_enc uqimg_enc.o readimage.o image_size.o uquanint.o stuffit.o -lm

uqimg_dec: uqimg_dec.o uquanint.o unstuff.o
	$(CC) -o uqimg_dec uqimg_dec.o uquanint.o unstuff.o -lm


uquan_ran: uquan_ran.o uquanflt.o rangen.o entropy.o
	$(CC) -o uquan_ran uquan_ran.o uquanflt.o rangen.o  entropy.o -lm


nuq_ran: nuq_ran.o nuquanflt.o rangen.o entropy.o
	$(CC) -o nuq_ran nuq_ran.o nuquanflt.o rangen.o entropy.o -lm


aqfimg_enc: aqfimg_enc.o readimage.o image_size.o uquanint.o stuffit.o
	$(CC) -o aqfimg_enc aqfimg_enc.o readimage.o image_size.o uquanint.o stuffit.o -lm

aqfimg_dec: aqfimg_dec.o uquanint.o readnbits.o
	$(CC) -o aqfimg_dec aqfimg_dec.o uquanint.o readnbits.o -lm


misuquan: misuquan.o uquanflt.o rangen.o entropy.o
	$(CC) -o misuquan misuquan.o uquanflt.o rangen.o  entropy.o -lm


misnuq: misnuq.o nuquanflt.o rangen.o entropy.o
	$(CC) -o misnuq misnuq.o nuquanflt.o rangen.o entropy.o -lm

trainvq: trainvq.o vqencode.o
	$(CC) -o trainvq trainvq.o vqencode.o

trvqsplit: trvqsplit.o vqencode.o
	$(CC) -o trvqsplit trvqsplit.o vqencode.o

trvqsp_img: trvqsp_img.o vqencode.o readimage.o image_size.o
	$(CC) -o trvqsp_img trvqsp_img.o vqencode.o image_size.o readimage.o

vqimg_enc: vqimg_enc.o vqencode.o readimage.o image_size.o stuffit.o
	$(CC) -o vqimg_enc vqimg_enc.o vqencode.o image_size.o readimage.o stuffit.o -lm

vqimg_dec: vqimg_dec.o unstuff.o
	$(CC) -o vqimg_dec vqimg_dec.o unstuff.o -lm


distimg: distimg.o readimage.o image_size.o
	$(CC) -o distimg distimg.o readimage.o image_size.o -lm


entfile: entfile.o entropy.o
	$(CC) -o entfile entfile.o entropy.o -lm

dpcm_enc: dpcm_enc.o readau.o nuquanflt.o predictor.o norm.o stuffit.o get_file_size.o entropy.o
	$(CC) -o dpcm_enc dpcm_enc.o readau.o nuquanflt.o predictor.o  norm.o stuffit.o get_file_size.o entropy.o -lm

huff_enc: huff_enc.o imsub.o sub.o
	$(CC) -o huff_enc huff_enc.o imsub.o sub.o

huff_dec: huff_dec.o imsub.o sub.o
	$(CC) -o huff_dec huff_dec.o imsub.o sub.o

dpcm_dec: dpcm_dec.o unstuff.o nuquanflt.o predictor.o
	$(CC) -o dpcm_dec dpcm_dec.o unstuff.o nuquanflt.o predictor.o -lm

distau: distau.o readau.o get_file_size.o
	$(CC) -o distau distau.o readau.o get_file_size.o -lm

all: jpegll_enc jpegll_dec progt_enc progt_dec misnuq uquan_ran uqimg_enc \
uqimg_dec nuq_ran aqfimg_enc aqfimg_dec misuquan trainvq trvqsplit trvqsp_img \
vqimg_enc vqimg_dec distimg entfile dpcm_enc dpcm_dec distau
