#include <string.h>
#include "circbuf.h"

static unsigned int s_start_idx = 0;
static unsigned int s_end_idx = 0;
static unsigned int s_length = 0;
static char s_parse_buffer[CIRCBUF_MAX_LENGTH];

int circbuf_add(char* data, int len)
{
	int space_left_before_wrap = CIRCBUF_MAX_LENGTH - s_start_idx;

	if (s_length + len > CIRCBUF_MAX_LENGTH)
	{
		return 0;
	}

	if (len > space_left_before_wrap)
	{
		strncpy(s_parse_buffer + s_start_idx, data, space_left_before_wrap);
		strncpy(s_parse_buffer, data + space_left_before_wrap,
				len - space_left_before_wrap);
	}
	else
	{
		strncpy(s_parse_buffer + s_start_idx, data, len);
	}
	s_end_idx += len % CIRCBUF_MAX_LENGTH;
	s_length += len;

	return len;
}

int circbuf_remove(int len)
{
	if (len > s_length)
	{
		len = s_length;
	}
	s_start_idx += len % CIRCBUF_MAX_LENGTH;

	return len;
}

char circbuf_pop(void)
{
	char c = s_parse_buffer[s_start_idx];
	circbuf_remove(1);
	return c;
}

int circbuf_len(void)
{
	return s_length;
}
