/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2015 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _NV_GPU_INFO_H_
#define _NV_GPU_INFO_H_

typedef struct {
    NvU32 gpu_id;

    struct {
        NvU32 domain;
        NvU8  bus, slot, function;
    } pci_info;

    /*
     * TODO remove os_dev_ptr after the change that added os_device_ptr
     * propagates to dev_a, Bug 200517921.
     * opaque OS-specific pointer; on Linux, this is a pointer to the
     * 'struct pci_dev' for the GPU.
     */
    void *os_dev_ptr;

    /*
     * opaque OS-specific pointer; on Linux, this is a pointer to the
     * 'struct device' for the GPU.
     */
    void *os_device_ptr;
} nv_gpu_info_t;

#define NV_MAX_GPUS 32

#endif /* _NV_GPU_INFO_H_ */
