#include <Dialogs.h>
#include <Fonts.h>
#include <MacWindows.h>
#include <Menus.h>
#include <QuickDraw.h>
#include <TextEdit.h>

#define kDebuggerInstalledBit   5
#define kAlertID    128
#define kBaseResID  128
#define MacJmpByte  ((UInt8 *)0x0120)   // MacsBug flags in 24 bit mode [byte].
#define MacJmpFlag  ((UInt8 *)0x0BFF)   // MacsBug flag [byte].

static void Initialize(void) {
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
}

void main(void) {
    SInt32          gestaltResponse;
    StringHandle    welcomeMsgH;
    Str255          welcomeMsg;
    
    Initialize();

    if (Gestalt(gestaltAddressingModeAttr, &gestaltResponse) == noErr) {
        UInt16  debugFlags;

        // As documented in the "Macsbug Reference & Debugging Guide", page 412
        // if we have a 32 bit capable Memory Manager, debugger flags are at 0x0BFF
        // if we have a 24 bit capable Memory Manager, debugger flags are at 0x0120

        if ((gestaltResponse & (1L << gestalt32BitCapable)) != 0) {
            debugFlags = *MacJmpFlag;
        } else {
            debugFlags = *MacJmpByte;
        }

        if ( (debugFlags & (1L << kDebuggerInstalledBit)) == 0 ) {           
	        StopAlert(kAlertID, NULL);
        } else {welcomeMsgH = GetString(kBaseResID);            
            if (welcomeMsgH == nil) ExitToShell();

            HLock((Handle)welcomeMsgH);
            BlockMove(*welcomeMsgH, welcomeMsg, *welcomeMsgH[0] + 1);
            HUnlock((Handle)welcomeMsgH);
            DebugStr(welcomeMsg);
        }
    }
}
