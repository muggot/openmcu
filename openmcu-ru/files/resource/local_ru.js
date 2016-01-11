window.l_welcome = 'OpenMCU-ru';
window.l_status = 'Состояние';

window.l_connections = 'Подключения';
window.l_connections_OFFLINE_PREFIX    = "<B>[Отключён] </B>"  ;
window.l_connections_OFFLINE_SUFFIX    = ""                    ;
window.l_connections_HIDDEN_PREFIX     = "<B>[Системный] </B>" ;
window.l_connections_HIDDEN_SUFFIX     = ""                    ;
window.l_connections_AUDIO_OUT_STR     = "Аудиовыход"          ;
window.l_connections_AUDIO_IN_STR      = "Аудиовход"           ;
window.l_connections_VIDEO_OUT_STR     = "Видеовыход"          ;
window.l_connections_VIDEO_IN_STR      = "Видеовход"           ;
window.l_connections_AI_NEG_ERR        = "Нет входящего аудио" ;
window.l_connections_AO_NEG_ERR        = "Нет исходящего аудио";
window.l_connections_VI_NEG_ERR        = "Нет входящего видео" ;
window.l_connections_VO_NEG_ERR        = "Нет исходящего видео";
window.l_connections_BUTTON_TEXT       = "Получить текст"      ;
window.l_connections_BUTTON_FORUM      = "Получить BBCode для форума";
window.l_connections_BUTTON_HTML       = "Получить HTML"       ;
window.l_connections_BUTTON_CLOSE      = "Закрыть код"         ;
window.l_connections_CODE_TOOLTIP      = "Нажмите Ctrl+C, чтобы скопировать код";
window.l_connections_DAYS_STR          = "д."                  ;
window.l_connections_COL_NAME          = "Имя"                 ;
window.l_connections_COL_DURATION      = "Длительность"        ;
window.l_connections_COL_RTP           = "RTP-канал и кодек"   ;
window.l_connections_COL_PACKETS       = "Пакеты"              ;
window.l_connections_COL_BYTES         = "Байты"               ;
window.l_connections_COL_KBPS          = "Кбит/с"              ;
window.l_connections_COL_FPS           = "Кадр/с"              ;
window.l_connections_word_room         = "Конференция"         ;
window.l_connections_COL_LOSTPCN       = "Потери за 60с"       ;

window.l_records = 'Видеозаписи';
window.l_control = 'Управление';
window.l_rooms = 'Комнаты';
window.l_invite = 'Позвонить';
window.l_invite_s = 'Вызов успешно выполнен';
window.l_invite_f = 'Вызов не выполнен';
window.l_settings = 'Настройки';
window.l_param_general = 'Основные';
window.l_param_registrar = 'Регистратор';
window.l_param_conference = 'Конференции';
window.l_param_export = 'Экспорт(named pipe)';
window.l_param_managing_users = 'Пользователи';
window.l_param_managing_groups = 'Группы';
window.l_param_control_codes = 'Коды управления';
window.l_param_room_codes = 'Коды комнат';
window.l_param_h323_endpoints = 'H.323 терминалы';
window.l_param_sip_endpoints = 'SIP терминалы';
window.l_param_rtsp = 'RTSP параметры';
window.l_param_rtsp_servers = 'RTSP серверы';
window.l_param_rtsp_endpoints = 'RTSP терминалы';
window.l_param_video = 'Видео';
window.l_param_record = 'Запись';
window.l_param_h323 = 'H323 параметры';
window.l_param_sip = 'SIP параметры';
window.l_param_sip_proxy = 'Учетные записи';
window.l_param_access_rules = 'Правила доступа';
window.l_param_receive_sound = 'Аудио кодеки (прием)';
window.l_param_transmit_sound = 'Аудио кодеки (отправка)';
window.l_param_receive_video = 'Видео кодеки (прием)';
window.l_param_transmit_video = 'Видео кодеки (отправка)';
window.l_param_sip_sound = 'Аудио кодеки';
window.l_param_sip_video = 'Видео кодеки';
window.l_help = 'Помощь';
window.l_forum = 'Форум'
window.l_manual = 'Документация';
window.l_support = 'Поддержка';

