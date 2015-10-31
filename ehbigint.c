#include "ehbigint.h"

#include <string.h>
#include <execinfo.h>

static int nibble_to_hex(unsigned char nibble, char *c)
{
	if (c == 0) {
		EHBI_LOG_ERROR0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		EHBI_LOG_ERROR1("Bad input '%x'", nibble);
		return EHBI_BAD_INPUT;
	}
	return EHBI_SUCCESS;
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
		EHBI_LOG_ERROR0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		EHBI_LOG_ERROR1("Not hex (%c)", c);
		return EHBI_NOT_HEX;
	}

	return EHBI_SUCCESS;
}

static int from_hex(unsigned char *byte, char high, char low)
{
	int err;
	unsigned char nibble;

	err = from_hex_nibble(&nibble, high);
	if (err) {
		EHBI_LOG_ERROR1("Error with high nibble (%c)", high);
		return EHBI_BAD_HIGH_NIBBLE;
	}
	*byte = (nibble << 4);

	err = from_hex_nibble(&nibble, low);
	if (err) {
		EHBI_LOG_ERROR1("Error with low nibble (%c)", high);
		return EHBI_BAD_LOW_NIBBLE;
	}
	*byte += nibble;

	return EHBI_SUCCESS;
}

int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
{
	size_t i, j;
	unsigned char high, low;

	if (bi == 0) {
		EHBI_LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		EHBI_LOG_ERROR0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}
	if (str == 0) {
		EHBI_LOG_ERROR0("Null string");
		return EHBI_NULL_STRING;
	}
	if (str_len == 0 || str[0] == 0) {
		EHBI_LOG_ERROR0("Zero length string");
		return EHBI_ZERO_LEN_STRING;
	}

	/* ignore characters starting with the first NULL in string */
	for (i = 1; i < str_len; ++i) {
		if (str[i] == 0) {
			str_len = i;
			break;
		}
	}

	/* skip over leading '0x' in string */
	if (str_len >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
		str_len -= 2;
	}

	j = str_len;
	i = bi->bytes_len;

	bi->bytes_used = 0;
	while (j > 0) {
		low = str[--j];
		if (j > 0) {
			high = str[--j];
		} else {
			high = '0';
		}
		if (bi->bytes_used >= bi->bytes_len) {
			EHBI_LOG_ERROR0("byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		if (from_hex(&(bi->bytes[--i]), high, low)) {
			EHBI_LOG_ERROR2("Bad data (high: %c, low: %c)", high,
					low);
			return EHBI_BAD_DATA;
		}
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for easier debug */
	while (i-- > 0) {
		bi->bytes[i] = (bi->bytes[i + 1] > 0x7F) ? 0xFF : 0;
	}

	return EHBI_SUCCESS;
}

int ehbi_to_hex_string(struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		EHBI_LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (buf == 0) {
		EHBI_LOG_ERROR0("Null buffer");
		return EHBI_NULL_STRING_BUF;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			EHBI_LOG_ERROR0("Buffer too small, partially written");
			return EHBI_STRING_BUF_TOO_SMALL_PARTIAL;
		}
		err = to_hex(bi->bytes[i], buf + j, buf + j + 1);
		if (err) {
			EHBI_LOG_ERROR0("Corrupted data?");
			return err;
		}
		j += 2;
	}
	if (j > buf_len) {
		EHBI_LOG_ERROR0("Unable to write trailing NULL to buffer");
		return EHBI_STRING_BUF_TOO_SMALL_NO_NULL;
	}
	buf[j] = '\0';

	/* strip leading '0's ("0x0123" -> "0x123") */
	/* strip leading "00"s ("0x000123" -> "0x0123") */
	while ((buf[2] == '0' || buf[2] == 'F') && buf[2] == buf[3]
	       && buf[2] == buf[4] && buf[2] == buf[5]) {
		for (j = 2; j < buf_len - 1 && buf[j] != 0; j += 2) {
			buf[j] = buf[j + 2];
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_add(struct ehbigint *res, struct ehbigint *bi1, struct ehbigint *bi2)
{
	size_t i;
	unsigned char a, b, c;
	struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (bi1->bytes_used < bi2->bytes_used) {
		tmp = bi1;
		bi1 = bi2;
		bi2 = tmp;
	}

	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= bi1->bytes_used; ++i) {
		a = bi1->bytes[bi1->bytes_len - i];
		b = (bi2->bytes_used < i) ? 0 : bi2->bytes[bi2->bytes_len - i];
		c = c + a + b;

		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c < a) ? 1 : 0;
	}
	if (c) {
		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
	}

	return EHBI_SUCCESS;
}

int ehbi_inc(struct ehbigint *bi, struct ehbigint *val)
{
	size_t i;
	unsigned char a, b, c;

	if (bi == 0 || val == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (val->bytes_used > bi->bytes_len) {
		EHBI_LOG_ERROR0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
	}

	c = 0;
	for (i = 1; i <= val->bytes_used; ++i) {
		a = val->bytes[val->bytes_len - i];
		b = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		c = c + a + b;

		bi->bytes[bi->bytes_len - i] = c;
		if (bi->bytes_used < i) {
			bi->bytes_used = i;
		}

		c = (c < a) ? 1 : 0;
	}
	while (c) {
		if (i > bi->bytes_len) {
			EHBI_LOG_ERROR0("byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		a = c;
		b = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		c = a + b;

		bi->bytes[bi->bytes_len - i] = c;
		if (bi->bytes_used < i) {
			bi->bytes_used = i;
		}

		c = (c < a) ? 1 : 0;
		++i;
	}

	return EHBI_SUCCESS;
}

int ehbi_inc_ul(struct ehbigint *bi, unsigned long val)
{
	size_t i, j;
	unsigned char c;
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	temp.bytes_used = sizeof(unsigned long);

	for (i = 0; i < temp.bytes_len; ++i) {
		c = (val >> (8 * i));
		j = (temp.bytes_len - 1) - i;
		temp.bytes[j] = c;
	}
	for (i = 0; i < temp.bytes_len; ++i) {
		if (temp.bytes[i] != 0x00) {
			break;
		}
	}
	temp.bytes_used = temp.bytes_len - i;

	return ehbi_inc(bi, &temp);
}

int ehbi_dec(struct ehbigint *bi, struct ehbigint *val)
{
	size_t i, j;
	unsigned char a, b, c;

	if (bi == 0 || val == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	c = 0;
	for (i = 1; i <= val->bytes_used; ++i) {
		a = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		b = val->bytes[val->bytes_len - i];
		c = a - b;

		bi->bytes[bi->bytes_len - i] = c;

		j = i;
		while (c > a) {
			if ((bi->bytes_len - j) == 0) {
				EHBI_LOG_ERROR0("byte[] too small for borrow");
				return EHBI_BYTES_TOO_SMALL_FOR_BORROW;
			}
			bi->bytes[bi->bytes_len - (j + 1)] -= 1;
			if (bi->bytes[bi->bytes_len - (j + 1)] == 0xFF) {
				c = 1;
				a = 0;
			} else {
				c = 0;
				a = 0;
			}
			++j;
		}
	}
	for (i = 0; i < bi->bytes_len; ++i) {
		if (bi->bytes[i] != 0x00) {
			break;
		}
	}
	bi->bytes_used = bi->bytes_len - i;

	return EHBI_SUCCESS;
}

int ehbi_subtract(struct ehbigint *res, struct ehbigint *bi1,
		  struct ehbigint *bi2)
{
	size_t i, max_len;
	unsigned char a, b, c;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (bi1->bytes_used >= bi2->bytes_used) {
		max_len = bi1->bytes_used;
	} else {
		max_len = bi2->bytes_used;
	}

	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= max_len; ++i) {
		if (bi1->bytes_used < i) {
			a = ((1 << 7) & bi1->bytes[0]) ? 0xFF : 0;
		} else {
			a = bi1->bytes[bi1->bytes_len - i];
		}
		b = (bi2->bytes_used < i) ? 0 : bi2->bytes[bi2->bytes_len - i];
		c = c + (a - b);

		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c > a) ? 0xFF : 0;
	}

	if (c) {
		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		while (i <= res->bytes_len) {
			res->bytes[res->bytes_len - i] = 0xFF;
			++i;
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_compare(struct ehbigint *bi1, struct ehbigint *bi2, int *err)
{
	size_t i;
	unsigned char a, b;

	if (bi1 == 0 || bi2 == 0 || err == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		return 0;
	}

	*err = EHBI_SUCCESS;
	if (bi1->bytes_used > bi2->bytes_used) {
		return 1;
	} else if (bi1->bytes_used < bi2->bytes_used) {
		return -1;
	}

	for (i = 0; i < bi1->bytes_used; ++i) {
		a = bi1->bytes[(bi1->bytes_len - bi1->bytes_used) + i];
		b = bi2->bytes[(bi2->bytes_len - bi2->bytes_used) + i];
		if (a > b) {
			return 1;
		} else if (a < b) {
			return -1;
		}
	}
	return 0;
}

int ehbi_equals(struct ehbigint *bi1, struct ehbigint *bi2, int *err)
{
	return ((ehbi_compare(bi1, bi2, err) == 0) && (*err == EHBI_SUCCESS));
}

int ehbi_less_than(struct ehbigint *bi1, struct ehbigint *bi2, int *err)
{
	return ((ehbi_compare(bi1, bi2, err) < 0) && (*err == EHBI_SUCCESS));
}

int ehbi_greater_than(struct ehbigint *bi1, struct ehbigint *bi2, int *err)
{
	return ((ehbi_compare(bi1, bi2, err) > 0) && (*err == EHBI_SUCCESS));
}

int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			size_t buf_len)
{
	size_t i, j, k, hex_len;
	int err;
	unsigned char *hex_buf;

	if (dec_str == 0 || buf == 0) {
		EHBI_LOG_ERROR0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 4) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	buf[0] = '0';
	buf[1] = 'x';

	/* first operate with binary data, convert to ASCII later */
	/* start by adjusting the buf for our needs - needs to be unsigned */
	hex_buf = (unsigned char *)buf + 2;	/* skip past leading "0x" */
	hex_len = buf_len - 3;	/* and leave room for the NULL terminator */

	/* zero out the buffer */
	for (i = 0; i < hex_len; ++i) {
		hex_buf[i] = 0;
	}

	for (i = 0; i < dec_len && dec_str[i] != 0; ++i) {
		if (dec_str[i] < '0' || dec_str[i] > '9') {
			EHBI_LOG_ERROR1("Character not decimal (%c)",
					dec_str[i]);
			return EHBI_BAD_INPUT;
		}
		/* we're doing another digit, multiply previous by 10 */
		for (j = 0; j < hex_len; ++j) {
			k = hex_len - 1 - j;
			hex_buf[k] *= 10;
		}

		hex_buf[hex_len - 1] += (dec_str[i] - '0');

		/* carry */
		for (j = 0; j < hex_len; ++j) {
			k = hex_len - 1 - j;
			if (hex_buf[k] >= 16) {
				hex_buf[k - 1] += (hex_buf[k] / 16);
				hex_buf[k] = (hex_buf[k] % 16);
			}
		}
	}

	/* convert to ASCII */
	for (j = 0; j < hex_len; ++j) {
		err = nibble_to_hex(hex_buf[j], buf + 2 + j);
		if (err) {
			EHBI_LOG_ERROR0("Null char pointer");
			return EHBI_CORRUPT_DATA;
		}
	}

	/* left shift away leading zeros */
	/* first find the index (j) of the first non-zero */
	for (j = 0; hex_buf[j] == '0' && j < hex_len; ++j) {
		;
	}

	/* but work on whole bytes */
	/* since j was incremented, whole bytes of '0' will be odd */
	if (j % 2 == 0) {
		j -= 1;
	}

	/* next, shift all the contents "j" places to the left */
	for (i = 0; i < (hex_len - j); ++i) {
		hex_buf[i] = hex_buf[i + j];
	}

	/* add a trailing NULL */
	buf[buf_len - 1 - j] = '\0';

	return EHBI_SUCCESS;
}

int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			size_t buf_len)
{
	size_t i, j, k, dec_len;
	unsigned char *dec_buf;
	char ascii_offset;
	unsigned char num_val;

	if (hex == 0 || buf == 0) {
		EHBI_LOG_ERROR0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 2 || buf_len < hex_len) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	/* skip over leading '0x' in string */
	if (hex_len >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
		hex += 2;
		hex_len -= 2;
	}

	/* first operate with binary data, convert to ASCII later */
	/* start by adjusting the buf for our needs - needs to be unsigned */
	dec_buf = (unsigned char *)buf;
	dec_len = buf_len - 1;	/* leave room for the NULL terminator */

	/* zero out the buffer */
	for (i = 0; i < dec_len; ++i) {
		dec_buf[i] = 0;
	}

	for (i = 0; i < hex_len && hex[i] != 0; ++i) {
		ascii_offset = 0;
		if (hex[i] >= '0' && hex[i] <= '9') {
			ascii_offset = '0';
		} else if (hex[i] >= 'A' && hex[i] <= 'F') {
			ascii_offset = 'A';
		} else if (hex[i] >= 'a' && hex[i] <= 'f') {
			ascii_offset = 'a';
		}
		if (ascii_offset == 0) {
			EHBI_LOG_ERROR1("Character not hex (%c)", hex[i]);
			return EHBI_BAD_INPUT;
		}

		/* we're doing another digit, multiply previous by 16 */
		for (j = 0; j < dec_len; ++j) {
			k = dec_len - 1 - j;
			dec_buf[k] *= 16;
		}

		if (ascii_offset == '0') {
			num_val = (hex[i] - '0');
		} else {
			num_val = 10 + (hex[i] - ascii_offset);
		}
		dec_buf[dec_len - 1] += num_val;

		/* carry */
		for (j = 0; j < dec_len; ++j) {
			k = dec_len - 1 - j;
			if (dec_buf[k] >= 10) {
				dec_buf[k - 1] += (dec_buf[k] / 10);
				dec_buf[k] = (dec_buf[k] % 10);
			}
		}
	}

	/* convert to ASCII */
	for (j = 0; j < dec_len; ++j) {
		buf[j] = '0' + dec_buf[j];
	}

	/* left shift away leading zeros */
	/* first find the index (j) of the first non-zero */
	for (j = 0; dec_buf[j] == '0' && j < dec_len; ++j) {
		;
	}
	/* next, shift all the contents "j" places to the left */
	for (i = 0; i < (dec_len - j); ++i) {
		dec_buf[i] = dec_buf[i + j];
	}

	/* add a trailing NULL */
	buf[buf_len - 1 - j] = '\0';

	return EHBI_SUCCESS;
}

void ehbi_log_backtrace(FILE *log)
{
	void *array[4096];
	size_t size;

	size = backtrace(array, 4096);
	backtrace_symbols_fd(array, size, fileno(log));
}
