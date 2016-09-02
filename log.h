#ifndef LOG_H_
#define LOG_H_

#define LOG(LVL, FMT, ...) log_log(LVL, "[%s]"FMT, MODULE_NAME, __VA_ARGS__)
#define ALL(FMT, ...) LOG(LOGGER_ALL, FMT, __VA_ARGS__)
#define ERR(FMT, ...) LOG(LOGGER_ERROR, FMT, __VA_ARGS__)
#define WRN(FMT, ...) LOG(LOGGER_WARN, FMT, __VA_ARGS__)
#define INF(FMT, ...) LOG(LOGGER_INFO, FMT, __VA_ARGS__)
#define DBG(FMT, ...) LOG(LOGGER_DEBUG, FMT, __VA_ARGS__)

enum {
	LOGGER_ALL=-1,
	LOGGER_SILENT=0,
	LOGGER_ERROR,
	LOGGER_WARN,
	LOGGER_INFO,
	LOGGER_DEBUG
};

void log_file_set(char *filename);
void log_cverb_set(int lvl);
void log_fverb_set(int lvl);
void log_color_set(bool t);
void log_prettytime_set(bool t);
char *log_color_get4lvl(int lvl);
void log_log(int lvl, const char *fmt, ...);

#endif
