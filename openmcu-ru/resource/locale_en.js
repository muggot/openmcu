window.l_welcome = 'OpenMCU-ru';
window.l_status = 'Status';

window.l_connections = 'Connections';
window.l_connections_OFFLINE_PREFIX    = "<B>[Offline] </B>"   ;
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Hidden] </B>"    ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "Audio Out"           ;
window.l_connections_AUDIO_IN_STR      = "Audio In"            ;
window.l_connections_VIDEO_OUT_STR     = "Video Out"           ;
window.l_connections_VIDEO_IN_STR      = "Video In"            ;
window.l_connections_AI_NEG_ERR        = "No input audio"      ;
window.l_connections_AO_NEG_ERR        = "No output Audio"     ;
window.l_connections_VI_NEG_ERR        = "No input video"      ;
window.l_connections_VO_NEG_ERR        = "No output video"     ;
window.l_connections_BUTTON_TEXT       = "Get Text"            ;
window.l_connections_BUTTON_FORUM      = "Get BBCode"          ;
window.l_connections_BUTTON_HTML       = "Get HTML"            ;
window.l_connections_BUTTON_CLOSE      = "Close code"          ;
window.l_connections_CODE_TOOLTIP      = "Press Ctrl+C to copy";
window.l_connections_DAYS_STR          = "day(s)"              ;
window.l_connections_COL_NAME          = "Name"                ;
window.l_connections_COL_DURATION      = "Duration"            ;
window.l_connections_COL_RTP           = "RTP Channel: Codec"  ;
window.l_connections_COL_PACKETS       = "Packets"             ;
window.l_connections_COL_BYTES         = "Bytes"               ;
window.l_connections_COL_KBPS          = "Kbit/s"              ;
window.l_connections_COL_FPS           = "FPS"                 ;
window.l_connections_word_room         = "Room"                ;
window.l_connections_COL_LOSTPCN       = "60s losses"          ;

window.l_records = 'Records';
window.l_control = 'Control';
window.l_rooms = 'Rooms';
window.l_invite = 'Invite';
window.l_invite_s = 'Invite succeeded';
window.l_invite_f = 'Invite failed';
window.l_settings = 'Settings';
window.l_param_general = 'General';
window.l_param_registrar = 'Registrar';
window.l_param_conference = 'Conference';
window.l_param_managing_users = 'Users';
window.l_param_managing_groups = 'Groups';
window.l_param_control_codes = 'Control codes';
window.l_param_room_codes = 'Room codes';
window.l_param_h323_endpoints = 'H.323 Endpoints';
window.l_param_sip_endpoints = 'SIP Endpoints';
window.l_param_rtsp = 'RTSP parameters';
window.l_param_rtsp_servers = 'RTSP Servers';
window.l_param_rtsp_endpoints = 'RTSP Endpoints';
window.l_param_video = 'Video';
window.l_param_record = 'Record';
window.l_param_h323 = 'H323 parameters';
window.l_param_sip = 'SIP parameters';
window.l_param_sip_proxy = 'Account information';
window.l_param_access_rules = 'Access rules';
window.l_param_receive_sound = 'Audio codecs (receive)';
window.l_param_transmit_sound = 'Audio codecs (transmit)';
window.l_param_receive_video = 'Video codecs (receive)';
window.l_param_transmit_video = 'Video codecs (transmit)';
window.l_param_sip_sound = 'Audio codecs';
window.l_param_sip_video = 'Video codecs';
window.l_help = 'Help';
window.l_forum = 'Forum';
window.l_manual = 'Manual';
window.l_support = 'Support';

window.l_info_welcome ='\
<p>\
Honeynet\';
window.l_welcome_logo ='\
<p>\
<b>Custom logo iamge</b>\
</p>\
<p>\
This image is shown when no input video in mixer. Accepted only JPEG, maximum 500kB.\
</p>\
';

window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='To view room and set it in managed mode - click its name.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="This page is used to manage your conference. You can chose participants from the list and place them in desired parts of a screen with layout control feature (drag-and-drop or drop-down boxes). Each position may be assigned to any participant directly (static), or it could be activated by voice.<br>Voice-activated positions could be marked as VAD or VAD2 (most active participants will be automatically moved from VAD to VAD2).";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>This page is used to\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Page shows video records directory (on the server), \
and allows to download them.';

window.l_info_connections ='\
Page shows current connections and terminal parameters.\
';
window.l_info_param_general ='Global server parameters. To reset all settings to defaults - check "RESTORE DEFAULTS" checkbox and click Accept.';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_info_param_managing_users = '';
window.l_info_param_managing_groups = '';
window.l_info_param_control_codes = '';
window.l_info_param_room_codes = '';
window.l_info_param_video ='Outgoing video quality.';
window.l_info_param_record ='Recording quality.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Create room'
window.l_select_enter =        'Enter room'
window.l_select_record =       'Record'
window.l_select_moderated =    'Moderated'
window.l_select_moderated_yes ='Yes'
window.l_select_moderated_no = 'No'
window.l_select_visible =      'Visible members'
window.l_select_unvisible =    'Unvisible members'
window.l_select_duration =     'Duration'
window.l_select_delete =       'Delete room'