window.l_info_welcome ='\
<p>\
Служба OpenMCU-ru запущена и принимает подключения.\
</p>\
<p>\
Конференция, созданная подключением первого терминала, изначально работает в неуправляемом режиме, при этом все участники конференции видят одинаковую картинку. В неуправляемом режиме OpenMCU-ru автоматически меняет раскладку экрана в зависимости от количества подключенных пользователей, в порядке, определённом в файле "layouts.conf". После подключения 100 пользователей все последующие подключенные пользователи будут не видны на экранах терминалов, но будут видеть первых 100 пользователей.<br />\
Звук в неуправляемом режиме передается и воспроизводится от всех подключенных участников.<br />\
</p>\
<p>\
Для проверки работы сервера также можно позвонить в комнату с именем "echo". Или в комнату с именем "testroomN", где N - число от 1 до 100.<br />\
</p>\
<p>\
Управление сервером осуществляется через главное меню вверху страницы.<br />\
Страница <b>Состояние</b> позволяет просматривать текущие соединения с сервером и параметры подключенных терминалов.<br />\
На страница <b>Управление</b> можно перевести необходимые комнаты на ручное управление, для задания собственных раскладок конференции.<br />\
На странице <b>Настройки</b> задаются глобальные параметры работы сервера.<br />\
Дополнительную информацию можно найти на странице <b>Помощь</b> (ссылка ведет на официальный сайт в Интернет).<br />\
</p>\
<p>Ниже показана сводка по серверу.</p>\
';
window.l_welcome_logo ='\
<p>\
<b>Собственный логотип</b>\
</p>\
<p>\
Этот логотип показывается в микшере при отсутствии входящего видео. Допускается загрузка BMP, JPEG(не более 500kB), PNG, GIF.\
</p>\
';

window.l_info_invite ='\
Для подключения в поле Room Name нужно указать имя конференции \
к которой будет подключен терминал (для подключения к конференции \
по умолчанию нужно указать имя room101), в поле Address \
набрать IP адрес (или номер при использовании гейткипера) \
подключаемого терминала и нажать кнопочку Invite. \
В результате на терминал уйдет вызов.';

window.l_info_invite_f ='В запросе на подключение была допущена ошибка.';
window.l_info_invite_s ='Запрос на подключение успешно отправлен на удаленный терминал.';
window.l_info_rooms ='Для просмотра и управления комнатой - щелкните по ее имени.';

window.l_info_control ='\
<div id=\'tip\' name=\'tip\'\
  onclick=\'if(typeof savedhelpcontent=="undefined"){savedhelpcontent=this.innerHTML;this.innerHTML="Эта страница предназначена для управления конференцией. Управление раскладкой позволяет выбрать видимых участников конференции и назначить им нужное расположение на экране. Для каждой позиции в раскладке можно явно указать видимого участника конференции, либо оставить позицию пустой, либо указать, что позиция выбрана для автоматической активации по голосовой активности.<br>За автоматическую активацию по голосовой активности отвечают два режима VAD и VAD2. Если позиции в раскладке назначить режим VAD, то это будет означать, что позицию может занять любой терминал, с которого идет активный звуковой сигнал.<br>Замена терминала на занятой VAD позиции возможна по истечении интервала Voice activation timeout.";}else {this.innerHTML=savedhelpcontent; try {delete savedhelpcontent;} catch(e){savedhelpcontent=undefined;};}\'\
>Эта страница предназначена для\
 <span style=\'cursor:pointer;font-weight:bold;color:#095\'> ...</span>\
</div>';

window.l_info_records ='\
Страница позволяет просматривать содержимое директории (на сервере), \
в которой сохранены записи видеоконференций, и скачивать видеозаписи.';

window.l_info_connections ='\
Страница позволяет просматривать текущие соединения с сервером и параметры подключенных терминалов.\
';
window.l_info_param_general ='Общие параметры работы сервера.';
window.l_info_param_registrar = '';
window.l_info_param_conference ='';
window.l_info_param_managing_users ='';
window.l_info_param_managing_groups ='';
window.l_info_param_control_codes ='';
window.l_info_param_room_codes ='';
window.l_info_param_video ='Настройки качества исходящего видео.';
window.l_info_param_record ='Настройки качества записи.';
window.l_info_param_h323 ='';
window.l_info_param_sip ='';
window.l_info_param_access_rules ='';
window.l_info_param_sip_proxy ='';
window.l_info_param_receive_sound = '';
window.l_info_param_transmit_sound = '';
window.l_info_param_receive_video = '';
window.l_info_param_transmit_video = '';

