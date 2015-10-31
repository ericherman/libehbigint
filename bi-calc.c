#include "ehbigint.c"
#include <string.h>

int main(int argc, char *argv[])
{
	char buf[1024];
	char hexbuf1[1024];
	char hexbuf2[1024];
	char hexbuf3[1024];
	unsigned char bbuf1[1024];
	unsigned char bbuf2[1024];
	unsigned char bbuf3[1024];
	struct ehbigint bigint_1;
	struct ehbigint bigint_2;
	struct ehbigint bigint_3;

	if (argc < 4) {
		fprintf(stderr, "usage:\n\t%s decimal1 [+-] decimal2\n",
			argv[0]);
		return -1;
	}

	bigint_1.bytes = bbuf1;
	bigint_1.bytes_len = 1024;

	bigint_2.bytes = bbuf2;
	bigint_2.bytes_len = 1024;

	bigint_3.bytes = bbuf3;
	bigint_3.bytes_len = 1024;

	ehbi_decimal_to_hex(argv[1], strlen(argv[1]), hexbuf1, 1024);
	ehbi_decimal_to_hex(argv[3], strlen(argv[3]), hexbuf2, 1024);

	ehbi_from_hex_string(&bigint_1, hexbuf1, 1024);
	ehbi_from_hex_string(&bigint_2, hexbuf2, 1024);

	switch (*argv[2]) {
	case '+':
		ehbi_add(&bigint_3, &bigint_1, &bigint_2);
		break;
	case '-':
		ehbi_subtract(&bigint_3, &bigint_1, &bigint_2);
		break;
	default:
		fprintf(stderr, "operator '%c' not supported.\n", *argv[2]);
		return -1;
	}

	ehbi_to_hex_string(&bigint_3, hexbuf3, 1024);

	ehbi_hex_to_decimal(hexbuf1, 1024, buf, 1024);
	printf("   %40s\n", buf);
	ehbi_hex_to_decimal(hexbuf2, 1024, buf, 1024);
	printf(" %c %40s\n", *argv[2], buf);
	ehbi_hex_to_decimal(hexbuf3, 1024, buf, 1024);
	printf(" = %40s\n", buf);

	return 0;
}
