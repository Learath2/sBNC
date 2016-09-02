#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "util.h"
#include "log.h"

#define RED 1
#define GRN 2
#define YEL 3
#define BLU 4
#define MAG 5
#define CYN 6
#define WHT 7
#define RST 0

char g_colors[8][8] = {"\x1B[0m", "\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m"};

int g_cverb = LOGGER_INFO;
char *g_file = NULL;
int g_fverb = LOGGER_ERROR;
bool g_prettytime = false;
bool g_color = false;

void log_file_set(char *filename)
{
	g_file = util_strdup(filename);
}

void log_cverb_set(int lvl)
{
	g_cverb = lvl;
}

void log_fverb_set(int lvl)
{
	g_fverb = lvl;
}

void log_color_set(bool t)
{
	g_color = t;
}

void log_prettytime_set(bool t)
{
	g_prettytime = t;
}

char *log_color_get4lvl(int lvl)
{
	switch (lvl){
		case LOGGER_ERROR:
			return g_colors[RED];
		case LOGGER_WARN:
			return g_colors[YEL];
		default:
			return g_colors[RST];
	}
}

void log_log(int lvl, const char *fmt, ...)
{
	char data[2048];
	va_list vl;
	va_start(vl, fmt);
	vsnprintf(data, sizeof data, fmt, vl);
	va_end(vl);

	char result[2048];
	if(g_prettytime){
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		strftime(result, sizeof result, "[%d/%m/%y %H:%M:%S]", tm);
		strcat(result, data);
	}
	else
		sprintf(result, "[%ld]%s\n", time(NULL), data);

	if(lvl < g_cverb){
		FILE *str = (lvl < LOGGER_INFO) ? stderr : stdout;
		if(g_color) fputs(log_color_get4lvl(lvl), str);
		fputs(result, str);
		if(g_color) fputs(g_colors[RST], str);
	}
	if(g_file && lvl < g_fverb){
		FILE *f = fopen(g_file, "a");
		fputs(result, f);
		fclose(f);
	}
}
