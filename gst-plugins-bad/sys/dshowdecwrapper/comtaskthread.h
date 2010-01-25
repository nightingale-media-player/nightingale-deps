/* GStreamer
 * Copyright (C) 2009 Pioneers of the Inevitable <songbird@songbirdnest.com>
 *
 * Authors: Michael Smith <msmith@songbirdnest.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __COMTASKTHREAD_H__
#define __COMTASKTHREAD_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

G_BEGIN_DECLS

typedef void (* TaskFunc)(gpointer arg, gpointer ret);

typedef struct {
  GThread *thread;

  /* this lock is used to ensure only one thread is running at a time */
  GMutex *lock;

  /* this lock is used to ensure only one task is running at a time */
  GMutex *task_lock;

  /* this condition is signalled to wake the calling thread */
  GCond *cond;

  /* This condition is signalled to wake the task thread */
  GCond *thread_cond;

  gint running;

  TaskFunc func;
  void *func_arg;
  void *func_ret;

  gboolean comIsInitialized;
} GstCOMTaskThread;

GstCOMTaskThread * gst_comtaskthread_init(void);
void gst_comtaskthread_destroy(GstCOMTaskThread *task);
void gst_comtaskthread_do_task (GstCOMTaskThread *task,
    TaskFunc func, void *arg, void *ret);


G_END_DECLS

#endif /* __COMTASKTHREAD_H__ */
