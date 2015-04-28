window.l_welcome = 'OpenMCU-ru';
window.l_status = 'Estado';

window.l_connections = 'Conexões';
window.l_connections_OFFLINE_PREFIX    = "<B>[Offline] </B>"   ;
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Oculto] </B>"    ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "Saída de Áudio"      ;
window.l_connections_AUDIO_IN_STR      = "Entrada de Áudio"    ;
window.l_connections_VIDEO_OUT_STR     = "Saída de Vídeo"      ;
window.l_connections_VIDEO_IN_STR      = "Entrada de Vídeo"    ;
window.l_connections_AI_NEG_ERR        = "Sem entrada de áudio";
window.l_connections_AO_NEG_ERR        = "Sem saída de áudio"  ;
window.l_connections_VI_NEG_ERR        = "Sem entrada de vídeo";
window.l_connections_VO_NEG_ERR        = "Sem saída de vídeo"  ;
window.l_connections_BUTTON_TEXT       = "Entre Texto"         ;
window.l_connections_BUTTON_FORUM      = "Entre BBCode"        ;
window.l_connections_BUTTON_HTML       = "entre HTML"          ;
window.l_connections_BUTTON_CLOSE      = "Feche código"        ;
window.l_connections_CODE_TOOLTIP      = "Copiar, prima Ctrl+C";
window.l_connections_DAYS_STR          = "dia(s)"              ;
window.l_connections_COL_NAME          = "Nome"                ;
window.l_connections_COL_DURATION      = "Duração"             ;
window.l_connections_COL_RTP           = "RTP Canal: Codec"    ;
window.l_connections_COL_PACKETS       = "Pacotes"             ;
window.l_connections_COL_BYTES         = "Bytes"               ;
window.l_connections_COL_KBPS          = "Kbit/s"              ;
window.l_connections_COL_FPS           = "FPS"                 ;
window.l_connections_word_room         = "Sala"                ;
window.l_connections_COL_LOSTPCN       = "60s perdas"          ;

window.l_records = 'Gravações';
window.l_control = 'Controle';
window.l_rooms = 'Salas';
window.l_invite = 'Convida';
window.l_invite_s = 'Convite bem sucedido';
window.l_invite_f = 'Convite falhou';
window.l_settings = 'Configuração';
window.l_param_general = 'Geral';
window.l_param_registrar = 'Registrar';
window.l_param_conference = 'Conferência';
window.l_param_export = 'Export(named pipe)';
window.l_param_managing_users = 'Utilizadores';
window.l_param_managing_groups = 'Grupos';
window.l_param_control_codes = 'Código de Controle';
window.l_param_room_codes = 'Código de Sala';
window.l_param_h323_endpoints = 'Terminais H.323';
window.l_param_sip_endpoints = 'Terminais SIP';
window.l_param_rtsp = 'Parâmetros RTSP';
window.l_param_rtsp_servers = 'Servidores RTSP';
window.l_param_rtsp_endpoints = 'Terminais RTSP';
window.l_param_video = 'Vídeo';
window.l_param_record = 'Gravar';
window.l_param_h323 = 'Parâmetros H323';
window.l_param_sip = 'Parâmetros SIP';
window.l_param_sip_proxy = 'Informação�de Conta';
window.l_param_access_rules = 'Regras de Acesso';
window.l_param_receive_sound = 'Codecs de áudio (recepçãoo)';
window.l_param_transmit_sound = 'Codecs de áudio (transmissão)';
window.l_param_receive_video = 'Codecs de vídeo (recepção)';
window.l_param_transmit_video = 'Codecs de vídeo (transmissão)';
window.l_param_sip_sound = 'Codecs áudio';
window.l_param_sip_video = 'Codecs vídeo';
window.l_help = 'Ajuda';
window.l_forum = 'Forum';
window.l_manual = 'Manual';
window.l_support = 'Suporte';

window.l_info_welcome ='\
<p>\
O serviço OpenMCU-ru está a funcionar e a aceitar conexões.\
</p>\
<p>\
Conferência iniciada a partir de algum terminal irá funcionar no modo não controlado. Todos os participantes irão receber a mesma imagem. No modo não controlado, OpenMCU-ru trocará o layout automaticamente a depender do número de participantes na conferência na ordem determinada no ficheiro "layouts.conf". Depois de atingir 100 participantes os próximos não serão vistos mas verão os outros.<br />\
O áudio está em modo não controlado, recebendo e transmitindo para todos os participantes.<br />\
</p>\
<p>\
Para testar o servidor faça uma chamada para a sala "echo" ou para a sala "testroomN" onde N é um número de 1 à 100.<br />\
</p>\
<p>\
A gestão do servidor é feita a partir do menu principal no topo da página.<br />\
A página <b>Estado</b> mostra as conexões existentes e os parâmetros dos terminais.<br />\
Na página <b>Controle</b> as salas podem ser controladas.<br />\
Na páginas <b>Configurações</b> pode ser feita a configuração�o global de parâmetros do servidor.<br />\
Mais informação pode ser obtida em <b>Ajuda</b> (link para o site oficial na Internet).<br />\
</p>\
<p>Segue abaixo um sumário sobre o servidor.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Imagem do seu logotipo personalizado</b>\
</p>\
<p>\
Esta imagem será apresentada quando não houver recepção de vídeo. Aceita somente BMP, JPEG(com um máximo de 500KB), PNG, GIF.\
</p>\
';

