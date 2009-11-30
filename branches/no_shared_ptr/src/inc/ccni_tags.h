#ifndef CCNI_TAGS_H_
#define CCNI_TAGS_H_
#pragma once

//
//  ccni message label definations
//

#define xmlTagCCNIMessages              "CCNIMessages"

#define xmlTagCCNI                      "CCNI"
#define xmlTagCCNIRes                   "CCNIRes"

#define xmlTagMyState                   "MyState"
#define xmlTagMyStateRes                "MyStateRes"

#define xmlTagLogin                     "Login"
#define xmlTagLoginRes                  "LoginRes"

#define xmlTagEnterRoom                 "EnterRoom"
#define xmlTagEnterRoomRes              "EnterRoomRes"


#define xmlTagLogout                    "Logout"
#define xmlTagLogoutRes                 "LogoutRes"

#define xmlTagNotifyLogin               "NotifyLogin"
#define xmlTagNotifyLogout              "NotifyLogout"

#define xmlTagNewSession                "NewSession"
#define xmlTagNewSessionRes             "NewSessionRes"
#define xmlTagNotifyNewSession          "NotifyNewSession"

#define xmlTagEnterSession              "EnterSession"
#define xmlTagEnterSessionRes           "EnterSessionRes"
#define xmlTagNotifyEnterSession        "NotifyEnterSession"

#define xmlTagWatchSession              "WatchSession"
#define xmlTagWatchSessionRes           "WatchSessionRes"
#define xmlTagNotifyWatchSession        "NotifyWatchSession"

#define xmlTagReady                     "Ready"
#define xmlTagNotifyStartPlay           "NotifyStartPlay"

#define xmlTagMove                      "Move"
#define xmlTagNotifyMove                "NotifyMove"

#define xmlTagRequestDraw               "RequestDraw"
#define xmlTagNotifyRequestDraw         "NotifyRequestDraw"
#define xmlTagAgreedDraw                "AgreedDraw"

#define xmlTagGiveUp                    "GiveUp"
#define xmlTagNotifyEndRound            "NotifyEndRound"

#define xmlTagLeaveSession              "LeaveSession"
#define xmlTagLeaveSessionRes           "LeaveSessionRes"
#define xmlTagNotifyLeaveSession        "NotifyLeaveSession"

#define xmlTagNotifyEndSession          "NotifyEndSession"
#define xmlTagNotifyBreakLine           "NotifyBreakLine"

#define xmlTagListUsers                 "ListUsers"
#define xmlTagListUsersRes              "ListUsersRes"

#define xmlTagListSessions              "ListSessions"
#define xmlTagListSessionsRes           "ListSessionsRes"

#define xmlTagSendMessage               "SendMessage"
#define xmlTagNotifyMessage             "NotifyMessage"


//
//
//
#define xmlTagReturnCode                "ReturnCode"
#define xmlTagReturnInfo                "ReturnInfo"
//
// ccni message parameter labels
//
#define xmlTagUserList                  "UserList"
#define xmlTagUser                      "User"
#define xmlTagUserName                  "UserName"
#define xmlTagPassword                  "Password"
#define xmlTagNickName                  "NickName"
#define xmlTagScore                     "Score"
#define xmlTagLevel                     "Level"

#define xmlTagDescription               "Description"
#define xmlTagReturnCode                "ReturnCode"
#define xmlTagReturnInfo                "ReturnInfo"

#define xmlTagCount                     "Count"
#define xmlTagSessionList               "SessionList"
#define xmlTagSession                   "Session"
#define xmlTagSessionDescription        "SessionDescription"
#define xmlTagSessionInformation        "SessionInformation"
#define xmlTagSessionID                 "SessionID"
#define xmlTagTitle                     "Title"
#define xmlTagFen                       "Fen"
#define xmlTagRed                       "Red"
#define xmlTagInnings                   "Innings"
#define xmlTagRule                      "Rule"
#define xmlTagTotalTime                 "TotalTime"
#define xmlTagStepTime                  "StepTime"
#define xmlTagBonusPerStep              "BonusPerStep"
#define xmlTagRequiredScore             "RequiredScore"
#define xmlTagAnte                      "Ante"                 //每局额外的赌注分
#define xmlTagMaxWatcherCount           "MaxWatcherCount"
#define xmlTagRedPlayer                 "RedPlayer"
#define xmlTagBlackPlayer               "BlackPlayer"
#define xmlTagWatcherList               "WatcherList"
#define xmlTagStep                      "Step"
#define xmlTagTimeStamp                 "TimeStamp"

#define xmlTagResult                    "Result"
#define xmlTagReason                    "Reason"
#define xmlTagInformation               "Information"

#define xmlTagState                     "State"

#define xmlTagUnknowMessage             "UnknowMessage"

#define xmlTagSeriverInformation        "SeriverInformation"
#define xmlTagServerType                "ServerType"
#define xmlTagServerVersion             "ServerVersion"
#define xmlTagCCNIVersion               "CCNIVersion"
#define xmlTagRes                       "Res"

#endif /*CCNI_TAGS_H_*/
