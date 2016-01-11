window.l_welcome = 'OpenMCU-ru';
window.l_status = 'ステータス';

window.l_connections = 'コネクション';
window.l_connections_OFFLINE_PREFIX    = "<B>[オフライン] </B>"   ;
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[非表示] </B>"    ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "オーディオアウト"    ;
window.l_connections_AUDIO_IN_STR      = "オーディオイン"      ;
window.l_connections_VIDEO_OUT_STR     = "ビデオアウト"        ;
window.l_connections_VIDEO_IN_STR      = "ビデオイン"          ;
window.l_connections_AI_NEG_ERR        = "オーディオ入力なし"  ;
window.l_connections_AO_NEG_ERR        = "オーディオ出力なし"     ;
window.l_connections_VI_NEG_ERR        = "ビデオ入力なし"      ;
window.l_connections_VO_NEG_ERR        = "ビデオ出力なし"     ;
window.l_connections_BUTTON_TEXT       = "テキスト取得"            ;
window.l_connections_BUTTON_FORUM      = "BBCode取得"          ;
window.l_connections_BUTTON_HTML       = "HTML取得"            ;
window.l_connections_BUTTON_CLOSE      = "閉じる"          ;
window.l_connections_CODE_TOOLTIP      = "Ctrl+Cでコピー";
window.l_connections_DAYS_STR          = "日"              ;
window.l_connections_COL_NAME          = "名前"                ;
window.l_connections_COL_DURATION      = "経過時間"            ;
window.l_connections_COL_RTP           = "RTPチャンネル: コード"  ;
window.l_connections_COL_PACKETS       = "パケット"             ;
window.l_connections_COL_BYTES         = "バイト"               ;
window.l_connections_COL_KBPS          = "Kbit/s"              ;
window.l_connections_COL_FPS           = "FPS"                 ;
window.l_connections_word_room         = "ルーム"              ;
window.l_connections_COL_LOSTPCN       = "60秒ロスト"          ;

window.l_records = '録画';
window.l_control = 'コントロール';
window.l_rooms = 'ルーム';
window.l_invite = '招待';
window.l_invite_s = '招待成功';
window.l_invite_f = '招待失敗';
window.l_settings = '設定';
window.l_param_general = '全体';
window.l_param_managing_users = 'ユーザ';
window.l_param_managing_groups = 'グループ';
window.l_param_control_codes = 'コントロールコード';
window.l_param_room_codes = 'ルームコード';
window.l_param_h323_endpoints = 'H.323端末';
window.l_param_sip_endpoints = 'SIP端末';
window.l_param_video = 'ビデオ';
window.l_param_record = '録画';
window.l_param_h323 = 'H323パラメータ';
window.l_param_sip = 'SIPパラメータ';
window.l_param_sip_proxy = 'アカウント情報';
window.l_param_access_rules = 'アクセスルール';
window.l_param_receive_sound = 'オーディオコーデック(receive)';
window.l_param_transmit_sound = 'オーディオコーデック(transmit)';
window.l_param_receive_video = 'ビデオコーデック(receive)';
window.l_param_transmit_video = 'ビデオコーデック(transmit)';
window.l_param_sip_sound = 'オーディオコーデック';
window.l_param_sip_video = 'ビデオコーデック';
window.l_help = 'ヘルプ';
window.l_forum = 'フォーラム';
window.l_manual = 'マニュアル';
window.l_support = 'サポート';

window.l_info_welcome ='\
<p>\
OpenMCU-ruサービスは動作しており、接続を受け付けています\
</p>\
<p>\
複数の端末から接続し開始された会議は、非管理モードで動作します。すべてのメンバーが同じ画像で表示されます。非管理モードでopenmcu-Ruは自動的に「layouts.conf」ファイルで設定されたレイアウトを会議メンバーの数に応じて変更します。メンバーが１００以上接続された場合、表示は行いません。<br />\
非管理モードではオーディオストリームはすべての端末で受信することができます。<br />\
</p>\
<p>\
ルーム"echo"へのテストを行う事ができます。またルーム"testroomN"では１から１００までの数が行えます。<br />\
</p>\
<p>\
サーバ管理は、ページの上にあるメインメニューから行う事ができます<br />\
ページ<b>ステータス</b> 現在の接続や端末のパラメータが表示されます。<br />\
ページ<b>コントロール</b> ルームを管理や設定する事ができます。<br />\
ページ<b>設定</b> グローバルサーバパラメータの設定ができます。<br />\
<b>ヘルプ</b>ではより多くの情報が記載されています (オフィシャルサイトへのリンク).<br />\
</p>\
<p>サーバの概要を表示します</p>\
';
window.l_welcome_logo ='\
<p>\
<b>カスタムロゴイメージ</b>\
</p>\
<p>\
このイメージはビデオ入力がない場合表示します。上限500KBのJPEGのみ受け付けます。\
</p>\
';

window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='管理できるルームを表示しています。名前をクリックしてください';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="This page is used to manage your conference. You can chose participants from the list and place them in desired parts of a screen with layout control feature (drag-and-drop or drop-down boxes). Each position may be assigned to any participant directly (static), or it could be activated by voice.<br>Voice-activated positions could be marked as VAD or VAD2 (most active participants will be automatically moved from VAD to VAD2).";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>This page is used to\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
録画されているディレクトリを表示しています(サーバ側) \
またダウンロード可能です。';

