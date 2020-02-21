#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tagha_libc.h"

#define TAGHA_BUFSIZ    1024
static struct {
	char buf[TAGHA_BUFSIZ];
	size_t count;
} _g_tagha_output;

int32_t _tagha_gen_printf(const char *fmt, size_t currarg, const union TaghaVal params[], size_t paramsize);

/*
 * File Ops
 */

/** int remove(const char *filename); */
static union TaghaVal native_remove(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = remove(params[0].string) };
}

/** int rename(const char *oldname, const char *newname); */
static union TaghaVal native_rename(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = rename(params[0].string, params[1].string) };
}

/** FILE *tmpfile(void); */
static union TaghaVal native_tmpfile(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args; (void)params;
	return (union TaghaVal){ .ptrvoid = tmpfile() };
}

/** char *tmpnam(char *str); */
/** tmpnam is dangerous to use.
static union TaghaVal native_tmpnam(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .string = tmpnam(params[0].string);
}
*/


/*
 * File Access
 */

/** int fclose(FILE *stream); */
static union TaghaVal native_fclose(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const f = params[0].ptrvoid;
	return (union TaghaVal){ .int32 = (f != NULL) ? fclose(f) : -1 };
}

/** int fflush(FILE *stream); */
static union TaghaVal native_fflush(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = fflush(params[0].ptrvoid) };
}

/** FILE *fopen(const char *filename, const char *modes); */
static union TaghaVal native_fopen(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	const char *restrict mode = params[1].string;
	return( mode==NULL ) ? (union TaghaVal){ .ptrvoid = NULL } : (union TaghaVal){ .ptrvoid = fopen(params[0].ptrvoid, mode) };
}

/** FILE *freopen(const char *filename, const char *mode, FILE *stream); */
static union TaghaVal native_freopen(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	const char *restrict filename = params[0].string;
	const char *restrict mode = params[1].string;
	FILE *const restrict stream = params[2].ptrvoid;
	
	if( filename==NULL || mode==NULL || stream==NULL )
		return (union TaghaVal){ .ptrvoid = NULL };
	else return (union TaghaVal){ .ptrvoid = freopen(filename, mode, stream) };
}

/** void setbuf(FILE *stream, char *buffer); */
static union TaghaVal native_setbuf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const stream = params[0].ptrvoid;
	if( stream != NULL )
		setbuf(stream, params[1].string);
	return (union TaghaVal){ 0 };
}


