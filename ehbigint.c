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

	j = 0;
	i = 0;

	bi->bytes_used = 0;
	while (j < str_len) {
		low = str[j++];
		if (j < str_len) {
			high = low;
			low = str[j++];
		} else {
			high = '0';
		}

		if (i > bi->bytes_len) {
			/* not enough room in bi->bytes */
			return 8;
		}
		if (from_hex(&(bi->bytes[i++]), high, low)) {
			return 9;
		}
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
	if (buf_len < (bi->bytes_used + 3)) {
		return 12;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = 0; i < bi->bytes_used; ++i) {
		if (j + 2 > buf_len) {
			return 13;
		}
		err = to_hex(bi->bytes[i], buf + j, buf + j + 1);
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

int ehbi_add(struct ehbigint *res, struct ehbigint *bi1, struct ehbigint *bi2)
{
	size_t i, j;
	int a, b, c;
	struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		return 15;
	}

	if (bi1->bytes_used < bi2->bytes_used) {
		tmp = bi1;
		bi1 = bi2;
		bi2 = tmp;
	}

	c = 0;
	for (i = 1; i <= bi1->bytes_used; ++i) {
		j = bi1->bytes_used - i;
		a = bi1->bytes[j];
		b = (bi2->bytes_used < i) ? 0 : bi2->bytes[j];
		c = c + a + b;
		if (res->bytes_used + 1 > res->bytes_len) {
			return 15;
		}
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}
	while (c) {
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}
	if (res->bytes_used != res->bytes_len) {
		for (i = 0; i < res->bytes_len; ++i) {
			if (i < res->bytes_used) {
				res->bytes[i] =
				    res->bytes[i + res->bytes_len -
					       res->bytes_used];
			} else {
				res->bytes[i] = 0;
			}
		}
	}

	return 0;
}