window.l_info_connections ='\
現在の接続と端末のパラメータが表示されています\
';
window.l_info_param_general ='グローバルサーバパラメータ。「RESTORE DEFAULTS」のチェックボックスをチェックする事でデフォルト値に設定されます。';
window.l_info_param_managing_users = '';
window.l_info_param_managing_groups = '';
window.l_info_param_control_codes = '';
window.l_info_param_room_codes = '';
window.l_info_param_video ='ビデオ品質';
window.l_info_param_record ='録画品質';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'ルーム作成';
window.l_select_enter =        'ルーム';
window.l_select_record =       '録画';
window.l_select_moderated =    '調整';
window.l_select_moderated_yes ='はい';
window.l_select_moderated_no = 'いいえ';
window.l_select_visible =      '表示されているメンバ';
window.l_select_unvisible =    '表示されていないメンバ';
window.l_select_duration =     '経過時間';
window.l_select_delete =       'ルーム削除';

window.l_not_found = '見つかりません';

window.l_name_accept         = '更新';
window.l_name_reset          = 'リセット';
window.l_name_user           = 'ユーザ';
window.l_name_group          = 'グループ';
window.l_name_password       = 'パスワード';
window.l_name_roomname       = 'ルーム名';
window.l_name_access         = 'アクセス';
window.l_name_action         = 'アクション';
window.l_name_code           = 'コード';
window.l_name_message        = 'メッセージ';
window.l_name_address        = 'アドレス';
window.l_name_address_book   = 'アドレス帳';
window.l_name_save           = '保存';
window.l_name_cancel         = 'キャンセル';

window.l_name_display_name                         = '表示名';
window.l_name_frame_rate_from_mcu                  = 'MCUからのフレームレート';
window.l_name_bandwidth_from_mcu                   = 'MCUからの帯域幅';
window.l_name_bandwidth_to_mcu                     = 'MCUへ帯域幅';
window.l_name_transport                            = '発信変換';
window.l_name_port                                 = 'ポート';

window.l_name_register                             = '登録';
window.l_name_address_sip_proxy                    = 'SIPプロキシアドレス';
window.l_name_expires                              = '期限';

window.l_name_audio_codec                          = 'オーディオコーデック';
window.l_name_video_codec                          = 'オーディオコーデック';
window.l_name_audio_codec_receive                  = 'オーディオコーデック(receive)';
window.l_name_video_codec_receive                  = 'オーディオコーデック(receive)';
window.l_name_audio_codec_transmit                 = 'オーディオコーデック(transmit)';
window.l_name_video_codec_transmit                 = 'オーディオコーデック(transmit)';

window.l_room_invite_all_inactive_members          = 'すべてのメンバーを招待しますか';
window.l_room_drop_all_active_members              = 'すべての接続を解除しますか';
window.l_room_remove_all_inactive_members          = 'リストから無効なメンバを削除しますか';
window.l_room_drop_connection_with                 = '接続を切断';
window.l_room_remove_from_list                     = 'リストから削除';

window.l_room_deletion_text = ["",                   "Closing room &laquo;%&raquo;: disconnecting participants",
                                                     "Closing room &laquo;%&raquo;: waiting for participants get out of room",
                                                     "Closing room &laquo;%&raquo;: disconnecting hidden system members",
                                                     "Closing room &laquo;%&raquo;: waiting for members get out of room",
                                                     "Room &laquo;%&raquo; DELETED"
];


window.l_param_registrar = '記録係';
window.l_param_conference = '会議';
window.l_param_export = 'Export(named pipe)';
window.l_param_rtsp = 'RTSPパラメータ';
window.l_param_rtsp_servers = 'RTSPサーバ';
window.l_param_rtsp_endpoints = 'RTSP端末';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_name_host           = 'Host';
window.l_name_advanced       = 'Advanced';
window.l_name_auto_create              = 'Auto create';
window.l_name_auto_delete_empty        = 'Auto delete';
window.l_name_auto_record_start        = 'Auto record';
window.l_name_recall_last_template     = 'Recall last template';
window.l_name_time_limit               = 'Time limit';

window.l_name_registrar                            = '記録係';
window.l_name_account                              = 'Account';
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
window.l_name_video_resolution                     = 'Video resolution';

window.l_name_parameters_for_sending               = "Parameters for sending";
window.l_name_codec_parameters                     = "Codec parameters<br>(override received)";
window.l_name_default_parameters                   = "Default parameters";
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
window.l_totaldrivesize                            = "Total drive size: * GiB.";
window.l_recordstakesup                            = "Records takes up: * GiB (%).";
window.l_freespaceleft                             = "Free space left:  * GiB (%).";
window.l_delete                                    = "Delete";
window.l_recwilldeleted                            = "* will be deleted!";
window.l_recwilldeleted_ok                         = "[OK]";
window.l_recwilldeleted_cancel                     = "Cancel";
window.l_recwasdeleted                             = "* has been deleted";
window.l_filesize                                  = "File Size";
window.l_resolution                                = "Resolution";
window.l_startdatetime                             = "Start Date/Time";
window.l_lock_tpl_default                          = "Template locks conference by default";
window.l_name_force_split_video        = "Caching and control via browser";
window.l_name_auto_record_stop         = 'Auto record (stop)';

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
window.l_room_mute_all                             = 'Mute all participants (microphones)';
window.l_room_unmute_all                           = 'Unmute all participants (microphones)';
window.l_room_dial_all_members                     = 'Run continuous dialing ALL members';
///
window.l_param_telserver = 'Telnet server';
window.l_info_telserver = '';
window.l_auto_dial_delay                           = "Auto dial delay, s";
///
window.l_mute_new_conference_users                 = 'Mute new conference users';
window.l_unmute_new_conference_users               = 'Unmute new conference users';
