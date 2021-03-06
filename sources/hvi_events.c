#include <linux/slab.h>

#include "hypervisor_introspection.h"

extern struct hvi_event_callback global_event_callbacks[];

int hvi_handle_event_cr(__u16 cr, unsigned long old_value, unsigned long new_value, int* allow);
int hvi_handle_event_msr(__u32 msr, __u64 old_value, __u64 new_value, int* allow);
int hvi_handle_event_vmcall(void);

static int hvi_report_event(hv_event_e event, void* data, int size, int *allow);

int hvi_handle_event_cr(__u16 cr, unsigned long old_value, unsigned long new_value, int *allow)
{
	struct hvi_event_cr cr_event;
	
	cr_event.cr = cr;
	
	cr_event.old_value = old_value;
	
	cr_event.new_value = new_value;
	
	return hvi_report_event(cr_write, (void*)&cr_event, sizeof(struct hvi_event_cr), allow);
}

int hvi_handle_event_msr(__u32 msr, __u64 old_value, __u64 new_value, int* allow)
{
	struct hvi_event_msr msr_event;
	
	msr_event.msr = msr;
	
	msr_event.old_value = old_value;
	
	msr_event.new_value = new_value;
	
	return hvi_report_event(msr_write, (void*)&msr_event, sizeof(struct hvi_event_msr), allow);
}

int hvi_handle_event_vmcall(void)
{
	int allow;
	
	return hvi_report_event(vmcall, NULL, 0, &allow);
}

static int hvi_report_event(hv_event_e event, void* data, int size, int *allow)
{
	if (global_event_callbacks[event].callback != NULL)
	{
		global_event_callbacks[event].callback(event, (unsigned char*)data, size, allow);
		return 0;
	}
		
	return -1;
}
