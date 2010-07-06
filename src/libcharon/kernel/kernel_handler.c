/*
 * Copyright (C) 2010 Tobias Brunner
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "kernel_handler.h"

#include <hydra.h>
#include <daemon.h>
#include <processing/jobs/acquire_job.h>

typedef struct private_kernel_handler_t private_kernel_handler_t;

/**
 * Private data of a kernel_handler_t object.
 */
struct private_kernel_handler_t {

	/**
	 * Public part of kernel_handler_t object.
	 */
	kernel_handler_t public;

};

METHOD(kernel_listener_t, acquire, bool,
	   private_kernel_handler_t *this, u_int32_t reqid,
	   traffic_selector_t *src_ts, traffic_selector_t *dst_ts)
{
	job_t *job;
	if (src_ts && dst_ts)
	{
		DBG1(DBG_KNL, "creating acquire job for policy %R === %R "
					  "with reqid {%u}", src_ts, dst_ts, reqid);
	}
	else
	{
		DBG1(DBG_KNL, "creating acquire job for policy with reqid {%u}", reqid);
	}
	job = (job_t*)acquire_job_create(reqid, src_ts, dst_ts);
	hydra->processor->queue_job(hydra->processor, job);
	return TRUE;
}

METHOD(kernel_handler_t, destroy, void,
	   private_kernel_handler_t *this)
{
	charon->kernel_interface->remove_listener(charon->kernel_interface,
											  &this->public.listener);
	free(this);
}

kernel_handler_t *kernel_handler_create()
{
	private_kernel_handler_t *this;

	INIT(this,
		.public = {
			.listener = {
				.acquire = _acquire,
			},
			.destroy = _destroy,
		},
	);

	charon->kernel_interface->add_listener(charon->kernel_interface,
										   &this->public.listener);

	return &this->public;
}

