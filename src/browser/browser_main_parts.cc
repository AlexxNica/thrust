// Copyright (c) 2014 Stanislas Polu.
// Copyright (c) 2012 The Chromium Authors. 
// See the LICENSE file.

#include "src/browser/browser_main_parts.h"

#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/message_loop/message_loop.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "cc/base/switches.h"
#include "net/base/net_module.h"
#include "net/base/net_util.h"
#include "url/gurl.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/url_constants.h"
#include "content/public/browser/browser_thread.h"

#include "src/common/switches.h"
#include "src/browser/exo_browser.h"
#include "src/browser/session/exo_session.h"
#include "src/net/net_log.h"
#include "src/api/api_handler.h"
#include "src/api/exo_browser_binding.h"


using namespace content;

namespace exo_browser {

// static
ExoBrowserMainParts* ExoBrowserMainParts::self_ = NULL;


ExoBrowserMainParts::ExoBrowserMainParts()
  : system_session_(NULL)
{
  DCHECK(!self_) << "Cannot have two ExoBrowserBrowserMainParts";
  self_ = this;
}

ExoBrowserMainParts::~ExoBrowserMainParts() {
}

// static
ExoBrowserMainParts* 
ExoBrowserMainParts::Get() 
{
  DCHECK(self_);
  return self_;
}

brightray::BrowserContext* 
ExoBrowserMainParts::CreateBrowserContext() {
  if(system_session_ == NULL) {
    /* We create an off the record session to be used internally. */
    /* This session has a dummy cookie store. Stores nothing.     */
    system_session_ = new ExoSession(true, "system_session", true);
  }
  return system_session_;
}

void 
ExoBrowserMainParts::PreMainMessageLoopRun() 
{
  brightray::BrowserMainParts::PreMainMessageLoopRun();
  net_log_.reset(new ExoBrowserNetLog());

  /* TODO(spolu): Get Path form command line */
  //CommandLine* command_line = CommandLine::ForCurrentProcess();
  
  base::FilePath path;
  base::GetTempDir(&path);
  api_handler_.reset(new ApiHandler(path.Append("_exo_browser.sock")));


  api_handler_->InstallBinding("exo_browser", 
                               new ExoBrowserBindingFactory());

  api_handler_->Start();

  /*
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE, base::Bind(&ExoBrowserMainParts::Startup));
  */
}

void ExoBrowserMainParts::PostMainMessageLoopRun() 
{
  brightray::BrowserMainParts::PostMainMessageLoopRun();
  /* system_session_ is cleaned up in the above call. */

  /* TODO(spolu): Cleanup Remaining ExoSessions? */
}

}  // namespace
