#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/param.h>
#include <sys/mman.h>

#include <ffi.h>

#include "common/compiler.h"

#include "flog.h"


#define BUF_SIZE (1<<20)

/*int flog_decode_msg(flog_msg_t *m, int fdout)
{
	ffi_type *args[34] = {
		[0]		= &ffi_type_sint,
		[1]		= &ffi_type_pointer,
		[2 ... 33]	= &ffi_type_slong
	};
	void *values[34];
	ffi_cif cif;
	ffi_arg rc;

	size_t i, ret = 0;
	char *fmt;

	values[0] = (void *)&fdout;

	if (m->magic != FLOG_MAGIC)
		return -EINVAL;
	if (m->version != FLOG_VERSION)
		return -EINVAL;

	fmt = (void *)m + m->fmt;
	values[1] = &fmt;

	for (i = 0; i < m->nargs; i++) {
		values[i + 2] = (void *)&m->args[i];
		if (m->mask & (1u << i))
			m->args[i] = (long)((void *)m + m->args[i]);
	}

	if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, m->nargs + 2,
			 &ffi_type_sint, args) == FFI_OK) {
		ffi_call(&cif, FFI_FN(dprintf), &rc, values);
	} else
		ret = -1;

	return ret;
}*/



static char _mbuf[BUF_SIZE];
static char *mbuf = _mbuf;
static char *fbuf;
static uint64_t fsize;
static uint64_t mbuf_size = sizeof(_mbuf);

int flog_map_buf(int fdout)
{
	uint64_t off = 0;
	void *addr;

	/*
	 * Two buffers are mmaped into memory. A new one is mapped when a first
	 * one is completly filled.
	 */
	if (fbuf && (mbuf - fbuf < BUF_SIZE))
		return 0;

	if (fbuf) {
		if (munmap(fbuf, BUF_SIZE * 2)) {
			fprintf(stderr, "Unable to unmap a buffer: %m");
			return 1;
		}
		off = mbuf - fbuf - BUF_SIZE;
		fbuf = NULL;
	}

	if (fsize == 0)
		fsize += BUF_SIZE;
	fsize += BUF_SIZE;

	if (ftruncate(fdout, fsize)) {
		fprintf(stderr, "Unable to truncate a file: %m");
		return -1;
	}

	if (!fbuf)
		addr = mmap(NULL, BUF_SIZE * 2, PROT_WRITE | PROT_READ,
			    MAP_FILE | MAP_SHARED, fdout, fsize - 2 * BUF_SIZE);
	else
		addr = mremap(fbuf + BUF_SIZE, BUF_SIZE,
				BUF_SIZE * 2, MREMAP_FIXED, fbuf);
	if (addr == MAP_FAILED) {
		fprintf(stderr, "Unable to map a buffer: %m");
		return -1;
	}

	fbuf = addr;
	mbuf = fbuf + off;
	mbuf_size = 2 * BUF_SIZE;

	return 0;
}


//static void bin_log(unsigned int lvl, const char *format, unsigned int nargs, unsigned int mask, va_list argptr)//себе такую
void flog_encode_msg(unsigned int lvl, const char *format, unsigned int nargs, unsigned int mask, va_list argptr)
//void flog_encode_msg(unsigned int lvl, unsigned int nargs,	const char *format, unsigned int mask,...)
{	//printf("sdf\n");
	flog_msg_t *m = (void *)mbuf;
	char *str_start, *p;
	//va_list argptr;
	size_t i;

	m->nargs = nargs;
	m->mask = mask;

	str_start = (void *)m->args + sizeof(m->args[0]) * nargs;
	p = memccpy(str_start, format, 0, mbuf_size - (str_start - mbuf));
	if (!p) {
		printf("memcpu error %d", 1);
		return;
    }
	m->fmt = str_start - mbuf;
	str_start = p;

	//va_start(argptr, format);
	for (i = 0; i < nargs; i++) {
		m->args[i] = (long)va_arg(argptr, long);
		/*
		 * If we got a string, we should either
		 * reference it when in rodata, or make
		 * a copy (FIXME implement rodata refs).
		 */
		if (mask & (1u << i)) {
			p = memccpy(str_start, (void *)m->args[i], 0, mbuf_size - (str_start - mbuf));
			if (!p) {
				printf("memcpu error %d", 2);
				return;
			}
			m->args[i] = str_start - mbuf;
			str_start = p;
		}
	}
	//va_end(argptr);
	m->size = str_start - mbuf;

	/*
	 * A magic is required to know where we stop writing into a log file,
	 * if it was not properly closed.  The file is mapped into memory, so a
	 * space in the file is allocated in advance and at the end it can have
	 * some unused tail.
	 */
	m->magic = FLOG_MAGIC;
	m->version = FLOG_VERSION;

	m->size = round_up(m->size, 8);
	
	mbuf += m->size;
	mbuf_size -= m->size;

	
}

