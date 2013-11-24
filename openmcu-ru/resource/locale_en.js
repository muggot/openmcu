window.l_welcome = 'OpenMCU-ru';
window.l_status = 'Status';

window.l_connections = 'Connections';
window.l_connections_OFFLINE_PREFIX    = "<B>[Offline] </B>"
window.l_connections_OFFLINE_SUFFIX    = ""
window.l_connections_HIDDEN_PREFIX     = "<B>[Hidden] </B>"
window.l_connections_HIDDEN_SUFFIX     = ""
window.l_connections_AUDIO_OUT_STR     = "Audio Out"
window.l_connections_AUDIO_IN_STR      = "Audio In"
window.l_connections_VIDEO_OUT_STR     = "Video Out"
window.l_connections_VIDEO_IN_STR      = "Video In"
window.l_connections_DAYS_STR          = "day(s)"
window.l_connections_COL_NAME          = "Name"
window.l_connections_COL_DURATION      = "Duration"
window.l_connections_COL_RTP           = "RTP Channel: Codec"
window.l_connections_COL_PACKETS       = "Packets"
window.l_connections_COL_BYTES         = "Bytes"
window.l_connections_COL_KBPS          = "Kbit/s"
window.l_connections_COL_FPS           = "FPS"
window.l_connections_word_room         = "Room"

window.l_records = 'Records';
window.l_control = 'Control';
window.l_rooms = 'Rooms';
window.l_invite = 'Invite';
window.l_invite_s = 'Invite succeeded';
window.l_invite_f = 'Invite failed';
window.l_settings = 'Settings';
window.l_param_general = 'General';
window.l_param_managing_users = 'Users';
window.l_param_managing_groups = 'Groups';
window.l_param_control_codes = 'Control codes';
window.l_param_room_codes = 'Room codes';
window.l_param_h323_endpoints = 'H.323 Endpoints';
window.l_param_sip_endpoints = 'SIP Endpoints';
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

window.l_info_welcome ='\
<p>\
Service OpenMCU-ru is running and accepting connections.\
</p>\
<p>\
Conference started by connection from some terminal will work in unmanaged mode. All members will see the same pictire. In unmanaged mode OpenMCU-ru automatically change layout depending on number of conference members, in order  from file "layouts.conf". After connecting 100 members other connecting members will not be visible, but will see others.<br />\
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
window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='To view room and set it in managed mode - click its name.';
window.l_info_control ='';
window.l_info_records ='\
Page shows video records directory (on the server), \
and allows to download them.';

window.l_info_connections ='\
Page shows current connections and terminal parameters.\
';
window.l_info_param_general ='Global server parameters. To reset all settings to defaults - check "RESTORE DEFAULTS" checkbox and click Accept.';
window.l_info_param_managing_users = '';
window.l_info_param_managing_groups = '';
window.l_info_param_control_codes = '';
window.l_info_param_room_codes = '';
window.l_info_param_h323_endpoints ='';
window.l_info_param_sip_endpoints ='';
window.l_info_param_video ='Outgoing video quality.';
window.l_info_param_record ='Recording quality.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_video = '';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Create room'
window.l_select_enter =        'Enter room'
window.l_select_moderated =    'Moderated'
window.l_select_visible =      'Visible members'
window.l_select_unvisible =    'Unvisible members'
window.l_select_duration =     'Duration'
window.l_select_delete =       'Delete room'

window.l_not_found = 'not found';

window.l_name_accept         = 'Accept';
window.l_name_reset          = 'Reset';
window.l_name_user           = 'User';
window.l_name_group          = 'Group';
window.l_name_password       = 'Password';
window.l_name_roomname       = 'Room name';
window.l_name_access         = 'Access';
window.l_name_action         = 'Action';
window.l_name_code           = 'Code';
window.l_name_message        = 'Message';
window.l_name_address        = 'Address';

window.l_name_display_name_override                = 'Display name override';
window.l_name_preferred_frame_rate_from_mcu        = 'Preferred frame rate from MCU';
window.l_name_preferred_bandwidth_from_mcu         = 'Preferred bandwidth from MCU';
window.l_name_preferred_bandwidth_to_mcu           = 'Preferred bandwidth to MCU';
window.l_name_outgoing_transport                   = 'Outgoing transport';

window.l_name_register                             = 'Register';
window.l_name_address_sip_proxy                    = 'Address SIP-proxy';
window.l_name_expires                              = 'Expires';

window.l_name_audio_codec                          = 'Audio codec';
window.l_name_video_codec                          = 'Video codec';
window.l_name_audio_codec_receive                  = 'Audio codec(receive)';
window.l_name_video_codec_receive                  = 'Video codec(receive)';
window.l_name_audio_codec_transmit                 = 'Audio codec(transmit)';
window.l_name_video_codec_transmit                 = 'Video codec(transmit)';
