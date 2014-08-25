window.l_welcome = 'OpenMCU-ru';
window.l_status = '<Estado';

window.l_connections = 'Conexões';
window.l_connections_OFFLINE_PREFIX    = "<B>[Offline] </B>"   ;
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Oculto] </B>"    ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "Audio Out"           ;
window.l_connections_AUDIO_IN_STR      = "Audio In"            ;
window.l_connections_VIDEO_OUT_STR     = "Video Out"           ;
window.l_connections_VIDEO_IN_STR      = "Video In"            ;
window.l_connections_AI_NEG_ERR        = "Sem entrada de áudio";
window.l_connections_AO_NEG_ERR        = "Sem saída de áudio"  ;
window.l_connections_VI_NEG_ERR        = "Sem entrada de vídeo";
window.l_connections_VO_NEG_ERR        = "Sem saída de vídeo"  ;
window.l_connections_BUTTON_TEXT       = "Pegue em Texto"      ;
window.l_connections_BUTTON_FORUM      = "Pegue em BBCode"     ;
window.l_connections_BUTTON_HTML       = "Pegue em HTML"       ;
window.l_connections_BUTTON_CLOSE      = "Feche code"          ;
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
window.l_param_managing_users = 'Utilzadores';
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
window.l_param_sip_proxy = 'Informação de Conta';
window.l_param_access_rules = 'Regras de Acesso';
window.l_param_receive_sound = 'Codecs de áudio (recepção)';
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
Conferência iniciada a partir de algum terminal irá funcionar no modo não controlado. Todos os participantes irão receber a mesma imagem. No modo não controlado, OpenMCU-ru trrocará o layout automaticamente a depender do número de participantes na conferência na ordem determinada no ficheiro "layouts.conf". Depois de atingir 100 participantes os próximos não serão vistos mas verão os outros.<br />\
O  áudio está em modo não controlado, recebendo e transmitindo para todos os participantes.<br />\
</p>\
<p>\
Para testar o servidor faça uma chamada para a sala "echo" ou para a sala "testroomN" onde N é um número de 1 à 100.<br />\
</p>\
<p>\
A gestão do servidor é feita a partir do menu principal no topo da página.<br />\
A página <b>Estado</b> mostra as conexões existentes e os parâmetros dos terminais.<br />\
Na página <b>Controle</b> as salas podem ser controladas.<br />\
Na páginas <b>Configurações</b> pode ser feita a configuração global de parâmetros do servidor.<br />\
Mais informação pode ser obtida em <b>Ajuda</b> (link para o site oficial na Internet).<br />\
</p>\
<p>Segue abaixo um sumário sobre o servidor.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Imagem do seu logotipo personalisado</b>\
</p>\
<p>\
Esta imagem será mostrada quando não houver recepção de vídeo. Aceita somente JPEG com um máximo de 500KB.\
</p>\
';

window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='Para ver uma sala e gerir a mesma clique no nome da sala.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="Esta página é utilizada para gerir sua conferência. Você pode escolher os participantes a partir da lista e colocá-lo no local da tela que lhe convier utilizando o controle de layout (drag-and-drop ou drop-down). Cada posição pode ser designada a um participante directamente (estática) ou pode ser activada por voz.<br>A posição Voice-activated pode ser marcada como VAD ou VAD2 (os participantes mais activos serão movidos de VAD para VAD2 automaticamente).";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>This page is used to\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Página permite mostrar as vídeo-gravações na directoria, \
Page shows video records directory (on the server), \
do servidor e baixar as mesmas.';

window.l_info_connections ='\
Página mostra as conexões existentes e parâmteros dos terminais.\
';
window.l_info_param_general ='Parâmetros globais do servidor. Para restaurar configuração por defeito - Marque "RESTORE DEFAULTS" na checkbox and clique Aceita.';
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
window.l_select_enter =        'Entre sala'
window.l_select_record =       'Grave'
window.l_select_moderated =    'Modere'
window.l_select_moderated_yes ='Sim'
window.l_select_moderated_no = 'Não'
window.l_select_visible =      'Membros Visíveis'
window.l_select_unvisible =    'Membros invisíveis'
window.l_select_duration =     'Duração'
window.l_select_delete =       'Remove sala'

window.l_not_found = 'não encontrado';

window.l_name_accept         = 'Aceita';
window.l_name_reset          = 'Reset';
window.l_name_user           = 'Utilizador';
window.l_name_host           = 'Host';
window.l_name_group          = 'Grupo';
window.l_name_password       = 'Senha';
window.l_name_roomname       = 'Nome da sala';
window.l_name_access         = 'Acesso';
window.l_name_action         = 'Ação';
window.l_name_code           = 'Código';
window.l_name_message        = 'Mensagem';
window.l_name_address        = 'Endereço';
window.l_name_address_book   = 'Livro de endereço';
window.l_name_save           = 'Salva';
window.l_name_cancel         = 'Cancela';
window.l_name_advanced       = 'Avançado';
window.l_name_auto_create              = 'Auto criação';
window.l_name_force_split_video        = 'Moderado';
window.l_name_auto_delete_empty        = 'Auto eliminação';
window.l_name_auto_record_not_empty    = 'Auto gravação';
window.l_name_recall_last_template     = 'Recarrega último modelo';
window.l_name_time_limit               = 'Limite de tempo';

window.l_name_display_name                         = 'Sobrepõe nome mostrado';
window.l_name_frame_rate_from_mcu                  = 'Frame rate da MCU';
window.l_name_bandwidth_from_mcu                   = 'Banda da MCU, Kbit/s';
window.l_name_bandwidth_to_mcu                     = 'Banda para MCU, Kbit/s';
window.l_name_transport                            = 'Transporte';
window.l_name_port                                 = 'porta';

window.l_name_registrar                            = 'Registrar';
window.l_name_account                              = 'Conta';
window.l_name_register                             = 'Registera';
window.l_name_address_sip_proxy                    = 'Endereço do SIP-proxy';
window.l_name_expires                              = 'Expira em';
window.l_name_path                                 = 'Caminho';
window.l_name_enable                               = 'Habilita';

window.l_name_registered                           = 'Registrado';
window.l_name_connected                            = 'Conectado';
window.l_name_last_ping_response                   = 'Última resposta';

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

window.l_room_invite_all_inactive_members          = 'Convida TODOS os membros inactivos?';
window.l_room_drop_all_active_members              = 'Desliga TODAS conexões activas?';
window.l_room_remove_all_inactive_members          = 'Remove TODOS membros inactivos da lista?';
window.l_room_drop_connection_with                 = 'Desliga conexões com';
window.l_room_remove_from_list                     = 'Remove da lista';

window.l_room_deletion_text = ["",                   "Closing room &laquo;%&raquo;: a desconectar participantes",
                                                     "Closing room &laquo;%&raquo;: aguardando participante sair da sala",
                                                     "Closing room &laquo;%&raquo;: desconectando membros escondidos de sistema",
                                                     "Closing room &laquo;%&raquo;: aguardando saída de membros da sala",
                                                     "Room &laquo;%&raquo; ELIMINADO"
];
