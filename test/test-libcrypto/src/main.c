#include <stdio.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

int test_sha256(void)
{
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	if (mdctx == NULL) {
		fprintf(stderr, "EVP_MD_CTX_new failed\n");
		return -1;
	}

	char const msg[] = "Hello Genode";
	int result = -1;

	unsigned char out[EVP_MAX_MD_SIZE];
	unsigned int  out_len;

	do {
		if (!EVP_DigestInit_ex2(mdctx, EVP_sha256(), NULL)) {
			fprintf(stderr, "EVP_DigestInit_ex2 failed\n");
			break;
		}
		if (!EVP_DigestUpdate(mdctx, msg, sizeof(msg) - 1 /* omit '\0' */)) {
			fprintf(stderr, "EVP_DigestUpdate failed\n");
			break;
		}
		if (!EVP_DigestFinal_ex(mdctx, out, &out_len)) {
			fprintf(stderr, "EVP_DigestFinal_ex failed\n");
			break;
		}

		result = 0;
	} while (0);

	EVP_MD_CTX_free(mdctx);

	if (result)
		return result;

	printf("SHA-256 '%s' ", msg);
	for (unsigned i = 0; i < out_len; i++)
		printf("%02x", out[i]);
	printf("\n");

	return 0;
}


int test_rand(void)
{
	unsigned char rand_buffer[64];
	bzero(rand_buffer, sizeof(rand_buffer));

	int const result = RAND_bytes(rand_buffer, sizeof(rand_buffer));
	if (result != 1) {
		fprintf(stderr, "RAND_bytes: failed: %d\n", result);
		return 1;
	}
	/*
	 * Only print failure case as printing random bytes is somewhat
	 * futile. The libc will issue warnings in case RNG or RTC is
	 * not configured.
	 */
	return 0;
}


int main(int argc, char *argv[])
{
	if (test_sha256())
		return -1;

	if (test_rand())
		return -1;

	return 0;
}
