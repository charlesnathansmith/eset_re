#include <iostream>
#include "unkhash2.h"

void unkhash2_sub1(uint64_t *out, uint8_t page, uint8_t r9b)
{
	printf("unkhash2_sub1 %.2X %.2X\n", page, r9b);

	size_t section = (r9b <= 0x0f) ? r9b : 0x10 - (r9b & 0x0f);
	if (section > 8) section = 0;

	uint64_t buf[12];
	memset(buf, 0, sizeof(buf));

	if (!section)
	{
		buf[0] = buf[4] = 1;
		printf("None\n");
	}
	else
	{
		printf("page %d, section %d\nK\n", page, section);
		const uint64_t *K = unkhash2_sub1_k[page][section - 1];

		for (size_t i = 0; i < 12; i++)
		{
			printf("%llx ", K[i]);
			buf[i] = K[i];
		}
		puts("\nOut");
	}

	out[10] = buf[8];
	out[11] = (buf[8] >> 0x33) | (buf[9] << 0xd);
	out[12] = (buf[10] << 0x1a) | (buf[9] >> 0x26);
	out[13] = (buf[11] << 0x27) | (buf[10] >> 0x19);
	out[14] = buf[11] >> 0xc;

	for (size_t i = 10; i < 15; i++)
		out[i] &= 0x7ffffffffffff;

	if (r9b >> 7)
	{
		out[0] = buf[4];
		out[1] = (buf[4] >> 0x33) | (buf[5] << 0xd);
		out[2] = (buf[6] << 0x1a) | (buf[5] >> 0x26);
		out[3] = (buf[7] << 0x27) | (buf[6] >> 0x19);
		out[4] = buf[7] >> 0xc;
		out[5] = buf[0];
		out[6] = (buf[1] << 0xd) | (buf[0] >> 0x33);
		out[7] = (buf[2] << 0x1a) | (buf[1] >> 0x26);
		out[8] = (buf[3] << 0x27) | (buf[2] >> 0x19);
		out[9] = buf[3] >> 0xc;

		uint64_t buf2[4];
		uint64_t tmp = 0xfffffffffffda - (buf[8] & 0x7ffffffffffff);

		for (size_t i = 0; i < 4; i++)
		{
			buf2[i] = tmp;
			tmp = (tmp >> 0x33) - out[11 + i] + 0x0FFFFFFFFFFFFE;
		}

		out[10] = (tmp >> 0x33) * 0x13 + buf2[0];
		out[11] = buf2[1];
		out[12] = buf2[2];
		out[13] = buf2[3];
		out[14] = tmp;

		for (size_t i = 10; i < 15; i++)
			out[i] &= 0x7ffffffffffff;
	}
	else
	{
		out[0] = buf[0];
		out[1] = (buf[1] << 0xd) | (buf[0] >> 0x33);
		out[2] = (buf[2] << 0x1a) | (buf[1] >> 0x26);
		out[3] = (buf[3] << 0x27) | (buf[2] >> 0x19);
		out[4] = buf[3] >> 0xc;
		out[5] = buf[4];
		out[6] = (buf[4] >> 0x33) | (buf[5] << 0xd);
		out[7] = (buf[6] << 0x1a) | (buf[5] >> 0x26);
		out[8] = (buf[7] << 0x27) | (buf[6] >> 0x19);
		out[9] = buf[7] >> 0xc;
	}

	for (size_t i = 0; i < 10; i++)
		out[i] &= 0x7ffffffffffff;

	for (size_t i = 0; i < 15; i++)
		printf("%llx ", out[i]);
	
	putchar('\n');
	getchar();
}

int main()
{
	uint64_t out[15];

	// All possible input values
	for (size_t j = 0; j < 0x20; j++)
	{
		for (size_t i = 0; i < 0x10; i++)
			unkhash2_sub1(out, j, i);

		for (size_t i = 0; i < 0x10; i++)
			unkhash2_sub1(out, j, 0xF0 | i);
	}

	return 0;
}
