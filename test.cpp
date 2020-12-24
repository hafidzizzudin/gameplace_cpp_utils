#include <stdio.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	//int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	int mib[4];
	int numCPU;
	size_t len = sizeof(numCPU);
	
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;

	sysctl(mib, 2, &numCPU, &len, NULL, 0);
	
	if(numCPU < 1)
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);
		if(numCPU < 1)
			numCPU = 1;
	}

	printf("ncpu: %d\n", numCPU);
	return 0;
}
