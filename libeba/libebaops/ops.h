#include <stdio.h>
#include <stdlib.h>

/** The following structs will be in the BSS segment */
struct eba_device_ops *devops;
struct eba_shared_ops *shmops;

/** This still need work, here is a basic outline */
struct eba_device_ops {
	int		(*post_send)(void);
	int		(*post_recv)(void);
	void 	*(sync_events)(void);
	void	*(* create_pool)(void);
	void 	*(* destroy_pool)(void);
	void 	*(*_compat_reg_mr)(void);
	void 	*(*_compat_rereg_mr)(void);
	void 	*(*_compat_dereg_mr)(void);
	void 	*(*_compat_cq_event)(void);
	void 	*(*_compat_alloc_pd)(void);
	void 	*(*_compat_dealloc_pd)(void);
};

struct eba_shared_ops {
	int		(*post_send)(void);
	int		(*post_recv)(void);
	void 	*(sync_events)(void);
	void	*(* create_pool)(void);
	void 	*(* destroy_pool)(void);
	void 	*(*_compat_reg_mr)(void);
	void 	*(*_compat_rereg_mr)(void);
	void 	*(*_compat_dereg_mr)(void);
	void 	*(*_compat_cq_event)(void);
	void 	*(*_compat_alloc_pd)(void);
	void 	*(*_compat_dealloc_pd)(void);
};
