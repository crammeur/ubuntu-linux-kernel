
/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */


#include "nv-misc.h"
#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"
#include "rmil.h"

irqreturn_t nv_gvi_kern_isr(
    int   irq,
    void *arg
)
{
    nv_linux_state_t *nvl = (void *) arg;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU32 need_to_run_bottom_half = 0;
    BOOL ret = TRUE;

    ret = rm_gvi_isr(nvl->sp[NV_DEV_STACK_ISR], nv, &need_to_run_bottom_half);
    if (need_to_run_bottom_half && !(nv->flags & NV_FLAG_SUSPENDED))
    {
        schedule_work(&nvl->work.task);
    }

    return IRQ_RETVAL(ret);
}

void nv_gvi_kern_bh(
    struct work_struct *data
)
{
    nv_state_t *nv;
    nv_work_t *work = container_of(data, nv_work_t, task);
    nv_linux_state_t *nvl = (nv_linux_state_t *)work->data;
    nv  = NV_STATE_PTR(nvl);

    rm_gvi_bh(nvl->sp[NV_DEV_STACK_ISR_BH], nv);
}

#if defined(CONFIG_PM)
NV_STATUS
nv_gvi_suspend(
    nv_state_t *nv,
    nv_pm_action_t pm_action
)
{
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_shutdown_gvi_device(sp, nv);
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVGVI: failed to stop gvi!\n");
        goto failed;
    }

    flush_scheduled_work();

    status = rm_gvi_suspend(sp, nv);
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVGVI: failed to suspend gvi!\n");
    }

failed:
    nv_kmem_cache_free_stack(sp);
    return status;
}

NV_STATUS
nv_gvi_resume(
    nv_state_t *nv,
    nv_pm_action_t pm_action
)
{
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gvi_resume(sp, nv);

    nv_kmem_cache_free_stack(sp);
    return status;
}
#endif /* defined(CONFIG_PM) */