window.l_select_create =       'Создать комнату'
window.l_select_enter =        'Управлять комнатой'
window.l_select_record =       'Запись'
window.l_select_moderated =    'Управляемый режим'
window.l_select_moderated_yes ='Да'
window.l_select_moderated_no = 'Нет'
window.l_select_visible =      'Видимых участников'
window.l_select_unvisible =    'Невидимых участников'
window.l_select_duration =     'Время работы'
window.l_select_delete =       'Удалить комнату'

window.l_not_found = 'не найден';

window.l_name_accept         = 'Сохранить';
window.l_name_reset          = 'Отменить';
window.l_name_user           = 'Пользователь';
window.l_name_host           = 'Хост';
window.l_name_group          = 'Группа';
window.l_name_password       = 'Пароль';
window.l_name_roomname       = 'Имя комнаты';
window.l_name_access         = 'Доступ';
window.l_name_action         = 'Действие';
window.l_name_code           = 'Код';
window.l_name_message        = 'Сообщение';
window.l_name_address        = 'Адрес';
window.l_name_address_book   = 'Адресная книга';
window.l_name_save           = 'Сохранить';
window.l_name_cancel         = 'Отменить';
window.l_name_advanced       = 'Расширенные';
window.l_name_auto_create              = 'Автоматическое создание';
window.l_name_force_split_video        = "Кэширование и управление через браузер";
window.l_name_auto_delete_empty        = 'Автоматическое удаление';
window.l_name_auto_record_start        = 'Автоматическая запись';
window.l_name_auto_record_stop         = 'Автоматическая запись (остановить)';
window.l_lock_tpl_default              = "Отключать терминалы, отсутствующие в шаблоне (запереть конференцию)";
window.l_name_recall_last_template     = 'Создать с последним шаблоном';
window.l_name_time_limit               = 'Ограничение по времени';

window.l_name_display_name                         = 'Отображаемое имя';
window.l_name_frame_rate_from_mcu                  = 'Частота кадров от MCU';
window.l_name_bandwidth_from_mcu                   = 'Битрейт от MCU, Кбит/с';
window.l_name_bandwidth_to_mcu                     = 'Битрейт к MCU, Кбит/с';
window.l_name_transport                            = 'Транспорт';
window.l_name_port                                 = 'порт';

window.l_name_registrar                            = 'Регистратор';
window.l_name_account                              = 'Учетная запись';
window.l_name_register                             = 'Регистрировать';
window.l_name_address_sip_proxy                    = 'Адрес SIP-прокси';
window.l_name_expires                              = 'Продолжительность регистрации';
window.l_name_path                                 = 'Путь';
window.l_name_enable                               = 'Включить';

window.l_name_registered                           = 'Зарегистрирован';
window.l_name_connected                            = 'Подключен';
window.l_name_last_ping_response                   = 'Последний отклик';

window.l_name_codec                                = 'Кодек';
window.l_name_audio                                = 'Аудио';
window.l_name_video                                = 'Видео';
window.l_name_audio_receive                        = 'Аудио (прием)';
window.l_name_audio_transmit                       = 'Аудио (передача)';
window.l_name_video_receive                        = 'Видео (прием)';
window.l_name_video_transmit                       = 'Видео (передача)';
window.l_name_audio_codec                          = 'Аудио кодек';
window.l_name_video_codec                          = 'Видео кодек';
window.l_name_audio_codec_receive                  = 'Аудио кодек(прием)';
window.l_name_video_codec_receive                  = 'Видео кодек(прием)';
window.l_name_audio_codec_transmit                 = 'Аудио кодек(передача)';
window.l_name_video_codec_transmit                 = 'Видео кодек(передача)';
window.l_name_video_resolution                     = 'Разрешение видео';

window.l_name_parameters_for_sending               = "Параметры для отправки";
window.l_name_codec_parameters                     = "Параметры кодека<br>(переопределить полученные)";
window.l_name_default_parameters                   = "Параметры по умолчанию";

window.l_room_mute_all                             = 'Отключить звук от всех участников (микрофоны)';
window.l_room_unmute_all                           = 'Восстановить звук от всех участников (микрофоны)';
window.l_room_invite_all_inactive_members          = 'Позвонить всем неактивным участникам';
window.l_room_dial_all_members                     = 'Дозвон всем участникам';
window.l_room_drop_all_active_members              = 'Отключить всех активных участников';
window.l_room_remove_all_inactive_members          = 'Удалить всех неактивных участников из списка';
window.l_room_drop_connection_with                 = 'Разорвать соединение с';
window.l_room_remove_from_list                     = 'Удалить из списка';

