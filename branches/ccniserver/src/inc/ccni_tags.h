/*
  Copyright (C) 2009  Wang Fei (bjwf2000@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Generl Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

#define xmlTagLogout                    "Logout"
#define xmlTagLogoutRes                 "LogoutRes"

#define xmlTagEnterRoom                 "EnterRoom"
#define xmlTagEnterRoomRes              "EnterRoomRes"
#define xmlTagBroadcastEnterRoom        "BroadcastEnterRoom"

#define xmlTagLeaveRoom                 "LeaveRoom"
#define xmlTagLeaveRoomRes              "LeaveRoomRes"
#define xmlTagBroadcastLeaveRoom        "BroadcastLeaveRoom"


#define xmlTagNewSession                "NewSession"
#define xmlTagNewSessionRes             "NewSessionRes"
#define xmlTagBroadcastNewSession       "BroadcastNewSession"
#define xmlTagBroadcastDelSession       "BroadcastDelSession"

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

#define xmlTagDraw                      "Draw"
#define xmlTagNotifyDraw                "NotifyDraw"
#define xmlTagAgreedDraw                "AgreedDraw"

#define xmlTagGiveUp                    "GiveUp"
#define xmlTagNotifyEndRound            "NotifyEndRound"

#define xmlTagLeaveSession              "LeaveSession"
#define xmlTagLeaveSessionRes           "LeaveSessionRes"
#define xmlTagNotifyLeaveSession        "NotifyLeaveSession"

#define xmlTagNotifyEndSession          "NotifyEndSession"
#define xmlTagNotifyBreakLine           "NotifyBreakLine"

#define xmlTagListRooms                 "ListRooms"
#define xmlTagListRoomsRes              "ListRoomsRes"

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
#define xmlTagSessionS                  "SessionS"
#define xmlTagSession                   "Session"
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
#define xmlTagOwner                     "Owner"
#define xmlTagOpponent                  "Opponent"
#define xmlTagWatchers                  "Watchers"
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

#define xmlTagRooms                      "Rooms"
#define xmlTagRoom                      "Room"
#define xmlTagCapacity                   "Capacity"
#define xmlTagUserCount                  "UserCount"
#define xmlTagId                       "Id"
 

#endif /*CCNI_TAGS_H_*/
