// igHeader.h

// TODO: Disolve this header; it slows down compilation.

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/thread.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/aboutdlg.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/dc.h>
#include <wx/utils.h>
#include <wx/cmdline.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/msw/regconf.h>
#include <wx/file.h>

#include <list>

#ifdef VIDEO_SUPPORT
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}
#endif //VIDEO_SUPPORT

class igPlugin;
class igApp;
class igFrame;
class igThread;
class igCanvas;
class igOptionsDialog;

#include "igPlugin.h"
#include "igApp.h"
#include "igFrame.h"
#include "igThread.h"
#include "igCanvas.h"
#include "igOptions.h"

// igHeader.h