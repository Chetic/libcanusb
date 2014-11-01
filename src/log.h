/** Maximum string length per log_write call (after formatting) */
#define LOG_BUFFER_SIZE 1024

/** Open log.txt for logging. Always call before log_write */
extern int log_init(void);

/** Close log.txt. Always call before shutting down */
extern void log_close(void);

/** printf-wrapper that writes to log.txt and stdout */
extern void log_write(const char* str, ...);
