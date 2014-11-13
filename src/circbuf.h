#ifndef CIRCBUF_H_
#define CIRCBUF_H_

#define CIRCBUF_MAX_LENGTH 4096

/** Adds len bytes from data to the circular buffer.
 * Adds 0 bytes if buffer can't fit len more bytes.
 * Returns number of bytes added */
int circbuf_add(char* data, int len);

/** Removes len bytes from buffer. Buffer is emptied if len is larger than bytes in buffer.
 * Returns number of bytes removed */
int circbuf_remove(int len);

/** Returns and removes the oldest byte in the buffer */
char circbuf_pop(void);

/** Returns the length of data in the buffer */
int circbuf_len(void);

#endif /* CIRCBUF_H_ */