/** int fprintf(FILE *stream, const char *format, ...); */
static union TaghaVal native_fprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const restrict stream = params[0].ptrvoid;
	const char *restrict fmt = params[1].string;
	if( stream==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		_tagha_gen_printf(fmt, 2, params, args);
		return (union TaghaVal){ .int32 = fprintf(stream, "%s", _g_tagha_output.buf) };
		/// ugghhhh
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int printf(const char *fmt, ...); */
static union TaghaVal native_printf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	const char *restrict fmt = params[0].string;
	if( fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		_tagha_gen_printf(fmt, 1, params, args);
		return (union TaghaVal){ .int32 = printf("%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int sprintf(char *str, const char *format, ...); */
static union TaghaVal native_sprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	char *restrict str = params[0].string;
	const char *restrict fmt = params[1].string;
	if( str==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		_tagha_gen_printf(fmt, 2, params, args);
		return (union TaghaVal){ .int32 = sprintf(str, "%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int snprintf(char *s, size_t n, const char *format, ...); */
static union TaghaVal native_snprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	char *restrict str = params[0].string;
	const char *restrict fmt = params[2].string;
	if( str==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		_tagha_gen_printf(fmt, 3, params, args);
		return (union TaghaVal){ .int32 = snprintf(str, params[1].uint64, "%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int vprintf(const char *fmt, va_list arg); */
static union TaghaVal native_vprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	const char *restrict fmt = params[0].string;
	if( fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		const struct { union TaghaVal area; uint64_t args; } *const restrict valist = params[1].ptrvoid;
		_tagha_gen_printf(fmt, 0, valist->area.ptrself, valist->args);
		return (union TaghaVal){ .int32 = printf("%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int vfprintf(FILE *stream, const char *format, va_list arg); */
static union TaghaVal native_vfprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const restrict stream = params[0].ptrvoid;
	const char *restrict fmt = params[1].string;
	if( stream==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		const struct { union TaghaVal area; uint64_t args; } *const valist = params[2].ptrvoid;
		_tagha_gen_printf(fmt, 0, valist->area.ptrself, valist->args);
		return (union TaghaVal){ .int32 = fprintf(stream, "%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int vsprintf(char *s, const char *format, va_list arg); */
static union TaghaVal native_vsprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	char *restrict str = params[0].ptrvoid;
	const char *restrict fmt = params[1].string;
	if( str==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		const struct { union TaghaVal area; uint64_t args; } *const valist = params[2].ptrvoid;
		_tagha_gen_printf(fmt, 0, valist->area.ptrself, valist->args);
		return (union TaghaVal){ .int32 = sprintf(str, "%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}

/** int vsnprintf(char *s, size_t n, const char *format, va_list arg); */
static union TaghaVal native_vsnprintf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	char *restrict str = params[0].ptrvoid;
	const char *restrict fmt = params[2].string;
	if( str==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		const struct { union TaghaVal area; uint64_t args; } *const restrict valist = params[3].ptrvoid;
		_tagha_gen_printf(fmt, 0, valist->area.ptrself, valist->args);
		return (union TaghaVal){ .int32 = snprintf(str, params[1].uint64, "%s", _g_tagha_output.buf) };
		memset(&_g_tagha_output, 0, sizeof _g_tagha_output);
	}
}


/** int fscanf(FILE *stream, const char *format, ...); */
static union TaghaVal native_fscanf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const restrict stream = params[0].ptrvoid;
	const char *restrict fmt = params[1].string;
	if( stream==NULL || fmt==NULL ) {
		return (union TaghaVal){ .int32 = -1 };
	} else {
		return (union TaghaVal){ .int32 = -1 };
	}
}

/** int puts(const char *s); */
static union TaghaVal native_puts(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = puts(params[0].string) };
}

/** int setvbuf(FILE *stream, char *buffer, int mode, size_t size); */
static union TaghaVal native_setvbuf(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	FILE *const restrict stream = params[0].ptrvoid;
	char *restrict buffer = params[1].string;
	if( stream==NULL || buffer==NULL )
		return (union TaghaVal){ .int32 = -1 };
	else return (union TaghaVal){ .int32 = setvbuf(stream, buffer, params[2].int32, params[3].uint64) };
}

/** int fgetc(FILE *stream); */
static union TaghaVal native_fgetc(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = fgetc(params[0].ptrvoid) };
}

/** char *fgets(char *str, int num, FILE *stream); */
static union TaghaVal native_fgets(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .string = fgets(params[0].string, params[1].int32, params[2].ptrvoid) };
}

/** int fputc(int character, FILE *stream); */
static union TaghaVal native_fputc(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = fputc(params[0].int32, params[1].ptrvoid) };
}

/** int fputs(const char *str, FILE *stream); */
static union TaghaVal native_fputs(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = fputs(params[0].string, params[1].ptrvoid) };
}

/** int getc(FILE *stream); */
static union TaghaVal native_getc(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = getc(params[0].ptrvoid) };
}

/** int getchar(void); */
static union TaghaVal native_getchar(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args; (void)params;
	return (union TaghaVal){ .int32 = getchar() };
}

/** int putc(int character, FILE *stream); */
static union TaghaVal native_putc(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = putc(params[0].int32, params[1].ptrvoid) };
}

/** int putchar(int character); */
static union TaghaVal native_putchar(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = putchar(params[0].int32) };
}

/** int ungetc(int character, FILE *stream); */
static union TaghaVal native_ungetc(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = ungetc(params[0].int32, params[1].ptrvoid) };
}

/** size_t fread(void *ptr, size_t size, size_t count, FILE *stream); */
static union TaghaVal native_fread(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .uint64 = fread(params[0].ptrvoid, params[1].uint64, params[2].uint64, params[3].ptrvoid) };
}

/** size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream); */
static union TaghaVal native_fwrite(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .uint64 = fwrite(params[0].ptrvoid, params[1].uint64, params[2].uint64, params[3].ptrvoid) };
}

/** int fseek(FILE *stream, long int offset, int origin); */
static union TaghaVal native_fseek(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = fseek(params[0].ptrvoid, params[1].int64, params[2].int32) };
}

/** long int ftell(FILE *stream); */
static union TaghaVal native_ftell(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int64 = ftell(params[0].ptrvoid) };
}

/** void rewind(FILE *stream); */
static union TaghaVal native_rewind(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	rewind(params[0].ptrvoid);
	return (union TaghaVal){ 0 };
}

/** void clearerr(FILE *stream); */
static union TaghaVal native_clearerr(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	clearerr(params[0].ptrvoid);
	return (union TaghaVal){ 0 };
}

/** int feof(FILE *stream); */
static union TaghaVal native_feof(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = feof(params[0].ptrvoid) };
}

/** int ferror(FILE *stream); */
static union TaghaVal native_ferror(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	return (union TaghaVal){ .int32 = ferror(params[0].ptrvoid) };
}

/** void perror(const char *str); */
static union TaghaVal native_perror(struct TaghaModule *const module, const size_t args, const union TaghaVal params[const static 1])
{
	(void)module; (void)args;
	perror(params[0].string);
	return (union TaghaVal){ 0 };
}


bool tagha_module_load_stdio_natives(struct TaghaModule *const module)
{
	const struct TaghaNative libc_stdio_natives[] = {
		{"remove", native_remove},
		{"rename", native_rename},
		{"tmpfile", native_tmpfile},
		//{"tmpnam", native_tmpnam},
		{"fprintf", native_fprintf},
		{"printf", native_printf},
		{"sprintf", native_sprintf},
		{"snprintf", native_snprintf},
		{"vprintf", native_vprintf},
		{"vfprintf", native_vfprintf},
		{"vsprintf", native_vsprintf},
		{"vsnprintf", native_vsnprintf},
		{"puts", native_puts},
		{"fopen", native_fopen},
		{"fclose", native_fclose},
		{"freopen", native_freopen},
		{"setbuf", native_setbuf},
		{"fflush", native_fflush},
		{"setvbuf", native_setvbuf},
		{"fgetc", native_fgetc},
		{"fgets", native_fgets},
		{"fputc", native_fputc},
		{"fputs", native_fputs},
		{"getc", native_getc},
		{"getchar", native_getchar},
		{"putc", native_putc},
		{"putchar", native_putchar},
		{"ungetc", native_ungetc},
		{"fread", native_fread},
		{"fwrite", native_fwrite},
		{"fseek", native_fseek},
		{"ftell", native_ftell},
		{"rewind", native_rewind},
		{"clearerr", native_clearerr},
		{"feof", native_feof},
		{"ferror", native_ferror},
		{"perror", native_perror},
		{NULL, NULL}
	};
	return module ? tagha_module_register_natives(module, libc_stdio_natives) : false;
}


int32_t _tagha_printf_write_decimal_int(const union TaghaVal value, const bool is64bit)
{
	return is64bit
		? sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIi64 "", value.int64)
		: sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIi32 "", value.int32);
}

int32_t _tagha_printf_write_decimal_uint(const union TaghaVal value, const bool is64bit)
{
	return is64bit
		? sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIu64 "", value.uint64)
		: sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIu32 "", value.uint32);
}

