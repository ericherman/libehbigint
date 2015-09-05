#include "ehbigint.h"

#include <stdio.h>

static int nibble_to_hex(unsigned char nibble, char *c)
{
	if (c == 0) {
		return 1;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		return 2;
	}
	return 0;
}

static int to_hex(unsigned char byte, char *high, char *low)
{
	int err;

	err = 0;
	err += nibble_to_hex((byte & 0xF0) >> 4, high);
	err += nibble_to_hex((byte & 0x0F), low);

	return err;
}

static int from_hex_nibble(unsigned char *nibble, char c)
{

	if (nibble == 0) {
		return 3;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		return 4;
	}

	return 0;
}

static int from_hex(unsigned char *byte, char high, char low)
{
	int err;
	unsigned char nibble;

	err = from_hex_nibble(&nibble, high);
	if (err) {
		return err;
	}
	*byte = (nibble << 4);

	err = from_hex_nibble(&nibble, low);
	if (err) {
		return err;
	}
	*byte += nibble;

	return 0;
}

int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
{
	size_t i, j;
	unsigned char high, low;

	if (bi == 0) {
		return 5;
	}
	if (str == 0) {
		return 6;
	}
	if (str_len == 0) {
		return 7;
	}

	/* skip over leading '0x' in string */
	if (str_len >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
		str_len -= 2;
	}

	j = str_len;
	i = 0;

	bi->bytes_used = 0;
	while (j > 0) {
		--j;
		low = str[j];
		if (j != 0) {
			--j;
			high = str[j];
		} else {
			high = '0';
		}

		if (i > bi->bytes_len) {
			/* not enough room in bi->bytes */
			return 8;
		}
		if (from_hex(&(bi->bytes[i]), high, low)) {
			return 9;
		}
		i++;
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for eazier debug */
	while (i < bi->bytes_len) {
		bi->bytes[i++] = 0;
	}

	return 0;
}

int ehbi_to_hex_string(struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		return 10;
	}
	if (buf == 0) {
		return 11;
	}
	if (buf_len < 2) {
		return 12;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_used; i > 0; --i) {
		if (j + 2 > buf_len) {
			return 13;
		}
		err = to_hex(bi->bytes[i - 1], buf + j, buf + j + 1);
		if (err) {
			return err;
		}
		j += 2;
	}
	if (j > buf_len) {
		return 14;
	}
	buf[j] = '\0';

	return 0;
}
