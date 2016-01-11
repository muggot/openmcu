window.l_welcome = 'OpenMCU-ru';
window.l_status = 'Statut';

window.l_connections = 'Connexions';
window.l_connections_OFFLINE_PREFIX    = "<B>[Hors Ligne] </B>" ;
window.l_connections_OFFLINE_SUFFIX    = ""                     ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Caché] </B>"   ;
window.l_connections_HIDDEN_SUFFIX     = ""                     ;
window.l_connections_AUDIO_OUT_STR     = "Sortie Audio"         ;
window.l_connections_AUDIO_IN_STR      = "Entrée Audio"      ;
window.l_connections_VIDEO_OUT_STR     = "Sortie Video"      ;
window.l_connections_VIDEO_IN_STR      = "Entrée video"   ;
window.l_connections_AI_NEG_ERR        = "Pas d'audio en entrée"       ;
window.l_connections_AO_NEG_ERR        = "Pas d'audio en sortie"      ;
window.l_connections_VI_NEG_ERR        = "Pas d'entrée video"       ;
window.l_connections_VO_NEG_ERR        = "Pas de sortie video"      ;
window.l_connections_BUTTON_TEXT       = "Obtenir Texte"             ;
window.l_connections_BUTTON_FORUM      = "Obtenir BBCode"           ;
window.l_connections_BUTTON_HTML       = "Obtenir HTML"             ;
window.l_connections_BUTTON_CLOSE      = "Fermer code"           ;
window.l_connections_CODE_TOOLTIP      = "Presser Ctrl+C pour copier";
window.l_connections_DAYS_STR          = "jour(s)"               ;
window.l_connections_COL_NAME          = "Nom"                 ;
window.l_connections_COL_DURATION      = "Durée"            ;
window.l_connections_COL_RTP           = "Canal RTP: Codec"  ;
window.l_connections_COL_PACKETS       = "Paquets"             ;
window.l_connections_COL_BYTES         = "Octets"               ;
window.l_connections_COL_KBPS          = "Kbit/s"              ;
window.l_connections_COL_FPS           = "IPS"                 ;
window.l_connections_word_room         = "Salle"                ;
window.l_connections_COL_LOSTPCN       = "Pertes 60s"          ;

window.l_records = 'Enregistrements';
window.l_control = 'Contrôle';
window.l_rooms = 'Salles';
window.l_invite = 'Inviter';
window.l_invite_s = 'Invitation réussie';
window.l_invite_f = 'Invitation échouée';
window.l_settings = 'Réglages';
window.l_param_general = 'Général';
window.l_param_registrar = 'Registrar';
window.l_param_conference = 'Conference';
window.l_param_export = 'Export(named pipe)';
window.l_param_managing_users = 'Utilisateurs';
window.l_param_managing_groups = 'Groupes';
window.l_param_control_codes = 'Codes de contrôle';
window.l_param_room_codes = 'Codes des salles';
window.l_param_h323_endpoints = 'Terminaux H.323';
window.l_param_sip_endpoints = 'Terminaux SIP';
window.l_param_rtsp = 'Paramètres RTSP';
window.l_param_rtsp_servers = 'Serveurs RTSP';
window.l_param_rtsp_endpoints = 'Terminaux RTSP';
window.l_param_video = 'Vidéo';
window.l_param_record = 'Enregistrer';
window.l_param_h323 = 'Paramètres H323';
window.l_param_sip = 'Paramètres SIP';
window.l_param_sip_proxy = 'Informations sur le compte';
window.l_param_access_rules = 'Règles d\'accès';
window.l_param_receive_sound = 'Codecs Audio (réception)';
window.l_param_transmit_sound = 'Codecs Audio (emission)';
window.l_param_receive_video = 'Codecs Vidéo (emission)';
window.l_param_transmit_video = 'Codecs Vidéo (réception)';
window.l_param_sip_sound = 'Codecs Audio';
window.l_param_sip_video = 'Codecs Vidéo';
window.l_help = 'Aide';
window.l_forum = 'Forum';
window.l_manual = 'Manuel';
window.l_support = 'Support';

window.l_info_welcome ='\
<p>\
Le service OpenMCU-ru est lancé et accepte les connexions\
</p>\
<p>\
Une conférence initiée par un terminal fonctionnera en mode non managé. Tous les membres verront la même image. En mode non managé OpenMCU-ru change automatiquement la disposition en fonction du nombre de membres dans la conférence, dans l\'ordre défini dans le fichier "layouts.conf". Seuls les 100 premiers membres d\'une conférence seront visibles.<br/>\
En mode non managé, le flux audio est transmis vers et depuis tous les membres connectés<br/>\
</p>\
<p>\
Pour tester le server, appelez la salle "echo" ou la salle "testroomN", ou N est un nombre de 1 à 100.<br/>\
</p>\
<p>\
Le management du serveur est effectué depuis le menu principal en haut de la page.<br/>\
La page <b>Statut</b> montre les connections actives et les paramètres des terminaux.<br />\
La page <b>Contrôle</b> sert à administer les salles de conférence et à les passer en mode managé.<br />\
La page <b>Paramètres</b> sert à régler les paramètres généraux.<br />\
Plus d\'informations sont disponibles dans l\'<b>Aide</b> (lien vers le site officiel).<br />\
</p>\
<p>Un résumé du serveur est disponible ci-dessous.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Image de logo personnalisée</b>\
</p>\
<p>\
Cette image est montrée en l\'absence de source vidéo dans le mixer. Format BMP, JPEG(maximum 500kB), PNG, GIF.\
</p>\
';