window.l_room_deletion_text = ["",                   "Удаление конференции &laquo;%&raquo;: отключение участников",
                                                     "Удаление конференции &laquo;%&raquo;: ждём, пока все уйдут",
                                                     "Удаление конференции &laquo;%&raquo;: отключение невидимых системных участников",
                                                     "Удаление конференции &laquo;%&raquo;: ждём, пока все уйдут",
                                                     "Конференция &laquo;%&raquo; УДАЛЕНА"
];

window.l_takecontrol                               = "Конференция работает в неуправляемом (автоматическом) режиме. Нажмите для управления конференцией вручную.";
window.l_decontrol                                 = "Вы управляете конференцией вручную. Нажмите для перевода конференции в неуправляемый (автоматический) режим.";
window.l_vadsetup                                  = "Настройка параметров активации по голосу (VAD)";
window.l_globalmute                                = "Звук от участников, не выведенных на экране, ВКЛЮЧЁН. Нажмите для отключения.";
window.l_globalunmute                              = "Звук от участников, не выведенных на экране, ОТКЛЮЧЁН. Нажмите для включения.";
window.l_filtermode                                = [
                                                       "Режим фильтрации изображения при изменении размера кадров: САМЫЙ БЫСТРЫЙ."
                                                      ,"Режим фильтрации изображения при изменении размера кадров: ОПТИМАЛЬНЫЙ (билинейная интерполяция)."
                                                      ,"Режим фильтрации изображения при изменении размера кадров: САМЫЙ КАЧЕСТВЕННЫЙ."
                                                     ];
window.l_videorecorder                             = "Начать запись конференции";
window.l_videorecorderstop                         = "Остановить запись конференции";
window.l_pleasetakecontrol                         = "Команда недоступна, поскольку конференция работает в НЕУПРАВЛЯЕМОМ (автоматическом) режиме";
window.l_decontrolmixersconfirm                    = "Вы переводите конференцию в НЕУПРАВЛЯЕМЫЙ (автоматический) режим.\r\nДополнительные видеомиксеры будут удалены, и останется только один.\r\nВы уверены, что хотите продолжить?";
window.l_templatedeleteconfirm                     = "Шаблон * будет удалён";
window.l_changelogo                                = "Заменить: ";

window.l_dir_no_records                            = "В этой директории пока нет ни одной сохранённой видеозаписи.";
window.l_download                                  = "Скачать";
window.l_totaldrivesize                            = "Общий размер дискового раздела: * ГиБ.";
window.l_recordstakesup                            = "Видеозаписи занимают:           * ГиБ (%).";
window.l_freespaceleft                             = "Свободно:                       * ГиБ (%).";
window.l_delete                                    = "Удалить";
window.l_recwilldeleted                            = "Видеозапись * будет удалена!";
window.l_recwilldeleted_ok                         = "[Подтверждаю]";
window.l_recwilldeleted_cancel                     = "Отмена";
window.l_recwasdeleted                             = "Видеозапись * удалена";
window.l_filesize                                  = "Размер файла";
window.l_resolution                                = "Разрешение";
window.l_startdatetime                             = "Дата и время начала";

