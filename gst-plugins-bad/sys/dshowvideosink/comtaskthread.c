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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <windows.h>

#include <gst/gst.h>

#include "comtaskthread.h"

static void
comtaskthread_com_initialize (gpointer arg, gpointer ret)
{
  GstCOMTaskThread *task = (GstCOMTaskThread *)arg;
  gboolean *result = (gboolean *)ret;

  HRESULT hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);

  if (hr == S_OK) {
    task->comIsInitialized = TRUE;
    *result = TRUE;
  }
  else if (SUCCEEDED (hr)) {
    GST_WARNING ("COM was already initialized on this thread!");
    task->comIsInitialized = TRUE;
    *result = TRUE;
  }
  else {
    GST_WARNING ("Failed to initialize COM");
    task->comIsInitialized = FALSE;
    *result = FALSE;
  }
}

static void
comtaskthread_com_uninitialize (gpointer arg, gpointer ret)
{
  GstCOMTaskThread *task = (GstCOMTaskThread *)arg;

  if (task->comIsInitialized) {
    CoUninitialize ();
    task->comIsInitialized = FALSE;
  }
}

static gpointer
comtaskthread_thread (gpointer data)
{
  GstCOMTaskThread *task = (GstCOMTaskThread *)data;

  g_mutex_lock (task->lock);
  while (g_atomic_int_get (&task->running)) {

    if (task->func) {
      task->func (task->func_arg, task->func_ret);

      /* Tell caller that we're done */
      g_cond_signal (task->cond);
    }

    g_cond_wait (task->thread_cond, task->lock);
  }
  g_cond_broadcast (task->cond);
  g_mutex_unlock (task->lock);

  return NULL;
}

GstCOMTaskThread *
gst_comtaskthread_init(void)
{
  GstCOMTaskThread *task = g_new0(GstCOMTaskThread, 1);
  gboolean *ret;

  task->lock = g_mutex_new ();
  task->cond = g_cond_new ();
  task->thread_cond = g_cond_new ();

  g_atomic_int_set (&task->running, 1); 
  task->thread = g_thread_create (comtaskthread_thread, task, TRUE, NULL);

  if (!task->thread)
    goto fail;

  gst_comtaskthread_do_task (task, comtaskthread_com_initialize, task, &ret);
  if (!ret) {
    GST_WARNING ("Failed to initialize COM");
    goto fail;
  }

  return task;
fail:
  g_free (task);
  return NULL;
}

void
gst_comtaskthread_destroy(GstCOMTaskThread *task)
{
  gpointer ret;
  gst_comtaskthread_do_task (task, comtaskthread_com_uninitialize, task, NULL);

  task->running = FALSE;
  g_mutex_lock (task->lock);
  g_cond_signal (task->thread_cond);
  g_cond_wait (task->cond, task->lock);

  g_thread_join (task->thread);

  g_mutex_free (task->lock);
  g_cond_free (task->cond);
  g_cond_free (task->thread_cond);

  g_free (task);
}

void
gst_comtaskthread_do_task (GstCOMTaskThread *task, TaskFunc func, void *arg,
    void *ret)
{
  g_mutex_lock (task->lock);

  /* Set the function to run, and then wake up the thread to run it */
  task->func = func;
  task->func_arg = arg;
  task->func_ret = ret;

  g_cond_signal (task->thread_cond);

  /* Wait until the task thread signals that it is done */
  g_cond_wait (task->cond, task->lock);

  g_mutex_unlock (task->lock);
}