window.l_not_found = 'not found';

window.l_name_accept         = 'Accept';
window.l_name_reset          = 'Reset';
window.l_name_user           = 'User';
window.l_name_host           = 'Host';
window.l_name_group          = 'Group';
window.l_name_password       = 'Password';
window.l_name_roomname       = 'Room name';
window.l_name_access         = 'Access';
window.l_name_action         = 'Action';
window.l_name_code           = 'Code';
window.l_name_message        = 'Message';
window.l_name_address        = 'Address';
window.l_name_address_book   = 'Address book';
window.l_name_save           = 'Save';
window.l_name_cancel         = 'Cancel';
window.l_name_advanced       = 'Advanced';
window.l_name_auto_create              = 'Auto create';
window.l_name_force_split_video        = 'Moderated';
window.l_name_auto_delete_empty        = 'Auto delete';
window.l_name_auto_record_not_empty    = 'Auto record';
window.l_name_recall_last_template     = 'Recall last template';
window.l_name_time_limit               = 'Time limit';

window.l_name_display_name                         = 'Display name override';
window.l_name_frame_rate_from_mcu                  = 'Frame rate from MCU';
window.l_name_bandwidth_from_mcu                   = 'Bandwidth from MCU, Kbit/s';
window.l_name_bandwidth_to_mcu                     = 'Bandwidth to MCU, Kbit/s';
window.l_name_transport                            = 'Transport';
window.l_name_port                                 = 'port';

window.l_name_registrar                            = 'Registrar';
window.l_name_account                              = 'Account';
window.l_name_register                             = 'Register';
window.l_name_address_sip_proxy                    = 'Address SIP-proxy';
window.l_name_expires                              = 'Expires';
window.l_name_path                                 = 'Path';
window.l_name_enable                               = 'Enable';

window.l_name_registered                           = 'Registered';
window.l_name_connected                            = 'Connected';
window.l_name_last_ping_response                   = 'Last response';

window.l_name_codec                                = 'Codec';
window.l_name_audio                                = 'Audio';
window.l_name_video                                = 'Video';
window.l_name_audio_receive                        = 'Audio (receive)';
window.l_name_audio_transmit                       = 'Audio (transmit)';
window.l_name_video_receive                        = 'Video (receive)';
window.l_name_video_transmit                       = 'Video (transmit)';
window.l_name_audio_codec                          = 'Audio codec';
window.l_name_video_codec                          = 'Video codec';
window.l_name_audio_codec_receive                  = 'Audio codec(receive)';
window.l_name_video_codec_receive                  = 'Video codec(receive)';
window.l_name_audio_codec_transmit                 = 'Audio codec(transmit)';
window.l_name_video_codec_transmit                 = 'Video codec(transmit)';
window.l_name_video_resolution                     = 'Video resolution';

window.l_name_parameters_for_sending               = "Parameters for sending";
window.l_name_codec_parameters                     = "Codec parameters<br>(override received)";
window.l_name_default_parameters                   = "Default parameters";

window.l_room_invite_all_inactive_members          = 'Invite ALL inactive members?';
window.l_room_drop_all_active_members              = 'Drop ALL active connections?';
window.l_room_remove_all_inactive_members          = 'Remove ALL inactive members from list?';
window.l_room_drop_connection_with                 = 'Drop connection with';
window.l_room_remove_from_list                     = 'Remove from list';

window.l_room_deletion_text = ["",                   "Closing room &laquo;%&raquo;: disconnecting participants",
                                                     "Closing room &laquo;%&raquo;: waiting for participants get out of room",
                                                     "Closing room &laquo;%&raquo;: disconnecting hidden system members",
                                                     "Closing room &laquo;%&raquo;: waiting for members get out of room",
                                                     "Room &laquo;%&raquo; DELETED"
];

window.l_takecontrol                               = "Current conference work mode is automatic (unmanaged). Click to operate the conference by your own (take control).";
window.l_decontrol                                 = "Current conference work mode is managed by operator (that is you). Click to stop management and convert conference to unmanaged (automatic) state.";
window.l_vadsetup                                  = "Voice activity detection (VAD) parameters";
window.l_globalmute                                = "Invisible members are UNMUTED. Click to mute.";
window.l_globalunmute                              = "Invisible members are MUTED. Click to unmute.";
window.l_filtermode                                = [
                                                       "Currently selected FASTEST video scaling, worst quality."
                                                      ,"Currently selected BILINEAR video scaling, optimal for most cases."
                                                      ,"Currently selected BOX FILTER for video scaling - perfect, but slow."
                                                     ];
window.l_videorecorder                             = "Start video recording";
window.l_videorecorderstop                         = "Stop video recording";
window.l_pleasetakecontrol                         = "This could not be done because the conference is working in unmanaged (automatic) mode.";
window.l_decontrolmixersconfirm                    = "You are converting the converence into unmanaged mode..\r\nAdditional video mixers will be removed and the only one will be kept.\r\nAre you sure, you want to continue?";
window.l_templatedeleteconfirm                     = "Template * will deleted";
