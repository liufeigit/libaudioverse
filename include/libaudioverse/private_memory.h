/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "private_threads.h"
#include "libaudioverse.h"
#ifdef _cplusplus
extern "c" {
#endif

typedef void (*LavFreeingFunction)(void*);

Lav_PUBLIC_FUNCTION void* createMmanager();
Lav_PUBLIC_FUNCTION void freeMmanager(void* manager);

//same as stdlib.
Lav_PUBLIC_FUNCTION void* mmanagerAlloc(void* manager, unsigned int size);
Lav_PUBLIC_FUNCTION void* mmanagerCalloc(void* manager, unsigned int elements, unsigned int size);
Lav_PUBLIC_FUNCTION void mmanagerFree(void* manager, void* ptr);

//Declare ownership of a pointer, provided an appropriate function to free it.
//This is incredibly useful for file handles.  File handles are the whole reason for this function.
//it also makes internal operations easier.
Lav_PUBLIC_FUNCTION LavError mmanagerAssociatePointer(void *manager, void* ptr, LavFreeingFunction freer);

#ifdef _cplusplus
}
#endif