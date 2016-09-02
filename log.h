#ifndef LOG_H_
#define LOG_H_

#define LOG(LVL, MSG, ...) log_log(LVL, "[%s]"MSG, MODULE_NAME)
#define ALL(MSG, ...) LOG(LOGGER_ALL, MSG)
#define ERR(MSG, ...) LOG(LOGGER_ERROR, MSG)
#define WRN(MSG, ...) LOG(LOGGER_WARN, MSG)
#define INF(MSG, ...) LOG(LOGGER_INFO, MSG)
#define DBG(MSG, ...) LOG(LOGGER_DEBUG, MSG)

#define LOGF(LVL, FMT, ...) log_log(LVL, "[%s]"FMT, MODULE_NAME, __VA_ARGS__)
#define ALLF(FMT, ...) LOG(LOGGER_ALL, FMT, __VA_ARGS__)
#define ERRF(FMT, ...) LOG(LOGGER_ERROR, FMT, __VA_ARGS__)
#define WRNF(FMT, ...) LOG(LOGGER_WARN, FMT, __VA_ARGS__)
#define INFF(FMT, ...) LOG(LOGGER_INFO, FMT, __VA_ARGS__)
#define DBGF(FMT, ...) LOG(LOGGER_DEBUG, FMT, __VA_ARGS__)

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
