#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>
#include <pam.h>

int main(int argc, char **argv) {
	struct pam inpam;
	unsigned int row;

	pnm_init(&argc, argv);

	FILE *ifile = NULL;

	if (!(ifile = fopen("test.ppm", "r"))) {
		perror("eek!\n");
		exit(1);
	}

	pnm_readpaminit(ifile, &inpam, PAM_STRUCT_SIZE(inpam.tuple_type));

	printf("width: %d\n", inpam.width);
	printf("height: %d\n", inpam.height);
	
	fclose(ifile);
}
