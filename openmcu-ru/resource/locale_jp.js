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
window.l_info_param_h323_endpoints ='';
window.l_info_param_sip_endpoints ='';
window.l_info_param_video ='ビデオ品質';
window.l_info_param_record ='録画品質';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_video = '';
window.l_info_param_receive_sound = '';
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

window.l_name_display_name_override                = '表示名';
window.l_name_preferred_frame_rate_from_mcu        = 'MCUからのフレームレート';
window.l_name_preferred_bandwidth_from_mcu         = 'MCUからの帯域幅';
window.l_name_preferred_bandwidth_to_mcu           = 'MCUへ帯域幅';
window.l_name_outgoing_transport                   = '発信変換';
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

window.l_room_invite_all_inactive_members          = 'すべてのメンバーを招待しますか?';
window.l_room_drop_all_active_members              = 'すべての接続を解除しますか?';
window.l_room_remove_all_inactive_members          = 'リストから無効なメンバを削除しますか?';
window.l_room_drop_connection_with                 = '接続を切断';
window.l_room_remove_from_list                     = 'リストから削除';

window.l_room_deletion_text = ["",                   "Closing room &laquo;%&raquo;: disconnecting participants",
                                                     "Closing room &laquo;%&raquo;: waiting for participants get out of room",
                                                     "Closing room &laquo;%&raquo;: disconnecting hidden system members",
                                                     "Closing room &laquo;%&raquo;: waiting for members get out of room",
                                                     "Room &laquo;%&raquo; DELETED"
];