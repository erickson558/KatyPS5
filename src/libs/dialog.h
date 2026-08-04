#ifndef EMULATOR_INCLUDE_EMULATOR_DIALOG_H_
#define EMULATOR_INCLUDE_EMULATOR_DIALOG_H_

#include "common/abi.h"
#include "common/common.h"

namespace Libs::Dialog {

namespace CommonDialog {

int KATY_SYSV_ABI CommonDialogInitialize();

} // namespace CommonDialog

namespace ImeDialog {

int KATY_SYSV_ABI ImeDialogGetStatus();

} // namespace ImeDialog

namespace LoginDialog {

int KATY_SYSV_ABI  LoginDialogInitialize();
int KATY_SYSV_ABI  LoginDialogTerminate();
int KATY_SYSV_ABI  LoginDialogOpen(const void* param);
int KATY_SYSV_ABI  LoginDialogClose();
int KATY_SYSV_ABI  LoginDialogUpdateStatus();
int KATY_SYSV_ABI  LoginDialogGetStatus();
int KATY_SYSV_ABI  LoginDialogGetResult(void* result);
void KATY_SYSV_ABI LoginDialogParamInitialize(void* param);

} // namespace LoginDialog

namespace SigninDialog {

int KATY_SYSV_ABI SigninDialogInitialize();
int KATY_SYSV_ABI SigninDialogTerminate();
int KATY_SYSV_ABI SigninDialogOpen(const void* param);
int KATY_SYSV_ABI SigninDialogClose();
int KATY_SYSV_ABI SigninDialogUpdateStatus();
int KATY_SYSV_ABI SigninDialogGetStatus();
int KATY_SYSV_ABI SigninDialogGetResult(void* result);

} // namespace SigninDialog

namespace SaveDataDialog {

int KATY_SYSV_ABI SaveDataDialogInitialize();
int KATY_SYSV_ABI SaveDataDialogGetStatus();
int KATY_SYSV_ABI SaveDataDialogUpdateStatus();
int KATY_SYSV_ABI SaveDataDialogGetResult(void* result);
int KATY_SYSV_ABI SaveDataDialogOpen(const void* param);
int KATY_SYSV_ABI SaveDataDialogClose(const void* close_param);
int KATY_SYSV_ABI SaveDataDialogIsReadyToDisplay();
int KATY_SYSV_ABI SaveDataDialogTerminate();
int KATY_SYSV_ABI SaveDataDialogProgressBarInc(int target, uint32_t delta);
int KATY_SYSV_ABI SaveDataDialogProgressBarSetValue(int target, uint32_t rate);

} // namespace SaveDataDialog

namespace MsgDialog {

int KATY_SYSV_ABI MsgDialogInitialize();
int KATY_SYSV_ABI MsgDialogOpen(const void* param);
int KATY_SYSV_ABI MsgDialogUpdateStatus();
int KATY_SYSV_ABI MsgDialogGetStatus();
int KATY_SYSV_ABI MsgDialogGetResult(void* result);
int KATY_SYSV_ABI MsgDialogTerminate();
int KATY_SYSV_ABI MsgDialogClose();
int KATY_SYSV_ABI MsgDialogProgressBarInc(int target, uint32_t delta);
int KATY_SYSV_ABI MsgDialogProgressBarSetValue(int target, uint32_t rate);
int KATY_SYSV_ABI MsgDialogProgressBarSetMsg(int target, const char* msg);

} // namespace MsgDialog

namespace ErrorDialog {

int KATY_SYSV_ABI ErrorDialogInitialize();
int KATY_SYSV_ABI ErrorDialogOpen(const void* param);
int KATY_SYSV_ABI ErrorDialogClose();
int KATY_SYSV_ABI ErrorDialogTerminate();
int KATY_SYSV_ABI ErrorDialogUpdateStatus();
int KATY_SYSV_ABI ErrorDialogGetStatus();

} // namespace ErrorDialog

} // namespace Libs::Dialog

#endif /* EMULATOR_INCLUDE_EMULATOR_DIALOG_H_ */