window.l_info_invite ='';
window.l_info_invite_f ='';
window.l_info_invite_s ='';
window.l_info_rooms ='Pour voir la salle et la passer en mode managé, cliquer sur son nom.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="Cette page sert à gérer votre conférence. Vous pouvez choisir des participants depuis la liste et les placer dans la partie désirer de l`écran grâce au contrôle des dispositions (Via drag-n-drop). Chaque position peut être assignée à n\'importe lequel des participants (de façon statique), ou activée par la voix.<br>Les positions activées par la voix peuvent être marquées comme VAD ou VAD2 (Les participants les plus actifs seront automatiquement déplacés de VAD à VAD2).";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>Cette page est utilisée pour\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Cette page montre le dossier des enregistrements vidéo (sur le serveur)\
et permet de les télécharger.';

window.l_info_connections ='\
Cette page montre les connexions actives et les paramêtres des terminaux.\
';
window.l_info_param_general ='Paramètres globaux du serveur. Pour remettre tous les paramètres à zéro, cocher la case "RESTAURER PAR DEFAUT" et cliquer sur accepter.';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_info_param_managing_users = '';
window.l_info_param_managing_groups = '';
window.l_info_param_control_codes = '';
window.l_info_param_room_codes = '';
window.l_info_param_video ='Qualité sortie vidéo.';
window.l_info_param_record ='Qualité enregistrement.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Créer Salle'
window.l_select_enter =        'Entrer dans la salle'
window.l_select_record =       'Enregistrer'
window.l_select_moderated =    'Modéré'
window.l_select_moderated_yes ='Oui'
window.l_select_moderated_no = 'Non'
window.l_select_visible =      'Membres visibles'
window.l_select_unvisible =    'Membres invisibles'
window.l_select_duration =     'Durée'
window.l_select_delete =       'Supprimer la salle'

window.l_not_found = 'non trouvé';

window.l_name_accept         = 'Accepter';
window.l_name_reset          = 'Réinitialiser';
window.l_name_user           = 'Utilisateur';
window.l_name_host           = 'Hôte';
window.l_name_group          = 'Groupe';
window.l_name_password       = 'Mot de passe';
window.l_name_roomname       = 'Nom de la salle';
window.l_name_access         = 'Accès';
window.l_name_action         = 'Action';
window.l_name_code           = 'Code';
window.l_name_message        = 'Message';
window.l_name_address        = 'Adresse';
window.l_name_address_book   = 'Répertoire';
window.l_name_save           = 'Sauver';
window.l_name_cancel         = 'Annuler';
window.l_name_advanced       = 'Avancé';
window.l_name_auto_create              = 'Création auto';
window.l_name_auto_delete_empty        = 'Suppression auto';
window.l_name_auto_record_start        = 'Enregistrement auto';
window.l_name_recall_last_template     = 'Rappel du dernier template';
window.l_name_time_limit               = 'Limite de temps';

window.l_name_display_name                         = 'Forcer nom affiché';
window.l_name_frame_rate_from_mcu                  = 'Framerate depuis MCU';
window.l_name_bandwidth_from_mcu                   = 'BP depuis MCU, Kbit/s';
window.l_name_bandwidth_to_mcu                     = 'BP vers MCU, Kbit/s';
window.l_name_transport                            = 'Transport';
window.l_name_port                                 = 'port';

window.l_name_registrar                            = 'Registrar';
window.l_name_account                              = 'Compte';
window.l_name_register                             = 'Register';
window.l_name_address_sip_proxy                    = 'Adresse du proxy SIP';
window.l_name_expires                              = 'Expires';
window.l_name_path                                 = 'Chemin';
window.l_name_enable                               = 'Activer';

window.l_name_registered                           = 'Enregistré';
window.l_name_connected                            = 'Connecté';
window.l_name_last_ping_response                   = 'Dernière réponse';

window.l_name_codec                                = 'Codec';
window.l_name_audio                                = 'Audio';
window.l_name_video                                = 'Vidéo';
window.l_name_audio_receive                        = 'Audio (réception)';
window.l_name_audio_transmit                       = 'Audio (émission)';
window.l_name_video_receive                        = 'Vidéo (réception)';
window.l_name_video_transmit                       = 'Vidéo (émission)';
window.l_name_audio_codec                          = 'Codec audio';
window.l_name_video_codec                          = 'Codec vidéo';
window.l_name_audio_codec_receive                  = 'Codec audio(réception)';
window.l_name_video_codec_receive                  = 'Codec vidéo(réception)';
window.l_name_audio_codec_transmit                 = 'Audio codec(émission)';
window.l_name_video_codec_transmit                 = 'Codec vidéo(émission)';
window.l_name_video_resolution                     = 'Résolution vidéo';

window.l_name_parameters_for_sending               = "Paramètres d'envoi";
window.l_name_codec_parameters                     = "Paramètres codec<br>(ignorer celui reçu)";
window.l_name_default_parameters                   = "Paramètres par défaut";

window.l_room_invite_all_inactive_members          = 'Inviter TOUS les membres inactifs';
window.l_room_drop_all_active_members              = 'Fermer TOUTES les connexions actives';
window.l_room_remove_all_inactive_members          = 'Retirer TOUS les membres inactifs de la liste';
window.l_room_drop_connection_with                 = 'Fermer la connexion avec';
window.l_room_remove_from_list                     = 'Retirer de la liste';

window.l_room_deletion_text = ["",                   "Suppression de la salle &laquo;%&raquo;: déconnexion des participants",
                                                     "Suppression de la salle &laquo;%&raquo;: en attente de la déconnexion de tous les participants",
                                                     "Suppression de la salle &laquo;%&raquo;: déconnexion des membres système cachés",
                                                     "Suppression de la salle &laquo;%&raquo;: en attente de la sortie de tous les participants",
                                                     "Salle &laquo;%&raquo; SUPPRIMEE"
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
