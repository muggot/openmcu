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
window.l_param_export = 'Export(named pipe)';
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
Service OpenMCU-ru is running and accepting connections.\
</p>\
<p>\
Conference started by connection from some terminal will work in unmanaged mode. All members will see the same picture. In unmanaged mode OpenMCU-ru automatically change layout depending on number of conference members, in order  from file "layouts.conf". After connecting 100 members other connecting members will not be visible, but will see others.<br />\
Audio streams in unmanaged mode are transmitting and receiving from all connected members.<br />\
</p>\
<p>\
To test the server make call to room "echo". Or to room "testroomN", where N is number from 1 to 100.<br />\
</p>\
<p>\
Server management is performed from main menu at the top of the page.<br />\
Page <b>Status</b> shows current connections and terminal parameters.<br />\
At page <b>Control</b> rooms can be set to managed mode.<br />\
At page <b>Settings</b> can be set global server parameters.<br />\
More info can be found in <b>Help</b> (link leads to official Internet site).<br />\
</p>\
<p>Below shows a summary of the server.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Custom logo iamge</b>\
</p>\
<p>\
This image is shown when no input video in mixer. Accepted only BMP, JPEG(maximum 500kB), PNG, GIF.\
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
window.l_info_param_general ='Global server parameters.';
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
window.l_name_force_split_video        = "Caching and control via browser";
window.l_name_auto_delete_empty        = 'Auto delete';
window.l_name_auto_record_start        = 'Auto record';
window.l_name_auto_record_stop         = 'Auto record (stop)';
window.l_lock_tpl_default              = "Template locks conference by default";
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

window.l_room_mute_all                             = 'Mute all participants (microphones)';
window.l_room_unmute_all                           = 'Unmute all participants (microphones)';
window.l_room_invite_all_inactive_members          = 'Invite ALL inactive members';
window.l_room_dial_all_members                     = 'Run continuous dialing ALL members';
window.l_room_drop_all_active_members              = 'Drop ALL active connections';
window.l_room_remove_all_inactive_members          = 'Remove ALL inactive members from list';
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
window.l_decontrolmixersconfirm                    = "You are converting the conference into unmanaged mode..\r\nAdditional video mixers will be removed and the only one will be kept.\r\nAre you sure, you want to continue?";
window.l_templatedeleteconfirm                     = "Template * will deleted";
window.l_changelogo                                = "Change: ";

window.l_dir_no_records                            = "The direcory does not contain records at the moment.";
window.l_download                                  = "Download";
window.l_delete                                    = "Delete";
window.l_totaldrivesize                            = "Total drive size: * GiB.";
window.l_recordstakesup                            = "Records takes up: * GiB (%).";
window.l_freespaceleft                             = "Free space left:  * GiB (%).";
window.l_recwilldeleted                            = "* will be deleted!";
window.l_recwilldeleted_ok                         = "[OK]";
window.l_recwilldeleted_cancel                     = "Cancel";
window.l_recwasdeleted                             = "* has been deleted";
window.l_filesize                                  = "File Size";
window.l_resolution                                = "Resolution";
window.l_startdatetime                             = "Start Date/Time";