window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='Clique no nome da sala para ver e gerir a mesma.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="Esta página é utilizada para gerir sua conferência. Você pode escolher os participantes a partir da lista e colocá-lo no local da tela que lhe convier utilizando o controle de layout (drag-and-drop ou drop-down). Cada posição�o pode ser designada a um participante directamente (estática) ou pode ser activada por voz.<br>A posição Voice-activated pode ser marcada como VAD ou VAD2 (os participantes mais activos serem�movidos de VAD para VAD2 automaticamente).";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>Esta página é usada para\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Esta página permite mostrar as gravações de vídeo no servidor \
e baixar as mesmas.';

window.l_info_connections ='\
Esta página mostra as conexões existentes e parâmteros dos terminais.\
';
window.l_info_param_general ='Parâmetros globais do servidor.';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_info_param_managing_users = '';
window.l_info_param_managing_groups = '';
window.l_info_param_control_codes = '';
window.l_info_param_room_codes = '';
window.l_info_param_video ='Qualidade do vídeo de saída.';
window.l_info_param_record ='Qualidade de gravação.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Cria sala'
window.l_select_enter =        'Entre na sala'
window.l_select_record =       'Grave'
window.l_select_moderated =    'Modere'
window.l_select_moderated_yes ='Sim'
window.l_select_moderated_no = 'Não'
window.l_select_visible =      'Membros Visíveis'
window.l_select_unvisible =    'Membros Invisíveis'
window.l_select_duration =     'Duração'
window.l_select_delete =       'Remove sala'

window.l_not_found = 'n�o encontrado';

window.l_name_accept         = 'Aceita';
window.l_name_reset          = 'Reset';
window.l_name_user           = 'Utilizador';
window.l_name_host           = 'Host';
window.l_name_group          = 'Grupo';
window.l_name_password       = 'Senha';
window.l_name_roomname       = 'Nome da sala';
window.l_name_access         = 'Acesso';
window.l_name_action         = 'Acção';
window.l_name_code           = 'Código';
window.l_name_message        = 'Mensagem';
window.l_name_address        = 'Endereço';
window.l_name_address_book   = 'Livro de endereços';
window.l_name_save           = 'Salva';
window.l_name_cancel         = 'Cancela';
window.l_name_advanced       = 'Avançado';
window.l_name_auto_create              = 'Auto criação';
window.l_name_auto_delete_empty        = 'Auto eliminação';
window.l_name_auto_record_start        = 'Auto gravação';
window.l_name_recall_last_template     = 'Recarrega último modelo';
window.l_name_time_limit               = 'Limite de tempo';

window.l_name_display_name                         = 'Sobrepõe o nome mostrado';
window.l_name_frame_rate_from_mcu                  = 'Frame rate da MCU';
window.l_name_bandwidth_from_mcu                   = 'Banda da MCU, Kbit/s';
window.l_name_bandwidth_to_mcu                     = 'Banda para MCU, Kbit/s';
window.l_name_transport                            = 'Transporte';
window.l_name_port                                 = 'porta';

window.l_name_registrar                            = 'Registrar';
window.l_name_account                              = 'Conta';
window.l_name_register                             = 'Registra';
window.l_name_address_sip_proxy                    = 'Endereço do SIP-proxy';
window.l_name_expires                              = 'Expira em';
window.l_name_path                                 = 'Caminho';
window.l_name_enable                               = 'Habilita';

window.l_name_registered                           = 'Registrado';
window.l_name_connected                            = 'Conectado';
window.l_name_last_ping_response                   = 'última resposta';

window.l_name_codec                                = 'Codec';
window.l_name_audio                                = 'Áudio';
window.l_name_video                                = 'Vídeo';
window.l_name_audio_receive                        = 'Áudio (recebe)';
window.l_name_audio_transmit                       = 'Áudio (transmite)';
window.l_name_video_receive                        = 'Vídeo (recebe)';
window.l_name_video_transmit                       = 'Vídeo (transmite)';
window.l_name_audio_codec                          = 'Codec de áudio';
window.l_name_video_codec                          = 'Codec de vídeo';
window.l_name_audio_codec_receive                  = 'Codec de áudio (recebe)';
window.l_name_video_codec_receive                  = 'Codec de vídeo (recebe)';
window.l_name_audio_codec_transmit                 = 'Codec de áudio (transmite)';
window.l_name_video_codec_transmit                 = 'Codec de vídeo (transmite)';
window.l_name_video_resolution                     = 'Resolução de vídeo';

window.l_name_parameters_for_sending               = "Parâmetros para envio";
window.l_name_codec_parameters                     = "Parâmetros de Codec<br>(sobrepõe o recebido)";
window.l_name_default_parameters                   = "Parâmetros por defeito";

window.l_room_invite_all_inactive_members          = 'Convida TODOS os membros inactivos';
window.l_room_drop_all_active_members              = 'Desliga TODAS conexões activas';
window.l_room_remove_all_inactive_members          = 'Remove TODOS membros inactivos da lista';
window.l_room_drop_connection_with                 = 'Desliga conexões com';
window.l_room_remove_from_list                     = 'Remove da lista';

window.l_room_deletion_text = ["",                   "Closing room &laquo;%&raquo;: a desconectar participantes",
                                                     "Closing room &laquo;%&raquo;: aguardando participante sair da sala",
                                                     "Closing room &laquo;%&raquo;: desconectando membros escondidos de sistema",
                                                     "Closing room &laquo;%&raquo;: aguardando saída de membros da sala",
                                                     "Room &laquo;%&raquo; ELIMINADO"
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
