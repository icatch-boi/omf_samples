
#include "omf_api.h"

int main(){
	omfInit((void*)0);
    omfCommand("shm0_cmd", "cmd=appUpgradeProc,operation=2", (void*)0);
	omfUninit((void*)0);
    return 0;
}