///
window.l_restore_defaults                          = "RESTORE DEFAULTS";
window.l_language                                  = "Language";
window.l_server_id                                 = "OpenMCU-ru Server Id";
window.l_default_protocol_for_outgoing_calls       = "Default protocol for outgoing calls";
window.l_http_secure                               = "HTTP secure";
window.l_http_certificate                          = "HTTP certificate";
window.l_http_ip                                   = "HTTP IP";
window.l_http_port                                 = "HTTP Port";
window.l_rtp_base_port                             = "RTP Base Port";
window.l_rtp_max_port                              = "RTP Max Port";
window.l_trace_level                               = "Trace level";
window.l_rotate_trace                              = "Rotate trace files at startup";
window.l_log_level                                 = "Log Level";
window.l_call_log_filename                         = "Call log filename";
window.l_room_control_event_buffer_size            = "Room control event buffer size";
window.l_copy_web_log                              = "Copy web log to call log";
window.l_default_room                              = "Default room";
window.l_reject_duplicate_name                     = "Reject duplicate name";
window.l_allow_loopback_calls                      = "Allow loopback calls";
///
window.l_allow_internal_calls                      = "Allow internal calls";
window.l_sip_allow_reg_without_auth                = "SIP allow registration without authentication";
window.l_sip_allow_mcu_calls_without_auth          = "SIP allow MCU calls without authentication";
window.l_sip_allow_internal_calls_without_auth     = "SIP allow internal calls without authentication";
window.l_sip_registrar_minimum_expiration          = "SIP registrar minimum expiration";
window.l_sip_registrar_maximum_expiration          = "SIP registrar maximum expiration";
window.l_h323_gatekeeper_enable                    = "H.323 gatekeeper enable";
window.l_h323_allow_reg_without_auth               = "H.323 allow registration without authentication";
window.l_h323_allow_mcu_calls_without_reg          = "H.323 allow MCU calls without registration";
window.l_h323_allow_internal_calls_without_reg     = "H.323 allow internal calls without registration";
window.l_h323_allow_duplicate_aliases              = "H.323 allow duplicate aliases";
window.l_h323_gatekeeper_minimum_ttl               = "H.323 gatekeeper minimum Time To Live";
window.l_h323_gatekeeper_maximum_ttl               = "H.323 gatekeeper maximum Time To Live";
///
window.l_enable_video                              = "Enable video";
window.l_max_bit_rate                              = "Max bit rate";
window.l_tx_key_frame_period                       = "Tx key frame period";
window.l_encoding_threads                          = "Encoding threads";
window.l_encoding_cpu_used                         = "Encoding CPU used";
///
window.l_enable_export                             = "Enable export";
window.l_video_frame_rate                          = "Video frame rate";
window.l_video_frame_width                         = "Video frame width";
window.l_video_frame_height                        = "Video frame height";
window.l_audio_sample_rate                         = "Audio sample rate";
window.l_audio_channels                            = "Audio channels";
window.l_video_bitrate                             = "Video bitrate";
window.l_audio_bitrate                             = "Audio bitrate";
///
window.l_listener                                  = "Listener";
window.l_interface                                 = "Interface";
window.l_nat_router_ip                             = "NAT Router IP";
window.l_treat_as_global_for_nat                   = "Treat as global for NAT";
window.l_disable_fast_start                        = "Disable Fast-Start";
window.l_disable_h245_tunneling                    = "Disable H.245 Tunneling";
window.l_gk_mode                                   = "Gatekeeper Mode";
window.l_gk_reg_ttl                                = "Gatekeeper registration TTL(Time To Live)";
window.l_gk_reg_retry_interval                     = "Gatekeeper request retry interval";
window.l_gk_host                                   = "Gatekeeper host";
window.l_gk_username                               = "Gatekeeper username";
window.l_gk_password                               = "Gatekeeper password";
window.l_gk_room_names                             = "Gatekeeper room names";
///
window.l_server                                    = "Server";
window.l_server_list                               = "Server list";
///
window.l_directory                                 = "Directory";
window.l_rtp_input_timeout                         = "RTP Input Timeout";
window.l_received_vfu_delay                        = "Limitation VFU, r/s";
window.l_video_cache                               = "Video cache";
window.l_interval                                  = "interval";
window.l_internal_call_processing                  = "Internal call processing";
window.l_room_auto_create_when_connecting          = "Auto create when connecting";
window.l_enter_template_name                       = "Enter template id";
window.l_disconnect                                = "Disconnect";
window.l_add_to_abook                              = "Add to address book";
window.l_accounts                                  = "Accounts";
///
window.l_param_telserver = 'Telnet server';
window.l_info_telserver = '';