int32_t _tagha_printf_write_hex(const union TaghaVal value, const bool is64bit, const bool capital_letters)
{
	return is64bit
		? sprintf(&_g_tagha_output.buf[_g_tagha_output.count], capital_letters ? "%" PRIX64 "" : "%" PRIx64 "", value.uint64)
		: sprintf(&_g_tagha_output.buf[_g_tagha_output.count], capital_letters ? "%" PRIX32 "" : "%" PRIx32 "", value.uint32);
}

int32_t _tagha_printf_write_octal(const union TaghaVal value, const bool is64bit)
{
	return is64bit
		? sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIo64 "", value.uint64)
		: sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%" PRIo32 "", value.uint32);
}

int32_t _tagha_printf_write_float(const union TaghaVal value, const bool is64bit)
{
	return sprintf(&_g_tagha_output.buf[_g_tagha_output.count], "%f", is64bit ? value.float64 : value.float32);
}


int32_t _tagha_gen_printf(const char *restrict fmt, size_t currarg, const union TaghaVal params[const restrict], const size_t paramsize)
{
#define FLAG_LONG    1
#define FLAG_FLTLONG 2
	int32_t chars_written = 0;
	for( ; *fmt ; fmt++ ) {
		if( currarg>paramsize )
			break;
		uint_least8_t flags = 0;
		if( *fmt=='%' ) {
printf_loop_restart:
			fmt++;
			switch( *fmt ) {
				case 'l':
					flags |= FLAG_LONG;
					/** jumping from here so we can increment the char ptr for additional reading. */
					goto printf_loop_restart;
				case 'L':
					flags |= FLAG_FLTLONG;
					goto printf_loop_restart;
				/** print int */
				case 'i': case 'd':
					chars_written += _tagha_printf_write_decimal_int(params[currarg++], flags & FLAG_LONG);
					break;
				/** print uint */
				case 'u':
					chars_written += _tagha_printf_write_decimal_uint(params[currarg++], flags & FLAG_LONG);
					break;
				/** print hex */
				case 'x': case 'X':
					chars_written += _tagha_printf_write_hex(params[currarg++], flags & FLAG_LONG, *fmt=='X');
					break;
				/** print octal */
				case 'o':
					chars_written += _tagha_printf_write_octal(params[currarg++], flags & FLAG_LONG);
					break;
				/** print ptr */
				case 'p': case 'P':
					chars_written += _tagha_printf_write_hex(params[currarg++], true, *fmt=='P');
					break;
				/** print float64_t */
				case 'f': case 'F':
					chars_written += _tagha_printf_write_float(params[currarg++], true);
					break;
				case 's': {
					const char *s = params[currarg++].string;
					const size_t len = strlen(s);
					strncat(&_g_tagha_output.buf[_g_tagha_output.count], s, len);
					chars_written += len;
					_g_tagha_output.count += len;
					break;
				}
				case '%':
					chars_written++;
					_g_tagha_output.buf[_g_tagha_output.count++] = '%';
					break;
			}
		} else if( *fmt=='\\' ) {
			fmt++;
			switch( *fmt ) {
				case 'a': _g_tagha_output.buf[_g_tagha_output.count++] = '\a'; chars_written++; break;
				case 'r': _g_tagha_output.buf[_g_tagha_output.count++] = '\r'; chars_written++; break;
				case 'b': _g_tagha_output.buf[_g_tagha_output.count++] = '\b'; chars_written++; break;
				case 't': _g_tagha_output.buf[_g_tagha_output.count++] = '\t'; chars_written++; break;
				case 'v': _g_tagha_output.buf[_g_tagha_output.count++] = '\v'; chars_written++; break;
				case 'n': _g_tagha_output.buf[_g_tagha_output.count++] = '\n'; chars_written++; break;
				case 'f': _g_tagha_output.buf[_g_tagha_output.count++] = '\f'; chars_written++; break;
				case '0': _g_tagha_output.buf[_g_tagha_output.count++] = '\0'; chars_written++; break;
			}
		} else {
			chars_written++;
			_g_tagha_output.buf[_g_tagha_output.count++] = *fmt;
		}
	}
	return chars_written;
}