///
window.l_restore_defaults                          = "Восстановить значения по умолчанию";
window.l_language                                  = "Language";
window.l_server_id                                 = "Идентификатор сервера OpenMCU-ru";
window.l_default_protocol_for_outgoing_calls       = "Протокол по умолчанию для исходящих звонков";
window.l_http_secure                               = "HTTPS";
window.l_http_certificate                          = "HTTPS сертификат";
window.l_http_ip                                   = "HTTP IP-адрес";
window.l_http_port                                 = "HTTP порт";
window.l_rtp_base_port                             = "RTP начальный порт";
window.l_rtp_max_port                              = "RTP максимальный порт";
window.l_trace_level                               = "Уровень трассировки";
window.l_rotate_trace                              = "Ротация файлов трассировки при запуске";
window.l_log_level                                 = "Уровень системного лога";
window.l_call_log_filename                         = "Файл журнала звонков";
window.l_room_control_event_buffer_size            = "Размер буфера событий веб-журнала";
window.l_copy_web_log                              = "Копировать веб-журнал в журнал звонков";
window.l_default_room                              = "Комната по умолчанию";
window.l_reject_duplicate_name                     = "Отклонить повторяющееся имя участника";
window.l_allow_loopback_calls                      = "Разрешить вызывать самого себя";
window.l_auto_dial_delay                           = "Интервал автодозвона, с";
///
window.l_allow_internal_calls                      = "Разрешить внутренние звонки";
window.l_sip_allow_reg_without_auth                = "SIP разрешить регистрацию без аутентификации";
window.l_sip_allow_mcu_calls_without_auth          = "SIP разрешить звонки в MCU без аутентификации";
window.l_sip_allow_internal_calls_without_auth     = "SIP разрешить внутренние звонки без аутентификации";
window.l_sip_registrar_minimum_expiration          = "SIP минимальный интервал регистрации";
window.l_sip_registrar_maximum_expiration          = "SIP максимальный интервал регистрации";
window.l_h323_gatekeeper_enable                    = "H.323 включить гейткипер";
window.l_h323_allow_reg_without_auth               = "H.323 разрешить регистрацию без аутентификации";
window.l_h323_allow_mcu_calls_without_reg          = "H.323 разрешить звонки в MCU без регистрации";
window.l_h323_allow_internal_calls_without_reg     = "H.323 разрешить внутренние звонки без регистрации";
window.l_h323_allow_duplicate_aliases              = "H.323 разрешить повторяющиеся алиасы";
window.l_h323_gatekeeper_minimum_ttl               = "H.323 минимальный интервал регистрации";
window.l_h323_gatekeeper_maximum_ttl               = "H.323 максимальный интервал регистрации";
///
window.l_enable_video                              = "Включить видео";
window.l_max_bit_rate                              = "Максимальный битрейт";
window.l_tx_key_frame_period                       = "Интервал отправки опорных кадров";
window.l_encoding_threads                          = "Количество потоков кодирования";
window.l_encoding_cpu_used                         = "Использование процессора для кодирования";
///
window.l_enable_export                             = "Включить экспорт";
window.l_video_frame_rate                          = "Видео частота кадров";
window.l_video_frame_width                         = "Видео ширина кадра";
window.l_video_frame_height                        = "Видео высота кадра";
window.l_audio_sample_rate                         = "Аудио частота дискретизации";
window.l_audio_channels                            = "Аудио количество каналов";
window.l_video_bitrate                             = "Видео битрейт";
window.l_audio_bitrate                             = "Аудио битрейт";
///
window.l_listener                                  = "Прослушиватель(listener)";
window.l_interface                                 = "Интерфейс";
window.l_nat_router_ip                             = "NAT IP-адрес роутера";
window.l_treat_as_global_for_nat                   = "Обрабатывать как глобальные для NAT";
window.l_disable_fast_start                        = "Отключить Fast-Start";
window.l_disable_h245_tunneling                    = "Отключить H.245 Tunneling";
window.l_gk_mode                                   = "Гейткипер: Тип поиска";
window.l_gk_reg_ttl                                = "Гейткипер: Продолжительность регистрации(TTL)";
window.l_gk_reg_retry_interval                     = "Гейткипер: Интервал попыток поиска и регистрации";
window.l_gk_host                                   = "Гейткипер: Хост";
window.l_gk_username                               = "Гейткипер: Имя пользователя";
window.l_gk_password                               = "Гейткипер: Пароль";
window.l_gk_room_names                             = "Гейткипер: Алиасы(имена комнат)";
///
window.l_server                                    = "Сервер";
window.l_server_list                               = "Список серверов";
///
window.l_directory                                 = "Директория";
window.l_rtp_input_timeout                         = "RTP таймаут";
window.l_received_vfu_delay                        = "Ограничение VFU, з/с";
window.l_video_cache                               = "Видео кэширование";
window.l_interval                                  = "интервал";
window.l_internal_call_processing                  = "Внутренние звонки";
window.l_room_auto_create_when_connecting          = "Автоматическое создание при подключении";
window.l_enter_template_name                       = "Введите имя шаблона";
window.l_disconnect                                = "Отключить";
window.l_add_to_abook                              = "Добавить в адресную книгу";
window.l_accounts                                  = "Учетные записи";
///
window.l_param_telserver = 'Telnet сервер';
window.l_info_telserver = '';
///
window.l_mute_new_conference_users                 = 'Заглушить новых участников конференции';
window.l_unmute_new_conference_users               = 'Включать звук от новых участников конференции';
