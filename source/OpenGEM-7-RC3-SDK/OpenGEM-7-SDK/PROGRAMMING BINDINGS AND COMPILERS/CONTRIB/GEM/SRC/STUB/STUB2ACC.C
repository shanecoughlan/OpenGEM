
#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *fp;

	if (argc < 2)
	{
		fprintf(stderr,"Syntax is %s accfile\n", argv[0]);
		exit(1);
	}
	fp = fopen(argv[1], "r+b");
	if (!fp)
	{
		perror(argv[1]);
		exit(1);
	}
	fseek(fp, 512, SEEK_SET);	/* Seek past exe header */
	fputc(0xE9, fp);
	fputc(0x53, fp);
	fputc(0x00, fp);	/* JMP 0056 */
	fclose(fp);

	return 0;
}