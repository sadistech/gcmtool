#include <stdio.h>
#include <stdlib.h>

#include <pam.h>

int main(int argc, char **argv) {
	struct pam inpam;
	unsigned int row;

	pnm_init(&argc, argv);

	FILE *ifile = NULL;

	if (!(fopen(ifile, "test.ppm", "r"))) {
		perror("eek!\n");
		exit(1);
	}

	pnm_readpaminit(ifile, &inpam, PAM_STRUCT_SIZE(tuple_type));

	printf("width: %d\n", inpam.width);
	printf("height: %d\n", inpam.height);
	
	fclose(ifile);
}
